#include "../../../../disco/tiles.h"

#include "generated/pack_seccomp.h"

#include "../../../../disco/topo/fd_pod_format.h"
#include "../../../../disco/shred/fd_shredder.h"
#include "../../../../disco/metrics/fd_metrics.h"
#include "../../../../ballet/pack/fd_pack.h"

#include <linux/unistd.h>

/* fd_pack is responsible for taking verified transactions, and
   arranging them into "microblocks" (groups) of transactions to
   be executed serially.  It can try to do clever things so that
   multiple microblocks can execute in parallel, if they don't
   write to the same accounts. */

#define IN_KIND_DEDUP  (0UL)
#define IN_KIND_POH    (1UL)
#define IN_KIND_BANK   (2UL)
#define IN_KIND_BUNDLE (3UL)

#define MAX_SLOTS_PER_EPOCH          432000UL

/* Pace microblocks, but only slightly.  This helps keep performance
   more stable.  This limit is 2,000 microblocks/second/bank.  At 31
   transactions/microblock, that's 62k txn/sec/bank. */
#define MICROBLOCK_DURATION_NS  (500000L)

#define TRANSACTION_LIFETIME_NS (60UL*1000UL*1000UL*1000UL) /* 60s */

/* About 6 kB on the stack */
#define FD_PACK_PACK_MAX_OUT FD_PACK_MAX_BANK_TILES

/* Time is normally a long, but pack expects a ulong.  Add -LONG_MIN to
   the time values so that LONG_MIN maps to 0, LONG_MAX maps to
   ULONG_MAX, and everything in between maps linearly with a slope of 1.
   Just subtracting LONG_MIN results in signed integer overflow, which
   is U.B. */
#define TIME_OFFSET 0x8000000000000000UL
FD_STATIC_ASSERT( (ulong)LONG_MIN+TIME_OFFSET==0UL,       time_offset );
FD_STATIC_ASSERT( (ulong)LONG_MAX+TIME_OFFSET==ULONG_MAX, time_offset );


/* Optionally allow a larger limit for benchmarking */
#define LARGER_MAX_COST_PER_BLOCK (18UL*48000000UL)

/* 1.5 M cost units, enough for 1 max size transaction */
const ulong CUS_PER_MICROBLOCK = 1500000UL;

const float VOTE_FRACTION = 0.75;

/* There's overhead associated with each microblock the bank tile tries
   to execute it, so the optimal strategy is not to produce a microblock
   with a single transaction as soon as we receive it.  Basically, if we
   have less than 31 transactions, we want to wait a little to see if we
   receive additional transactions before we schedule a microblock.  We
   can model the optimum amount of time to wait, but the equation is
   complicated enough that we want to compute it before compile time.
   wait_duration[i] for i in [0, 31] gives the time in nanoseconds pack
   should wait after receiving its most recent transaction before
   scheduling if it has i transactions available.  Unsurprisingly,
   wait_duration[31] is 0.  wait_duration[0] is ULONG_MAX, so we'll
   always wait if we have 0 transactions. */
FD_IMPORT( wait_duration, "src/ballet/pack/pack_delay.bin", ulong, 6, "" );


/* When we are done being leader for a slot and we are leader in the
   very next slot, it can still take some time to transition.  This is
   because the bank has to be finalized, a hash calculated, and various
   other things done in the replay stage to create the new child bank.

   During that time, pack cannot send transactions to banks so it needs
   to be able to buffer.  Typically, these so called "leader
   transitions" are short (<15 millis), so a low value here would
   suffice.  However, in some cases when there is memory pressure on the
   NUMA node or when the operating system context switches relevant
   threads out, it can take significantly longer.

   To prevent drops in these cases and because we assume banks are fast
   enough to drain this buffer once we do become leader, we set this
   buffer size to be quite large. */

#define DEQUE_NAME extra_txn_deq
#define DEQUE_T    fd_txn_p_t
#define DEQUE_MAX  (128UL*1024UL)
#include "../../../../util/tmpl/fd_deque.c"


typedef struct {
  fd_wksp_t * mem;
  ulong       chunk0;
  ulong       wmark;
} fd_pack_in_ctx_t;

