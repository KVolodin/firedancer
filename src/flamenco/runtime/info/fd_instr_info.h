#ifndef HEADER_fd_src_flamenco_runtime_info_fd_instr_info_h
#define HEADER_fd_src_flamenco_runtime_info_fd_instr_info_h

#include "../../fd_flamenco_base.h"
#include "../../types/fd_types.h"
#include "../fd_txn_account.h"
// TODO: rename to _MASK
#define FD_INSTR_ACCT_FLAGS_IS_SIGNER   (0x01U)
#define FD_INSTR_ACCT_FLAGS_IS_WRITABLE (0x02U)

/* While the maximum number of instruction accounts allowed for instruction
   execution is 256, it is entirely possible to have a transaction with more
   than 256 instruction accounts that passes transaction loading checks and enters
   `fd_execute_instr` (See mainnet transaction
   3eDdfZE6HswPxFKrtnQPsEmTkyL1iP57gRPEXwaqNGAqF1paGXCYYMwh7z4uQDUMgFor742sikVSQZW1gFRDhPNh
   for an example). An instruction that goes into the VM with more than 256 instruction accounts
   will fail, but you could also theoretically invoke a native program with over 256 random
   unreferenced instruction accounts that will execute successfully. The true bound for the
   maximum number of instruction accounts you can pass in is slighly lower than the maximum
   possible size for a serialized transaction (1232).

   HOWEVER... to keep our memory footprint low, we cap the `acct_cnt` at 256 during setup since
   any extra accounts should (ideally) have literally 0 impact on program execution, whether
   or not they are present in the instr info. This keeps the transaction context size from
   blowing up to around 3MB in size. */
#define FD_INSTR_ACCT_MAX (256)

struct fd_instr_info {
  uchar               program_id;
  ushort              data_sz;
  ushort              acct_cnt;

  uchar *             data;
  fd_pubkey_t         program_id_pubkey;

  uchar               acct_txn_idxs[FD_INSTR_ACCT_MAX];
  uchar               acct_flags[FD_INSTR_ACCT_MAX];
  fd_pubkey_t         acct_pubkeys[FD_INSTR_ACCT_MAX];
  uchar               is_duplicate[FD_INSTR_ACCT_MAX];

  /* Indexed by index in instruction, not by index in transaction. */
  fd_txn_account_t *  accounts[FD_INSTR_ACCT_MAX];

  /* fd_uwide representation of uint_128 */
  ulong               starting_lamports_h;
  ulong               starting_lamports_l;
};

typedef struct fd_instr_info fd_instr_info_t;

FD_PROTOTYPES_BEGIN

void
fd_convert_txn_instr_to_instr( fd_exec_txn_ctx_t *     txn_ctx,
                               fd_txn_instr_t const *  txn_instr,
                               fd_txn_account_t *      accts,
                               fd_instr_info_t *       instr );

FD_FN_PURE static inline int
fd_instr_acc_is_writable_idx( fd_instr_info_t const * instr,
                              ulong                   idx ) {
  return !!(instr->acct_flags[idx] & FD_INSTR_ACCT_FLAGS_IS_WRITABLE);
}

static inline int
fd_instr_acc_is_writable(fd_instr_info_t const * instr, fd_pubkey_t const * acc) {
  for( uchar i = 0; i < instr->acct_cnt; i++ ) {
    if( memcmp( &instr->acct_pubkeys[i], acc, sizeof( fd_pubkey_t ) )==0 ) {
      return fd_instr_acc_is_writable_idx( instr, i );
    }
  }

  return 0;
}

FD_FN_PURE static inline int
fd_instr_acc_is_signer_idx( fd_instr_info_t const * instr,
                            ulong                   idx ) {
  return !!(instr->acct_flags[idx] & FD_INSTR_ACCT_FLAGS_IS_SIGNER);
}

static inline int
fd_instr_acc_is_signer(fd_instr_info_t const * instr, fd_pubkey_t const * acc) {
  for( uchar i = 0; i < instr->acct_cnt; i++ ) {
    if( memcmp( &instr->acct_pubkeys[i], acc, sizeof( fd_pubkey_t ) )==0 ) {
      return fd_instr_acc_is_signer_idx( instr, i );
    }
  }

  return 0;
}

/* https://github.com/solana-labs/solana/blob/v1.17.23/programs/system/src/system_processor.rs#L35-L41

   fd_instr_any_signed matches
   solana_system_program::system_processor::Address::is_signer
   Scans instruction accounts for matching signer.

   Returns 1 if *any* instruction account with the given pubkey is a
   signer and 0 otherwise.  Note that the same account/pubkey can be
   specified as multiple different instruction accounts that might not
   all have the signer bit. */

FD_FN_PURE int
fd_instr_any_signed( fd_instr_info_t const * info,
                     fd_pubkey_t const *     pubkey );

/* fd_instr_info_sum_account_lamports returns the sum of lamport account
   balances of all instruction accounts in the context.

   Aborts on integer overflow. */

int
fd_instr_info_sum_account_lamports( fd_instr_info_t const * instr,
                                    ulong *                 total_lamports_h,
                                    ulong *                 total_lamports_l );

static inline void
fd_instr_get_signers( fd_instr_info_t const * self,
                      fd_pubkey_t const *     signers[static FD_TXN_SIG_MAX] ) {
  ulong j = 0UL;
  for( uchar i = 0; i < self->acct_cnt && j < FD_TXN_SIG_MAX; i++ )
    if( fd_instr_acc_is_signer_idx( self, i ) )
      signers[j++] = &self->acct_pubkeys[i];
}

/* Loop conditions could be optimized to allow for unroll/vectorize */

static inline int
fd_instr_signers_contains( fd_pubkey_t const * signers[FD_TXN_SIG_MAX],
                           fd_pubkey_t const * pubkey ) {
  for( ulong i = 0; i < FD_TXN_SIG_MAX && signers[i]; i++ )
    if( 0==memcmp( signers[i], pubkey, sizeof( fd_pubkey_t ) ) ) return 1;
  return 0;
}

FD_PROTOTYPES_END

#endif /* HEADER_fd_src_flamenco_runtime_info_fd_instr_info_h */
