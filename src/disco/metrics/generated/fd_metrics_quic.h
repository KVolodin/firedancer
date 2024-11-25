/* THIS FILE IS GENERATED BY gen_metrics.py. DO NOT HAND EDIT. */

#include "../fd_metrics_base.h"

#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_OVERRUN_OFF  (16UL)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_OVERRUN_NAME "quic_tile_txns_overrun"
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_OVERRUN_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_OVERRUN_DESC "Count of txns overrun before reassembled (too small txn_reassembly_count)."
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_OVERRUN_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_TXN_REASMS_STARTED_OFF  (17UL)
#define FD_METRICS_COUNTER_QUIC_TILE_TXN_REASMS_STARTED_NAME "quic_tile_txn_reasms_started"
#define FD_METRICS_COUNTER_QUIC_TILE_TXN_REASMS_STARTED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_TXN_REASMS_STARTED_DESC "Count of fragmented txn receive ops started."
#define FD_METRICS_COUNTER_QUIC_TILE_TXN_REASMS_STARTED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_GAUGE_QUIC_TILE_TXN_REASMS_ACTIVE_OFF  (18UL)
#define FD_METRICS_GAUGE_QUIC_TILE_TXN_REASMS_ACTIVE_NAME "quic_tile_txn_reasms_active"
#define FD_METRICS_GAUGE_QUIC_TILE_TXN_REASMS_ACTIVE_TYPE (FD_METRICS_TYPE_GAUGE)
#define FD_METRICS_GAUGE_QUIC_TILE_TXN_REASMS_ACTIVE_DESC "Number of fragmented txn receive ops currently active."
#define FD_METRICS_GAUGE_QUIC_TILE_TXN_REASMS_ACTIVE_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OK_OFF  (19UL)
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OK_NAME "quic_tile_frags_ok"
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OK_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OK_DESC "Count of txn frags received"
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OK_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_GAP_OFF  (20UL)
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_GAP_NAME "quic_tile_frags_gap"
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_GAP_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_GAP_DESC "Count of txn frags dropped due to data gap"
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_GAP_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_DUP_OFF  (21UL)
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_DUP_NAME "quic_tile_frags_dup"
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_DUP_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_DUP_DESC "Count of txn frags dropped due to dup (stream already completed)"
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_DUP_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OVERSZ_OFF  (22UL)
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OVERSZ_NAME "quic_tile_frags_oversz"
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OVERSZ_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OVERSZ_DESC "Count of txn frags dropped because they would exceed txn MTU"
#define FD_METRICS_COUNTER_QUIC_TILE_FRAGS_OVERSZ_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_OFF  (23UL)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_CNT  (3UL)