typedef struct {
  fd_pack_t *  pack;
  fd_txn_p_t * cur_spot;

  /* The value passed to fd_pack_new, etc. */
  ulong    max_pending_transactions;

  /* The leader slot we are currently packing for, or ULONG_MAX if we
     are not the leader. */
  ulong  leader_slot;
  void const * leader_bank;

  /* The number of microblocks we have packed for the current leader
     slot.  Will always be <= slot_max_microblocks.  We must track
     this so that when we are done we can tell the PoH tile how many
     microblocks to expect in the slot. */
  ulong slot_microblock_cnt;

  /* The maximum number of microblocks that can be packed in this slot.
     Provided by the PoH tile when we become leader.*/
  ulong slot_max_microblocks;

  /* Cap (in bytes) of the amount of transaction data we produce in each
     block to avoid hitting the shred limits.  See where this is set for
     more explanation. */
  ulong slot_max_data;
  int   larger_shred_limits_per_block;

  /* If drain_banks is non-zero, then the pack tile must wait until all
     banks are idle before scheduling any more microblocks.  This is
     primarily helpful in irregular leader transitions, e.g. while being
     leader for slot N, we switch forks to a slot M (!=N+1) in which we
     are also leader.  We don't want to execute microblocks for
     different slots concurrently. */
  int drain_banks;

  /* Updated during housekeeping and used only for checking if the
     leader slot has ended.  Might be off by one housekeeping duration,
     but that should be small relative to a slot duration. */
  long  approx_wallclock_ns;

  fd_rng_t * rng;

  /* The end wallclock time of the leader slot we are currently packing
     for, if we are currently packing for a slot.

     _slot_end_ns is used as a temporary between during_frag and
     after_frag in case the tile gets overrun. */
  long _slot_end_ns;
  long slot_end_ns;

  /* last_successful_insert stores the tickcount of the last
     successful transaction insert. */
  long last_successful_insert;

  /* transaction_lifetime_ns, microblock_duration_ns, and wait_duration
     respectively scaled to be in ticks instead of nanoseconds */
  ulong transaction_lifetime_ticks;
  ulong microblock_duration_ticks;
  ulong wait_duration_ticks[ MAX_TXN_PER_MICROBLOCK+1UL ];

  /* In addition to the available transactions that pack knows about, we
     also store a larger ring buffer for handling cases when pack is
     full.  This is an fd_deque. */
  fd_txn_p_t * extra_txn_deq;
  int          insert_to_extra; /* whether the last insert was into pack or the extra deq */

  fd_pack_in_ctx_t in[ 32 ];
  int              in_kind[ 32 ];

  ulong    bank_cnt;
  ulong    bank_idle_bitset; /* bit i is 1 if we've observed *bank_current[i]==bank_expect[i] */
  int      poll_cursor; /* in [0, bank_cnt), the next bank to poll */
  int      use_consumed_cus;
  long     skip_cnt;
  ulong *  bank_current[ FD_PACK_PACK_MAX_OUT ];
  ulong    bank_expect[ FD_PACK_PACK_MAX_OUT  ];
  /* bank_ready_at[x] means don't check bank x until tickcount is at
     least bank_ready_at[x]. */
  long     bank_ready_at[ FD_PACK_PACK_MAX_OUT  ];

  fd_wksp_t * out_mem;
  ulong       out_chunk0;
  ulong       out_wmark;
  ulong       out_chunk;

  ulong      insert_result[ FD_PACK_INSERT_RETVAL_CNT ];
  fd_histf_t schedule_duration[ 1 ];
  fd_histf_t insert_duration  [ 1 ];

  struct {
    uint metric_state;
    long metric_state_begin;
    long metric_timing[ 16 ];
  };

  /* Used between during_frag and after_frag */
  ulong pending_rebate_cnt;
  fd_txn_p_t pending_rebate[ MAX_TXN_PER_MICROBLOCK ]; /* indexed [0, pending_rebate_cnt) */
} fd_pack_ctx_t;


#define FD_PACK_METRIC_STATE_TRANSACTIONS 0
#define FD_PACK_METRIC_STATE_BANKS        1
#define FD_PACK_METRIC_STATE_LEADER       2
#define FD_PACK_METRIC_STATE_MICROBLOCKS  3

/* Updates one component of the metric state.  If the state has changed,
   records the change. */
static inline void
update_metric_state( fd_pack_ctx_t * ctx,
                     long            effective_as_of,
                     int             type,
                     int             status ) {
  uint current_state = fd_uint_insert_bit( ctx->metric_state, type, status );
  if( FD_UNLIKELY( current_state!=ctx->metric_state ) ) {
    ctx->metric_timing[ ctx->metric_state ] += effective_as_of - ctx->metric_state_begin;
    ctx->metric_state_begin = effective_as_of;
    ctx->metric_state = current_state;
  }
}


FD_FN_CONST static inline ulong
scratch_align( void ) {
  return 4096UL;
}

FD_FN_PURE static inline ulong
scratch_footprint( fd_topo_tile_t const * tile ) {
  fd_pack_limits_t limits[1] = {{
    .max_cost_per_block        = tile->pack.larger_max_cost_per_block ? LARGER_MAX_COST_PER_BLOCK : FD_PACK_MAX_COST_PER_BLOCK,
    .max_vote_cost_per_block   = FD_PACK_MAX_VOTE_COST_PER_BLOCK,
    .max_write_cost_per_acct   = FD_PACK_MAX_WRITE_COST_PER_ACCT,
    .max_data_bytes_per_block  = tile->pack.larger_shred_limits_per_block ? LARGER_MAX_DATA_PER_BLOCK : FD_PACK_MAX_DATA_PER_BLOCK,
    .max_txn_per_microblock    = MAX_TXN_PER_MICROBLOCK,
    .max_microblocks_per_block = (ulong)UINT_MAX, /* Limit not known yet */
  }};

  ulong l = FD_LAYOUT_INIT;
  l = FD_LAYOUT_APPEND( l, alignof( fd_pack_ctx_t ), sizeof( fd_pack_ctx_t )                                   );
  l = FD_LAYOUT_APPEND( l, fd_rng_align(),           fd_rng_footprint()                                        );
  l = FD_LAYOUT_APPEND( l, fd_pack_align(),          fd_pack_footprint( tile->pack.max_pending_transactions,
                                                                        tile->pack.bank_tile_count,
                                                                        limits                               ) );
  l = FD_LAYOUT_APPEND( l, extra_txn_deq_align(),    extra_txn_deq_footprint()                                 );
  return FD_LAYOUT_FINI( l, scratch_align() );
}

static inline void
metrics_write( fd_pack_ctx_t * ctx ) {
  FD_MCNT_ENUM_COPY( PACK, TRANSACTION_INSERTED,          ctx->insert_result  );
  FD_MCNT_ENUM_COPY( PACK, METRIC_TIMING,        ((ulong*)ctx->metric_timing) );
  FD_MHIST_COPY( PACK, SCHEDULE_MICROBLOCK_DURATION_SECONDS, ctx->schedule_duration );
  FD_MHIST_COPY( PACK, INSERT_TRANSACTION_DURATION_SECONDS,  ctx->insert_duration   );

  fd_pack_metrics_write( ctx->pack );
}

static inline void
during_housekeeping( fd_pack_ctx_t * ctx ) {
  ctx->approx_wallclock_ns = fd_log_wallclock();
}

