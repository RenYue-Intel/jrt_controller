// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#include <stdint.h>  // For UINT64_MAX

#define MAX_SRS_CE_BLOCKS 256u
#define MAX_SRS_CE_BYTES  65535u

struct jbpf_srs_ce_block {
    uint32_t offset;
    uint32_t size;
};

typedef struct {
    uint16_t cell_id;
    uint16_t slot;
    uint16_t start_ant;
    uint16_t ant_count;
    uint16_t ue_count;
    uint16_t port_count;
    uint64_t timestamp_ns;
    uint32_t block_count;
    uint32_t payload_len;
    struct jbpf_srs_ce_block blocks[MAX_SRS_CE_BLOCKS];
    uint8_t  payload[MAX_SRS_CE_BYTES];
} example_3_msg;

struct jbpf_srs_ce_ctx {
    uint16_t cell_id;
    uint16_t slot;
    uint16_t start_ant;
    uint16_t ant_count;
    uint16_t ue_count;
    uint16_t port_count;
    uint64_t timestamp_ns;
    uint32_t block_count;
    uint32_t payload_len;
    struct jbpf_srs_ce_block blocks[MAX_SRS_CE_BLOCKS];
    uint8_t  payload[MAX_SRS_CE_BYTES];
};