#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_UDP_OFF  (23UL)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_UDP_NAME "quic_tile_txns_received_udp"
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_UDP_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_UDP_DESC "Count of txns received via TPU. (TPU/UDP)"
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_UDP_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FAST_OFF  (24UL)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FAST_NAME "quic_tile_txns_received_quic_fast"
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FAST_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FAST_DESC "Count of txns received via TPU. (TPU/QUIC unfragmented)"
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FAST_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FRAG_OFF  (25UL)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FRAG_NAME "quic_tile_txns_received_quic_frag"
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FRAG_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FRAG_DESC "Count of txns received via TPU. (TPU/QUIC fragmented)"
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_RECEIVED_QUIC_FRAG_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_ABANDONED_OFF  (26UL)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_ABANDONED_NAME "quic_tile_txns_abandoned"
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_ABANDONED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_ABANDONED_DESC "Count of txns abandoned because a conn was lost."
#define FD_METRICS_COUNTER_QUIC_TILE_TXNS_ABANDONED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_QUIC_PACKET_TOO_SMALL_OFF  (27UL)
#define FD_METRICS_COUNTER_QUIC_TILE_QUIC_PACKET_TOO_SMALL_NAME "quic_tile_quic_packet_too_small"
#define FD_METRICS_COUNTER_QUIC_TILE_QUIC_PACKET_TOO_SMALL_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_QUIC_PACKET_TOO_SMALL_DESC "Count of packets received on the QUIC port that were too small to be a valid IP packet."
#define FD_METRICS_COUNTER_QUIC_TILE_QUIC_PACKET_TOO_SMALL_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_SMALL_OFF  (28UL)
#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_SMALL_NAME "quic_tile_non_quic_packet_too_small"
#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_SMALL_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_SMALL_DESC "Count of packets received on the non-QUIC port that were too small to be a valid IP packet."
#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_SMALL_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_LARGE_OFF  (29UL)
#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_LARGE_NAME "quic_tile_non_quic_packet_too_large"
#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_LARGE_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_LARGE_DESC "Count of packets received on the non-QUIC port that were too large to be a valid transaction."
#define FD_METRICS_COUNTER_QUIC_TILE_NON_QUIC_PACKET_TOO_LARGE_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_PACKETS_OFF  (30UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_PACKETS_NAME "quic_received_packets"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_PACKETS_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_PACKETS_DESC "Number of IP packets received."
#define FD_METRICS_COUNTER_QUIC_RECEIVED_PACKETS_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_BYTES_OFF  (31UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_BYTES_NAME "quic_received_bytes"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_BYTES_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_BYTES_DESC "Total bytes received (including IP, UDP, QUIC headers)."
#define FD_METRICS_COUNTER_QUIC_RECEIVED_BYTES_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_SENT_PACKETS_OFF  (32UL)
#define FD_METRICS_COUNTER_QUIC_SENT_PACKETS_NAME "quic_sent_packets"
#define FD_METRICS_COUNTER_QUIC_SENT_PACKETS_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_SENT_PACKETS_DESC "Number of IP packets sent."
#define FD_METRICS_COUNTER_QUIC_SENT_PACKETS_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_SENT_BYTES_OFF  (33UL)
#define FD_METRICS_COUNTER_QUIC_SENT_BYTES_NAME "quic_sent_bytes"
#define FD_METRICS_COUNTER_QUIC_SENT_BYTES_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_SENT_BYTES_DESC "Total bytes sent (including IP, UDP, QUIC headers)."
#define FD_METRICS_COUNTER_QUIC_SENT_BYTES_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_GAUGE_QUIC_CONNECTIONS_ACTIVE_OFF  (34UL)
#define FD_METRICS_GAUGE_QUIC_CONNECTIONS_ACTIVE_NAME "quic_connections_active"
#define FD_METRICS_GAUGE_QUIC_CONNECTIONS_ACTIVE_TYPE (FD_METRICS_TYPE_GAUGE)
#define FD_METRICS_GAUGE_QUIC_CONNECTIONS_ACTIVE_DESC "The number of currently active QUIC connections."
#define FD_METRICS_GAUGE_QUIC_CONNECTIONS_ACTIVE_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CREATED_OFF  (35UL)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CREATED_NAME "quic_connections_created"
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CREATED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CREATED_DESC "The total number of connections that have been created."
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CREATED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CLOSED_OFF  (36UL)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CLOSED_NAME "quic_connections_closed"
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CLOSED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CLOSED_DESC "Number of connections gracefully closed."
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_CLOSED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_ABORTED_OFF  (37UL)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_ABORTED_NAME "quic_connections_aborted"
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_ABORTED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_ABORTED_DESC "Number of connections aborted."
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_ABORTED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_TIMED_OUT_OFF  (38UL)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_TIMED_OUT_NAME "quic_connections_timed_out"
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_TIMED_OUT_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_TIMED_OUT_DESC "Number of connections timed out."
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_TIMED_OUT_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_RETRIED_OFF  (39UL)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_RETRIED_NAME "quic_connections_retried"
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_RETRIED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_RETRIED_DESC "Number of connections established with retry."
#define FD_METRICS_COUNTER_QUIC_CONNECTIONS_RETRIED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_NO_SLOTS_OFF  (40UL)
#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_NO_SLOTS_NAME "quic_connection_error_no_slots"
#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_NO_SLOTS_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_NO_SLOTS_DESC "Number of connections that failed to create due to lack of slots."
#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_NO_SLOTS_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_RETRY_FAIL_OFF  (41UL)
#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_RETRY_FAIL_NAME "quic_connection_error_retry_fail"
#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_RETRY_FAIL_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_RETRY_FAIL_DESC "Number of connections that failed during retry (e.g. invalid token)."
#define FD_METRICS_COUNTER_QUIC_CONNECTION_ERROR_RETRY_FAIL_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_PKT_CRYPTO_FAILED_OFF  (42UL)
#define FD_METRICS_COUNTER_QUIC_PKT_CRYPTO_FAILED_NAME "quic_pkt_crypto_failed"
#define FD_METRICS_COUNTER_QUIC_PKT_CRYPTO_FAILED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_PKT_CRYPTO_FAILED_DESC "Number of packets that failed decryption."
#define FD_METRICS_COUNTER_QUIC_PKT_CRYPTO_FAILED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_PKT_NO_CONN_OFF  (43UL)
#define FD_METRICS_COUNTER_QUIC_PKT_NO_CONN_NAME "quic_pkt_no_conn"
#define FD_METRICS_COUNTER_QUIC_PKT_NO_CONN_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_PKT_NO_CONN_DESC "Number of packets with an unknown connection ID."
#define FD_METRICS_COUNTER_QUIC_PKT_NO_CONN_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_PKT_TX_ALLOC_FAIL_OFF  (44UL)
#define FD_METRICS_COUNTER_QUIC_PKT_TX_ALLOC_FAIL_NAME "quic_pkt_tx_alloc_fail"
#define FD_METRICS_COUNTER_QUIC_PKT_TX_ALLOC_FAIL_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_PKT_TX_ALLOC_FAIL_DESC "Number of packets failed to send because of metadata alloc fail."
#define FD_METRICS_COUNTER_QUIC_PKT_TX_ALLOC_FAIL_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_HANDSHAKES_CREATED_OFF  (45UL)
#define FD_METRICS_COUNTER_QUIC_HANDSHAKES_CREATED_NAME "quic_handshakes_created"
#define FD_METRICS_COUNTER_QUIC_HANDSHAKES_CREATED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_HANDSHAKES_CREATED_DESC "Number of handshake flows created."
#define FD_METRICS_COUNTER_QUIC_HANDSHAKES_CREATED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_HANDSHAKE_ERROR_ALLOC_FAIL_OFF  (46UL)
#define FD_METRICS_COUNTER_QUIC_HANDSHAKE_ERROR_ALLOC_FAIL_NAME "quic_handshake_error_alloc_fail"
#define FD_METRICS_COUNTER_QUIC_HANDSHAKE_ERROR_ALLOC_FAIL_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_HANDSHAKE_ERROR_ALLOC_FAIL_DESC "Number of handshakes dropped due to alloc fail."
#define FD_METRICS_COUNTER_QUIC_HANDSHAKE_ERROR_ALLOC_FAIL_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_EVENTS_OFF  (47UL)
#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_EVENTS_NAME "quic_stream_received_events"
#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_EVENTS_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_EVENTS_DESC "Number of stream RX events."
#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_EVENTS_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_BYTES_OFF  (48UL)
#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_BYTES_NAME "quic_stream_received_bytes"
#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_BYTES_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_BYTES_DESC "Total stream payload bytes received."
#define FD_METRICS_COUNTER_QUIC_STREAM_RECEIVED_BYTES_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_OFF  (49UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CNT  (22UL)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_UNKNOWN_OFF  (49UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_UNKNOWN_NAME "quic_received_frames_unknown"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_UNKNOWN_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_UNKNOWN_DESC "Number of QUIC frames received. (Unknown frame type)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_UNKNOWN_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_ACK_OFF  (50UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_ACK_NAME "quic_received_frames_ack"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_ACK_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_ACK_DESC "Number of QUIC frames received. (ACK frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_ACK_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RESET_STREAM_OFF  (51UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RESET_STREAM_NAME "quic_received_frames_reset_stream"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RESET_STREAM_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RESET_STREAM_DESC "Number of QUIC frames received. (RESET_STREAM frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RESET_STREAM_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STOP_SENDING_OFF  (52UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STOP_SENDING_NAME "quic_received_frames_stop_sending"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STOP_SENDING_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STOP_SENDING_DESC "Number of QUIC frames received. (STOP_SENDING frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STOP_SENDING_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CRYPTO_OFF  (53UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CRYPTO_NAME "quic_received_frames_crypto"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CRYPTO_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CRYPTO_DESC "Number of QUIC frames received. (CRYPTO frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CRYPTO_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_TOKEN_OFF  (54UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_TOKEN_NAME "quic_received_frames_new_token"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_TOKEN_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_TOKEN_DESC "Number of QUIC frames received. (NEW_TOKEN frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_TOKEN_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_OFF  (55UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_NAME "quic_received_frames_stream"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_DESC "Number of QUIC frames received. (STREAM frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_DATA_OFF  (56UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_DATA_NAME "quic_received_frames_max_data"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_DATA_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_DATA_DESC "Number of QUIC frames received. (MAX_DATA frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_DATA_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAM_DATA_OFF  (57UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAM_DATA_NAME "quic_received_frames_max_stream_data"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAM_DATA_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAM_DATA_DESC "Number of QUIC frames received. (MAX_STREAM_DATA frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAM_DATA_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAMS_OFF  (58UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAMS_NAME "quic_received_frames_max_streams"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAMS_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAMS_DESC "Number of QUIC frames received. (MAX_STREAMS frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_MAX_STREAMS_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_DATA_BLOCKED_OFF  (59UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_DATA_BLOCKED_NAME "quic_received_frames_data_blocked"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_DATA_BLOCKED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_DATA_BLOCKED_DESC "Number of QUIC frames received. (DATA_BLOCKED frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_DATA_BLOCKED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_DATA_BLOCKED_OFF  (60UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_DATA_BLOCKED_NAME "quic_received_frames_stream_data_blocked"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_DATA_BLOCKED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_DATA_BLOCKED_DESC "Number of QUIC frames received. (STREAM_DATA_BLOCKED frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAM_DATA_BLOCKED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAMS_BLOCKED_OFF  (61UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAMS_BLOCKED_NAME "quic_received_frames_streams_blocked"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAMS_BLOCKED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAMS_BLOCKED_DESC "Number of QUIC frames received. (STREAMS_BLOCKED(bidi) frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_STREAMS_BLOCKED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_CONN_ID_OFF  (62UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_CONN_ID_NAME "quic_received_frames_new_conn_id"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_CONN_ID_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_CONN_ID_DESC "Number of QUIC frames received. (NEW_CONN_ID frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_NEW_CONN_ID_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RETIRE_CONN_ID_OFF  (63UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RETIRE_CONN_ID_NAME "quic_received_frames_retire_conn_id"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RETIRE_CONN_ID_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RETIRE_CONN_ID_DESC "Number of QUIC frames received. (RETIRE_CONN_ID frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_RETIRE_CONN_ID_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_CHALLENGE_OFF  (64UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_CHALLENGE_NAME "quic_received_frames_path_challenge"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_CHALLENGE_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_CHALLENGE_DESC "Number of QUIC frames received. (PATH_CHALLENGE frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_CHALLENGE_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_RESPONSE_OFF  (65UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_RESPONSE_NAME "quic_received_frames_path_response"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_RESPONSE_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_RESPONSE_DESC "Number of QUIC frames received. (PATH_RESPONSE frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PATH_RESPONSE_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_QUIC_OFF  (66UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_QUIC_NAME "quic_received_frames_conn_close_quic"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_QUIC_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_QUIC_DESC "Number of QUIC frames received. (CONN_CLOSE(transport) frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_QUIC_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_APP_OFF  (67UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_APP_NAME "quic_received_frames_conn_close_app"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_APP_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_APP_DESC "Number of QUIC frames received. (CONN_CLOSE(app) frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_CONN_CLOSE_APP_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_HANDSHAKE_DONE_OFF  (68UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_HANDSHAKE_DONE_NAME "quic_received_frames_handshake_done"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_HANDSHAKE_DONE_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_HANDSHAKE_DONE_DESC "Number of QUIC frames received. (HANDSHAKE_DONE frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_HANDSHAKE_DONE_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PING_OFF  (69UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PING_NAME "quic_received_frames_ping"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PING_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PING_DESC "Number of QUIC frames received. (PING frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PING_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PADDING_OFF  (70UL)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PADDING_NAME "quic_received_frames_padding"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PADDING_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PADDING_DESC "Number of QUIC frames received. (PADDING frame)"
#define FD_METRICS_COUNTER_QUIC_RECEIVED_FRAMES_PADDING_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_ACK_TX_OFF  (71UL)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_CNT  (5UL)