static inline void
before_credit( fd_pack_ctx_t *     ctx,
               fd_stem_context_t * stem ) {
  (void)stem;

  if( FD_UNLIKELY( ctx->cur_spot ) ) {
    /* If we were overrun while processing a frag from an in, then cur_spot
       is left dangling and not cleaned up, so clean it up here (by returning
       the slot to the pool of free slots). */
    if( FD_LIKELY( !ctx->insert_to_extra ) ) fd_pack_insert_txn_cancel( ctx->pack, ctx->cur_spot );
    else                                     extra_txn_deq_remove_tail( ctx->extra_txn_deq       );
    ctx->cur_spot = NULL;
  }
}

/* insert_from_extra: helper method to pop the transaction at the head
   off the extra txn deque and insert it into pack.  Requires that
   ctx->extra_txn_deq is non-empty, but it's okay to call it if pack is
   full.  Returns the result of fd_pack_insert_txn_fini. */
static inline int
insert_from_extra( fd_pack_ctx_t * ctx ) {
  fd_txn_p_t       * spot       = fd_pack_insert_txn_init( ctx->pack );
  fd_txn_p_t const * insert     = extra_txn_deq_peek_head( ctx->extra_txn_deq );
  fd_txn_t   const * insert_txn = TXN(insert);
  fd_memcpy( spot->payload, insert->payload, insert->payload_sz                                                           );
  fd_memcpy( TXN(spot),     insert_txn,      fd_txn_footprint( insert_txn->instr_cnt, insert_txn->addr_table_lookup_cnt ) );
  spot->payload_sz = insert->payload_sz;
  extra_txn_deq_remove_head( ctx->extra_txn_deq );

  /* Unpack the time stashed in the CUs field */
  long now = (long)((((ulong)insert->executed_cus)<<32) | ((ulong)insert->requested_cus));

  long insert_duration = -fd_tickcount();
  int result = fd_pack_insert_txn_fini( ctx->pack, spot, (ulong)now+TIME_OFFSET );
  insert_duration      += fd_tickcount();
  ctx->insert_result[ result + FD_PACK_INSERT_RETVAL_OFF ]++;
  fd_histf_sample( ctx->insert_duration, (ulong)insert_duration );
  return result;
}

static inline void
after_credit( fd_pack_ctx_t *     ctx,
              fd_stem_context_t * stem,
              int *               opt_poll_in ) {
  (void)opt_poll_in;

  if( FD_UNLIKELY( (ctx->skip_cnt--)>0L ) ) return; /* It would take ages for this to hit LONG_MIN */

  long now = fd_tickcount();

  ulong bank_cnt = ctx->bank_cnt;

  /* If we're using CU rebates, then we have one in for each bank in
     addition to the two normal ones.  That means that after_credit will
     be called about (bank_cnt/2) times more frequently per transaction
     we receive. */
  fd_long_store_if( ctx->use_consumed_cus, &(ctx->skip_cnt), (long)(bank_cnt/2UL) );

  /* If any banks are busy, check one of the busy ones see if it is
     still busy. */
  if( FD_LIKELY( ctx->bank_idle_bitset!=fd_ulong_mask_lsb( (int)bank_cnt ) ) ) {
    int   poll_cursor = ctx->poll_cursor;
    ulong busy_bitset = (~ctx->bank_idle_bitset) & fd_ulong_mask_lsb( (int)bank_cnt );

    /* Suppose bank_cnt is 4 and idle_bitset looks something like this
       (pretending it's a uchar):
                0000 1001
                       ^ busy cursor is 1
       Then busy_bitset is
                0000 0110
       Rotate it right by 2 bits
                1000 0001
       Find lsb returns 0, so busy cursor remains 2, and we poll bank 2.

       If instead idle_bitset were
                0000 1110
                       ^
       The rotated version would be
                0100 0000
       Find lsb will return 6, so busy cursor would be set to 0, and
       we'd poll bank 0, which is the right one. */
    poll_cursor++;
    poll_cursor = (poll_cursor + fd_ulong_find_lsb( fd_ulong_rotate_right( busy_bitset, (poll_cursor&63) ) )) & 63;

    if( FD_UNLIKELY( (fd_fseq_query( ctx->bank_current[poll_cursor] )==ctx->bank_expect[poll_cursor]) &
                     (ctx->bank_ready_at[poll_cursor]<now) ) ) {
      ctx->bank_idle_bitset |= 1UL<<poll_cursor;
    }

    ctx->poll_cursor = poll_cursor;
  }


  /* If we time out on our slot, then stop being leader.  This can only
     happen in the first after_credit after a housekeeping. */
  if( FD_UNLIKELY( ctx->approx_wallclock_ns>=ctx->slot_end_ns && ctx->leader_slot!=ULONG_MAX ) ) {
    if( FD_UNLIKELY( ctx->slot_microblock_cnt<ctx->slot_max_microblocks )) {
      /* As an optimization, The PoH tile will automatically end a slot
         if it receives the maximum allowed microblocks, since it knows
         there is nothing left to receive.  In that case, we don't need
         to send a DONE_PACKING notification, since they are already on
         the next slot.  If we did send one it would just get dropped. */
      fd_done_packing_t * done_packing = fd_chunk_to_laddr( ctx->out_mem, ctx->out_chunk );
      done_packing->microblocks_in_slot = ctx->slot_microblock_cnt;

      fd_stem_publish( stem, 0UL, fd_disco_poh_sig( ctx->leader_slot, POH_PKT_TYPE_DONE_PACKING, ULONG_MAX ), ctx->out_chunk, sizeof(fd_done_packing_t), 0UL, 0UL, 0UL );
      ctx->out_chunk = fd_dcache_compact_next( ctx->out_chunk, sizeof(fd_done_packing_t), ctx->out_chunk0, ctx->out_wmark );
    }

    ctx->drain_banks         = 1;
    ctx->leader_slot         = ULONG_MAX;
    ctx->slot_microblock_cnt = 0UL;
    fd_pack_end_block( ctx->pack );
    update_metric_state( ctx, now, FD_PACK_METRIC_STATE_LEADER,       0 );
    update_metric_state( ctx, now, FD_PACK_METRIC_STATE_BANKS,        0 );
    update_metric_state( ctx, now, FD_PACK_METRIC_STATE_MICROBLOCKS,  0 );
    return;
  }

  /* Am I leader? If not, see about inserting at most one transaction
     from extra storage.  It's important not to insert too many
     transactions here, or we won't end up servicing dedup_pack enough.
     If extra storage is empty or pack is full, do nothing. */
  if( FD_UNLIKELY( ctx->leader_slot==ULONG_MAX ) ) {
    if( FD_UNLIKELY( !extra_txn_deq_empty( ctx->extra_txn_deq ) &&
         fd_pack_avail_txn_cnt( ctx->pack )<ctx->max_pending_transactions ) ) {
      int result = insert_from_extra( ctx );
      if( FD_LIKELY( result>=0 ) ) ctx->last_successful_insert = now;
    }
    return;
  }

  /* Am I in drain mode?  If so, check if I can exit it */
  if( FD_UNLIKELY( ctx->drain_banks ) ) {
    if( FD_LIKELY( ctx->bank_idle_bitset==fd_ulong_mask_lsb( (int)bank_cnt ) ) ) ctx->drain_banks = 0;
    else                                                                         return;
  }

  /* Have I sent the max allowed microblocks? Nothing to do. */
  if( FD_UNLIKELY( ctx->slot_microblock_cnt>=ctx->slot_max_microblocks ) ) return;

  /* Do I have enough transactions and/or have I waited enough time? */
  if( FD_UNLIKELY( (ulong)(now-ctx->last_successful_insert) <
        ctx->wait_duration_ticks[ fd_ulong_min( fd_pack_avail_txn_cnt( ctx->pack ), MAX_TXN_PER_MICROBLOCK ) ] ) ) {
    update_metric_state( ctx, now, FD_PACK_METRIC_STATE_TRANSACTIONS, 0 );
    return;
  }

  int any_ready     = 0;
  int any_scheduled = 0;


  /* Try to schedule the next microblock.  Do we have any idle bank
     tiles? */
  if( FD_LIKELY( ctx->bank_idle_bitset ) ) { /* Optimize for schedule */
    any_ready = 1;

    int i               = fd_ulong_find_lsb( ctx->bank_idle_bitset );

    /* TODO: You can maybe make the case that this should happen as soon
       as we detect the bank has become idle, but doing it now probably
       helps with account locality. */
    fd_pack_microblock_complete( ctx->pack, (ulong)i );

    ulong exp_cnt = fd_pack_expire_before( ctx->pack, fd_ulong_max( (ulong)now+TIME_OFFSET, ctx->transaction_lifetime_ticks )-ctx->transaction_lifetime_ticks );
    FD_MCNT_INC( PACK, TRANSACTION_EXPIRED, exp_cnt );

    void * microblock_dst = fd_chunk_to_laddr( ctx->out_mem, ctx->out_chunk );
    long schedule_duration = -fd_tickcount();
    ulong schedule_cnt = fd_pack_schedule_next_microblock( ctx->pack, CUS_PER_MICROBLOCK, VOTE_FRACTION, (ulong)i, microblock_dst );
    schedule_duration      += fd_tickcount();
    fd_histf_sample( ctx->schedule_duration, (ulong)schedule_duration );

    if( FD_LIKELY( schedule_cnt ) ) {
      any_scheduled = 1;
      ulong tspub  = (ulong)fd_frag_meta_ts_comp( fd_tickcount() );
      ulong chunk  = ctx->out_chunk;
      ulong msg_sz = schedule_cnt*sizeof(fd_txn_p_t);
      fd_microblock_bank_trailer_t * trailer = (fd_microblock_bank_trailer_t*)((uchar*)microblock_dst+msg_sz);
      trailer->bank = ctx->leader_bank;

      ulong sig = fd_disco_poh_sig( ctx->leader_slot, POH_PKT_TYPE_MICROBLOCK, (ulong)i );
      fd_stem_publish( stem, 0UL, sig, chunk, msg_sz+sizeof(fd_microblock_bank_trailer_t), 0UL, 0UL, tspub );
      ctx->bank_expect[ i ] = stem->seqs[0]-1UL;
      ctx->bank_ready_at[i] = now + (long)ctx->microblock_duration_ticks;
      ctx->out_chunk = fd_dcache_compact_next( ctx->out_chunk, msg_sz+sizeof(fd_microblock_bank_trailer_t), ctx->out_chunk0, ctx->out_wmark );
      ctx->slot_microblock_cnt++;

      ctx->bank_idle_bitset = fd_ulong_pop_lsb( ctx->bank_idle_bitset );
      ctx->skip_cnt         = (long)schedule_cnt * fd_long_if( ctx->use_consumed_cus, (long)bank_cnt/2L, 1L );
    }
  }

  update_metric_state( ctx, now, FD_PACK_METRIC_STATE_BANKS,       any_ready     );
  update_metric_state( ctx, now, FD_PACK_METRIC_STATE_MICROBLOCKS, any_scheduled );
  now = fd_tickcount();
  update_metric_state( ctx, now, FD_PACK_METRIC_STATE_TRANSACTIONS, fd_pack_avail_txn_cnt( ctx->pack )>0 );

  if( FD_UNLIKELY( !extra_txn_deq_empty( ctx->extra_txn_deq ) ) ) {
    /* Don't start pulling from the extra storage until the available
       transaction count drops below half. */
    ulong avail_space   = (ulong)fd_long_max( 0L, (long)(ctx->max_pending_transactions>>1)-(long)fd_pack_avail_txn_cnt( ctx->pack ) );
    ulong qty_to_insert = fd_ulong_min( 10UL, fd_ulong_min( extra_txn_deq_cnt( ctx->extra_txn_deq ), avail_space ) );
    int any_successes = 0;
    for( ulong i=0UL; i<qty_to_insert; i++ ) any_successes |= (0<=insert_from_extra( ctx ));
    if( FD_LIKELY( any_successes ) ) ctx->last_successful_insert = now;
    FD_MCNT_INC( PACK, TRANSACTION_INSERTED_FROM_EXTRA, qty_to_insert );
  }

  /* Did we send the maximum allowed microblocks? Then end the slot. */
  if( FD_UNLIKELY( ctx->slot_microblock_cnt==ctx->slot_max_microblocks )) {
    update_metric_state( ctx, now, FD_PACK_METRIC_STATE_LEADER,       0 );
    update_metric_state( ctx, now, FD_PACK_METRIC_STATE_BANKS,        0 );
    update_metric_state( ctx, now, FD_PACK_METRIC_STATE_MICROBLOCKS,  0 );
    ctx->drain_banks         = 1;
    ctx->leader_slot         = ULONG_MAX;
    ctx->slot_microblock_cnt = 0UL;
    fd_pack_end_block( ctx->pack );
  }
}