#define FD_METRICS_COUNTER_QUIC_ACK_TX_NOOP_OFF  (71UL)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_NOOP_NAME "quic_ack_tx_noop"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_NOOP_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_NOOP_DESC "ACK events (non-ACK-eliciting packet)"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_NOOP_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_ACK_TX_NEW_OFF  (72UL)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_NEW_NAME "quic_ack_tx_new"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_NEW_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_NEW_DESC "ACK events (new ACK range)"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_NEW_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_ACK_TX_MERGED_OFF  (73UL)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_MERGED_NAME "quic_ack_tx_merged"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_MERGED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_MERGED_DESC "ACK events (merged into existing ACK range)"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_MERGED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_ACK_TX_DROP_OFF  (74UL)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_DROP_NAME "quic_ack_tx_drop"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_DROP_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_DROP_DESC "ACK events (out of buffers)"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_DROP_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_QUIC_ACK_TX_CANCEL_OFF  (75UL)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_CANCEL_NAME "quic_ack_tx_cancel"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_CANCEL_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_ACK_TX_CANCEL_DESC "ACK events (ACK suppressed by handler)"
#define FD_METRICS_COUNTER_QUIC_ACK_TX_CANCEL_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_HISTOGRAM_QUIC_SERVICE_DURATION_SECONDS_OFF  (76UL)
#define FD_METRICS_HISTOGRAM_QUIC_SERVICE_DURATION_SECONDS_NAME "quic_service_duration_seconds"
#define FD_METRICS_HISTOGRAM_QUIC_SERVICE_DURATION_SECONDS_TYPE (FD_METRICS_TYPE_HISTOGRAM)
#define FD_METRICS_HISTOGRAM_QUIC_SERVICE_DURATION_SECONDS_DESC "Duration spent in service"
#define FD_METRICS_HISTOGRAM_QUIC_SERVICE_DURATION_SECONDS_CVT  (FD_METRICS_CONVERTER_SECONDS)
#define FD_METRICS_HISTOGRAM_QUIC_SERVICE_DURATION_SECONDS_MIN  (1e-08)
#define FD_METRICS_HISTOGRAM_QUIC_SERVICE_DURATION_SECONDS_MAX  (0.1)
#define FD_METRICS_HISTOGRAM_QUIC_RECEIVE_DURATION_SECONDS_OFF  (93UL)
#define FD_METRICS_HISTOGRAM_QUIC_RECEIVE_DURATION_SECONDS_NAME "quic_receive_duration_seconds"
#define FD_METRICS_HISTOGRAM_QUIC_RECEIVE_DURATION_SECONDS_TYPE (FD_METRICS_TYPE_HISTOGRAM)
#define FD_METRICS_HISTOGRAM_QUIC_RECEIVE_DURATION_SECONDS_DESC "Duration spent receiving packets"
#define FD_METRICS_HISTOGRAM_QUIC_RECEIVE_DURATION_SECONDS_CVT  (FD_METRICS_CONVERTER_SECONDS)
#define FD_METRICS_HISTOGRAM_QUIC_RECEIVE_DURATION_SECONDS_MIN  (1e-08)
#define FD_METRICS_HISTOGRAM_QUIC_RECEIVE_DURATION_SECONDS_MAX  (0.1)
#define FD_METRICS_COUNTER_QUIC_FRAME_FAIL_PARSE_OFF  (110UL)
#define FD_METRICS_COUNTER_QUIC_FRAME_FAIL_PARSE_NAME "quic_frame_fail_parse"
#define FD_METRICS_COUNTER_QUIC_FRAME_FAIL_PARSE_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_QUIC_FRAME_FAIL_PARSE_DESC "Number of QUIC frames failed to parse."
#define FD_METRICS_COUNTER_QUIC_FRAME_FAIL_PARSE_CVT  (FD_METRICS_CONVERTER_NONE)


#define FD_METRICS_QUIC_TOTAL (63UL)
extern const fd_metrics_meta_t FD_METRICS_QUIC[FD_METRICS_QUIC_TOTAL];