/* At this point, we have started receiving frag seq with details in
    mline at time now.  Speculatively process it here. */

static inline void
during_frag( fd_pack_ctx_t * ctx,
             ulong           in_idx,
             ulong           seq,
             ulong           sig,
             ulong           chunk,
             ulong           sz ) {
  (void)seq;

  uchar const * dcache_entry = fd_chunk_to_laddr_const( ctx->in[ in_idx ].mem, chunk );

  switch( ctx->in_kind[ in_idx ] ) {
  case IN_KIND_POH: {
      /* Not interested in stamped microblocks, only leader updates. */
    if( fd_disco_poh_sig_pkt_type( sig )!=POH_PKT_TYPE_BECAME_LEADER ) return;

    /* There was a leader transition.  Handle it. */
    if( FD_UNLIKELY( chunk<ctx->in[ in_idx ].chunk0 || chunk>ctx->in[ in_idx ].wmark || sz!=sizeof(fd_became_leader_t) ) )
      FD_LOG_ERR(( "chunk %lu %lu corrupt, not in range [%lu,%lu]", chunk, sz, ctx->in[ in_idx ].chunk0, ctx->in[ in_idx ].wmark ));

    if( FD_UNLIKELY( ctx->leader_slot!=ULONG_MAX ) ) {
      FD_LOG_WARNING(( "switching to slot %lu while packing for slot %lu. Draining bank tiles.", fd_disco_poh_sig_slot( sig ), ctx->leader_slot ));
      ctx->drain_banks         = 1;
      ctx->leader_slot         = ULONG_MAX;
      ctx->slot_microblock_cnt = 0UL;
      fd_pack_end_block( ctx->pack );
    }
    ctx->leader_slot = fd_disco_poh_sig_slot( sig );

    fd_became_leader_t * became_leader = (fd_became_leader_t *)dcache_entry;
    ctx->leader_bank          = became_leader->bank;
    ctx->slot_max_microblocks = became_leader->max_microblocks_in_slot;
    /* Reserve some space in the block for ticks */
    ctx->slot_max_data        = (ctx->larger_shred_limits_per_block ? LARGER_MAX_DATA_PER_BLOCK : FD_PACK_MAX_DATA_PER_BLOCK)
                                      - 48UL*(became_leader->ticks_per_slot+became_leader->total_skipped_ticks);


    /* The dcache might get overrun, so set slot_end_ns to 0, so if it does
       the slot will get skipped.  Then update it in the `after_frag` case
       below to the correct value. */
    ctx->slot_end_ns = 0L;
    ctx->_slot_end_ns = became_leader->slot_end_ns;

    update_metric_state( ctx, fd_tickcount(), FD_PACK_METRIC_STATE_LEADER, 1 );
    return;
  }
  case IN_KIND_BUNDLE: {
    FD_LOG_WARNING(( "Pack tile received a bundle... dropping..." ));
    return;
  }
  case IN_KIND_BANK: {
    FD_TEST( ctx->use_consumed_cus );
      /* For a previous slot */
    if( FD_UNLIKELY( fd_disco_poh_sig_slot( sig )!=ctx->leader_slot ) ) return;

    if( FD_UNLIKELY( chunk<ctx->in[ in_idx ].chunk0 || chunk>ctx->in[ in_idx ].wmark || sz<sizeof(fd_microblock_trailer_t)
          || sz>sizeof(fd_microblock_trailer_t)+sizeof(fd_txn_p_t)*MAX_TXN_PER_MICROBLOCK ) )
      FD_LOG_ERR(( "chunk %lu %lu corrupt, not in range [%lu,%lu]", chunk, sz, ctx->in[ in_idx ].chunk0, ctx->in[ in_idx ].wmark ));

    ctx->pending_rebate_cnt = (sz-sizeof(fd_microblock_trailer_t))/sizeof(fd_txn_p_t);
    fd_memcpy( ctx->pending_rebate, dcache_entry, sz-sizeof(fd_microblock_trailer_t) );
    return;
  }
  case IN_KIND_DEDUP: {
    if( FD_UNLIKELY( chunk<ctx->in[ in_idx ].chunk0 || chunk>ctx->in[ in_idx ].wmark || sz>FD_TPU_DCACHE_MTU ) )
      FD_LOG_ERR(( "chunk %lu %lu corrupt, not in range [%lu,%lu]", chunk, sz, ctx->in[ in_idx ].chunk0, ctx->in[ in_idx ].wmark ));

    long now = fd_tickcount();
    ulong exp_cnt = fd_pack_expire_before( ctx->pack, fd_ulong_max( (ulong)now+TIME_OFFSET, ctx->transaction_lifetime_ticks )-ctx->transaction_lifetime_ticks );
    FD_MCNT_INC( PACK, TRANSACTION_EXPIRED, exp_cnt );

    if( FD_LIKELY( ctx->leader_slot!=ULONG_MAX || fd_pack_avail_txn_cnt( ctx->pack )<ctx->max_pending_transactions ) ) {
      ctx->cur_spot = fd_pack_insert_txn_init( ctx->pack );
      ctx->insert_to_extra = 0;
    } else {
      if( FD_UNLIKELY( extra_txn_deq_full( ctx->extra_txn_deq ) ) ) {
        extra_txn_deq_remove_head( ctx->extra_txn_deq );
        FD_MCNT_INC( PACK, TRANSACTION_DROPPED_FROM_EXTRA, 1UL );
      }
      ctx->cur_spot = extra_txn_deq_peek_tail( extra_txn_deq_insert_tail( ctx->extra_txn_deq ) );
      /* We want to store the current time in cur_spot so that we can
         track its expiration better.  We just stash it in the CU
         fields, since those aren't important right now. */
      ctx->cur_spot->requested_cus = (uint)((ulong)now & UINT_MAX);
      ctx->cur_spot->executed_cus  = (uint)((ulong)now>>32       );
      ctx->insert_to_extra = 1;
      FD_MCNT_INC( PACK, TRANSACTION_INSERTED_TO_EXTRA, 1UL );
    }

    ulong payload_sz;
    /* We get transactions from the dedup tile.
      The transactions should have been parsed and verified. */
    FD_MCNT_INC( PACK, NORMAL_TRANSACTION_RECEIVED, 1UL );
    /* Assume that the dcache entry is:
          Payload ....... (payload_sz bytes)
          0 or 1 byte of padding (since alignof(fd_txn) is 2)
          fd_txn ....... (size computed by fd_txn_footprint)
          payload_sz  (2B)
      mline->sz includes all three fields and the padding */
    payload_sz = *(ushort*)(dcache_entry + sz - sizeof(ushort));
    uchar    const * payload = dcache_entry;
    fd_txn_t const * txn     = (fd_txn_t const *)( dcache_entry + fd_ulong_align_up( payload_sz, 2UL ) );
    fd_memcpy( ctx->cur_spot->payload, payload, payload_sz                                                     );
    fd_memcpy( TXN(ctx->cur_spot),     txn,     fd_txn_footprint( txn->instr_cnt, txn->addr_table_lookup_cnt ) );
    ctx->cur_spot->payload_sz = payload_sz;

  #if DETAILED_LOGGING
    FD_LOG_NOTICE(( "Pack got a packet. Payload size: %lu, txn footprint: %lu", payload_sz,
          fd_txn_footprint( txn->instr_cnt, txn->addr_table_lookup_cnt )
        ));
  #endif
    break;
  }
  }
}

/* After the transaction has been fully received, and we know we were
   not overrun while reading it, insert it into pack. */

static inline void
after_frag( fd_pack_ctx_t *     ctx,
            ulong               in_idx,
            ulong               seq,
            ulong               sig,
            ulong               chunk,
            ulong               sz,
            ulong               tsorig,
            fd_stem_context_t * stem ) {
  (void)seq;
  (void)chunk;
  (void)sz;
  (void)tsorig;
  (void)stem;

  long now = fd_tickcount();

  switch( ctx->in_kind[ in_idx ] ) {
  case IN_KIND_POH: {
    if( fd_disco_poh_sig_pkt_type( sig )!=POH_PKT_TYPE_BECAME_LEADER ) return;

    ctx->slot_end_ns = ctx->_slot_end_ns;
    fd_pack_set_block_limits( ctx->pack, ctx->slot_max_microblocks, ctx->slot_max_data );
    break;
  }
  case IN_KIND_BUNDLE: {
    FD_LOG_WARNING(( "Pack tile received a bundle... dropping..." ));
    break;
  }
  case IN_KIND_BANK: {
    /* For a previous slot */
    if( FD_UNLIKELY( fd_disco_poh_sig_slot( sig )!=ctx->leader_slot ) ) return;

    fd_pack_rebate_cus( ctx->pack, ctx->pending_rebate, ctx->pending_rebate_cnt );
    ctx->pending_rebate_cnt = 0UL;
    break;
  }
  case IN_KIND_DEDUP: {
    /* Normal transaction case */
    if( FD_LIKELY( !ctx->insert_to_extra ) ) {
      long insert_duration = -fd_tickcount();
      int result = fd_pack_insert_txn_fini( ctx->pack, ctx->cur_spot, (ulong)now+TIME_OFFSET );
      insert_duration      += fd_tickcount();
      ctx->insert_result[ result + FD_PACK_INSERT_RETVAL_OFF ]++;
      fd_histf_sample( ctx->insert_duration, (ulong)insert_duration );
      if( FD_LIKELY( result>=0 ) ) ctx->last_successful_insert = now;
    }

    ctx->cur_spot = NULL;
    break;
  }
  }

  update_metric_state( ctx, now, FD_PACK_METRIC_STATE_TRANSACTIONS, fd_pack_avail_txn_cnt( ctx->pack )>0 );
}

static void
unprivileged_init( fd_topo_t *      topo,
                   fd_topo_tile_t * tile ) {
  void * scratch = fd_topo_obj_laddr( topo, tile->tile_obj_id );

  fd_pack_limits_t limits[1] = {{
    .max_cost_per_block        = tile->pack.larger_max_cost_per_block ? LARGER_MAX_COST_PER_BLOCK : FD_PACK_MAX_COST_PER_BLOCK,
    .max_vote_cost_per_block   = FD_PACK_MAX_VOTE_COST_PER_BLOCK,
    .max_write_cost_per_acct   = FD_PACK_MAX_WRITE_COST_PER_ACCT,
    .max_data_bytes_per_block  = tile->pack.larger_shred_limits_per_block ? LARGER_MAX_DATA_PER_BLOCK : FD_PACK_MAX_DATA_PER_BLOCK,
    .max_txn_per_microblock    = MAX_TXN_PER_MICROBLOCK,
    .max_microblocks_per_block = (ulong)UINT_MAX, /* Limit not known yet */
  }};

  ulong pack_footprint = fd_pack_footprint( tile->pack.max_pending_transactions, tile->pack.bank_tile_count, limits );

  FD_SCRATCH_ALLOC_INIT( l, scratch );
  fd_pack_ctx_t * ctx = FD_SCRATCH_ALLOC_APPEND( l, alignof( fd_pack_ctx_t ), sizeof( fd_pack_ctx_t ) );
  fd_rng_t *      rng = fd_rng_join( fd_rng_new( FD_SCRATCH_ALLOC_APPEND( l, fd_rng_align(), fd_rng_footprint() ), 0U, 0UL ) );
  if( FD_UNLIKELY( !rng ) ) FD_LOG_ERR(( "fd_rng_new failed" ));

  ctx->pack = fd_pack_join( fd_pack_new( FD_SCRATCH_ALLOC_APPEND( l, fd_pack_align(), pack_footprint ),
                                         tile->pack.max_pending_transactions, tile->pack.bank_tile_count,
                                         limits, rng ) );
  if( FD_UNLIKELY( !ctx->pack ) ) FD_LOG_ERR(( "fd_pack_new failed" ));

  if( FD_UNLIKELY( tile->in_cnt>32UL ) ) FD_LOG_ERR(( "Too many input links (%lu>32) to pack tile", tile->in_cnt ));

  for( ulong i=0UL; i<tile->in_cnt; i++ ) {
    fd_topo_link_t const * link = &topo->links[ tile->in_link_id[ i ] ];

    if( FD_LIKELY(      !strcmp( link->name, "dedup_pack"  ) ) ) ctx->in_kind[ i ] = IN_KIND_DEDUP;
    else if( FD_LIKELY( !strcmp( link->name, "poh_pack"    ) ) ) ctx->in_kind[ i ] = IN_KIND_POH;
    else if( FD_LIKELY( !strcmp( link->name, "bank_poh"    ) ) ) ctx->in_kind[ i ] = IN_KIND_BANK;
    else if( FD_LIKELY( !strcmp( link->name, "bundle_pack" ) ) ) ctx->in_kind[ i ] = IN_KIND_BUNDLE;
    else FD_LOG_ERR(( "pack tile has unexpected input link %lu %s", i, link->name ));
  }

  ulong out_cnt = fd_topo_link_consumer_cnt( topo, &topo->links[ tile->out_link_id[ 0 ] ] );

  if( FD_UNLIKELY( !out_cnt                                ) ) FD_LOG_ERR(( "pack tile connects to no banking tiles" ));
  if( FD_UNLIKELY( out_cnt>FD_PACK_PACK_MAX_OUT            ) ) FD_LOG_ERR(( "pack tile connects to too many banking tiles" ));
  if( FD_UNLIKELY( out_cnt!=tile->pack.bank_tile_count+1UL ) ) FD_LOG_ERR(( "pack tile connects to %lu banking tiles, but tile->pack.bank_tile_count is %lu", out_cnt-1UL, tile->pack.bank_tile_count ));

  ctx->extra_txn_deq = extra_txn_deq_join( extra_txn_deq_new( FD_SCRATCH_ALLOC_APPEND( l, extra_txn_deq_align(),
                                                                                          extra_txn_deq_footprint() ) ) );

  ctx->cur_spot                      = NULL;
  ctx->max_pending_transactions      = tile->pack.max_pending_transactions;
  ctx->leader_slot                   = ULONG_MAX;
  ctx->leader_bank                   = NULL;
  ctx->slot_microblock_cnt           = 0UL;
  ctx->slot_max_microblocks          = 0UL;
  ctx->slot_max_data                 = 0UL;
  ctx->larger_shred_limits_per_block = tile->pack.larger_shred_limits_per_block;
  ctx->rng                           = rng;
  ctx->last_successful_insert        = 0L;
  ctx->transaction_lifetime_ticks    = (ulong)(fd_tempo_tick_per_ns( NULL )*(double)TRANSACTION_LIFETIME_NS + 0.5);
  ctx->microblock_duration_ticks     = (ulong)(fd_tempo_tick_per_ns( NULL )*(double)MICROBLOCK_DURATION_NS  + 0.5);
  ctx->insert_to_extra               = 0;
  ctx->use_consumed_cus              = tile->pack.use_consumed_cus;

  ctx->wait_duration_ticks[ 0 ] = ULONG_MAX;
  for( ulong i=1UL; i<MAX_TXN_PER_MICROBLOCK+1UL; i++ ) {
    ctx->wait_duration_ticks[ i ]=(ulong)(fd_tempo_tick_per_ns( NULL )*(double)wait_duration[ i ] + 0.5);
  }


  ctx->bank_cnt         = tile->pack.bank_tile_count;
  ctx->poll_cursor      = 0;
  ctx->skip_cnt         = 0L;
  ctx->bank_idle_bitset = fd_ulong_mask_lsb( (int)tile->pack.bank_tile_count );
  for( ulong i=0UL; i<tile->pack.bank_tile_count; i++ ) {
    ulong busy_obj_id = fd_pod_queryf_ulong( topo->props, ULONG_MAX, "bank_busy.%lu", i );
    FD_TEST( busy_obj_id!=ULONG_MAX );
    ctx->bank_current[ i ] = fd_fseq_join( fd_topo_obj_laddr( topo, busy_obj_id ) );
    ctx->bank_expect[ i ] = ULONG_MAX;
    if( FD_UNLIKELY( !ctx->bank_current[ i ] ) ) FD_LOG_ERR(( "banking tile %lu has no busy flag", i ));
    ctx->bank_ready_at[ i ] = 0L;
    FD_TEST( ULONG_MAX==fd_fseq_query( ctx->bank_current[ i ] ) );
  }

  for( ulong i=0UL; i<tile->in_cnt; i++ ) {
    fd_topo_link_t * link = &topo->links[ tile->in_link_id[ i ] ];
    fd_topo_wksp_t * link_wksp = &topo->workspaces[ topo->objs[ link->dcache_obj_id ].wksp_id ];

    ctx->in[ i ].mem    = link_wksp->wksp;
    ctx->in[ i ].chunk0 = fd_dcache_compact_chunk0( ctx->in[ i ].mem, link->dcache );
    ctx->in[ i ].wmark  = fd_dcache_compact_wmark ( ctx->in[ i ].mem, link->dcache, link->mtu );
  }

  ctx->out_mem    = topo->workspaces[ topo->objs[ topo->links[ tile->out_link_id[ 0 ] ].dcache_obj_id ].wksp_id ].wksp;
  ctx->out_chunk0 = fd_dcache_compact_chunk0( ctx->out_mem, topo->links[ tile->out_link_id[ 0 ] ].dcache );
  ctx->out_wmark  = fd_dcache_compact_wmark ( ctx->out_mem, topo->links[ tile->out_link_id[ 0 ] ].dcache, topo->links[ tile->out_link_id[ 0 ] ].mtu );
  ctx->out_chunk  = ctx->out_chunk0;

  /* Initialize metrics storage */
  memset( ctx->insert_result, '\0', FD_PACK_INSERT_RETVAL_CNT * sizeof(ulong) );
  fd_histf_join( fd_histf_new( ctx->schedule_duration, FD_MHIST_SECONDS_MIN( PACK, SCHEDULE_MICROBLOCK_DURATION_SECONDS ),
                                                       FD_MHIST_SECONDS_MAX( PACK, SCHEDULE_MICROBLOCK_DURATION_SECONDS ) ) );
  fd_histf_join( fd_histf_new( ctx->insert_duration,   FD_MHIST_SECONDS_MIN( PACK, INSERT_TRANSACTION_DURATION_SECONDS  ),
                                                       FD_MHIST_SECONDS_MAX( PACK, INSERT_TRANSACTION_DURATION_SECONDS  ) ) );
  ctx->metric_state = 0;
  ctx->metric_state_begin = fd_tickcount();
  memset( ctx->metric_timing, '\0', 16*sizeof(long) );

  FD_LOG_INFO(( "packing microblocks of at most %lu transactions to %lu bank tiles", MAX_TXN_PER_MICROBLOCK, tile->pack.bank_tile_count ));

  ulong scratch_top = FD_SCRATCH_ALLOC_FINI( l, 1UL );
  if( FD_UNLIKELY( scratch_top > (ulong)scratch + scratch_footprint( tile ) ) )
    FD_LOG_ERR(( "scratch overflow %lu %lu %lu", scratch_top - (ulong)scratch - scratch_footprint( tile ), scratch_top, (ulong)scratch + scratch_footprint( tile ) ));

}

static ulong
populate_allowed_seccomp( fd_topo_t const *      topo,
                          fd_topo_tile_t const * tile,
                          ulong                  out_cnt,
                          struct sock_filter *   out ) {
  (void)topo;
  (void)tile;

  populate_sock_filter_policy_pack( out_cnt, out, (uint)fd_log_private_logfile_fd() );
  return sock_filter_policy_pack_instr_cnt;
}

static ulong
populate_allowed_fds( fd_topo_t const *      topo,
                      fd_topo_tile_t const * tile,
                      ulong                  out_fds_cnt,
                      int *                  out_fds ) {
  (void)topo;
  (void)tile;

  if( FD_UNLIKELY( out_fds_cnt<2UL ) ) FD_LOG_ERR(( "out_fds_cnt %lu", out_fds_cnt ));

  ulong out_cnt = 0UL;
  out_fds[ out_cnt++ ] = 2; /* stderr */
  if( FD_LIKELY( -1!=fd_log_private_logfile_fd() ) )
    out_fds[ out_cnt++ ] = fd_log_private_logfile_fd(); /* logfile */
  return out_cnt;
}

#define STEM_BURST (1UL)

/* We want lazy (measured in ns) to be small enough that the producer
    and the consumer never have to wait for credits.  For most tango
    links, we use a default worst case speed coming from 100 Gbps
    Ethernet.  That's not very suitable for microblocks that go from
    pack to bank.  Instead we manually estimate the very aggressive
    1000ns per microblock, and then reduce it further (in line with the
    default lazy value computation) to ensure the random value chosen
    based on this won't lead to credit return stalls. */
#define STEM_LAZY  (128L*3000L)

#define STEM_CALLBACK_CONTEXT_TYPE  fd_pack_ctx_t
#define STEM_CALLBACK_CONTEXT_ALIGN alignof(fd_pack_ctx_t)

#define STEM_CALLBACK_DURING_HOUSEKEEPING during_housekeeping
#define STEM_CALLBACK_BEFORE_CREDIT       before_credit
#define STEM_CALLBACK_AFTER_CREDIT        after_credit
#define STEM_CALLBACK_DURING_FRAG         during_frag
#define STEM_CALLBACK_AFTER_FRAG          after_frag
#define STEM_CALLBACK_METRICS_WRITE       metrics_write

#include "../../../../disco/stem/fd_stem.c"

fd_topo_run_tile_t fd_tile_pack = {
  .name                     = "pack",
  .populate_allowed_seccomp = populate_allowed_seccomp,
  .populate_allowed_fds     = populate_allowed_fds,
  .scratch_align            = scratch_align,
  .scratch_footprint        = scratch_footprint,
  .unprivileged_init        = unprivileged_init,
  .run                      = stem_run,
};
