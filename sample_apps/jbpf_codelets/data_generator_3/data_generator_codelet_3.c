// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <string.h>

#include "jbpf_defs.h"
#include "jbpf_helper.h"
#include "generated_data_3.h"

jbpf_ringbuf_map(srs_ce_ringbuf, example_3_msg, 4096);

struct jbpf_load_map_def SEC("maps") msg_cache = {
    .type       = JBPF_MAP_TYPE_ARRAY,
    .key_size   = sizeof(int),
    .value_size = sizeof(example_3_msg),
    .max_entries= 1,
};

SEC("jbpf_ran_fapi")
uint64_t jbpf_main(void *state)
{
    struct jbpf_generic_ctx *generic_ctx = (struct jbpf_generic_ctx *)state;

    // BPF VERIFIER FIX: Add bounds checking before accessing data
    if (generic_ctx->data + sizeof(struct jbpf_srs_ce_ctx) > generic_ctx->data_end) 
    {
        return 0;
    }
    struct jbpf_srs_ce_ctx *in = (struct jbpf_srs_ce_ctx *)(void *)(generic_ctx->data);

    int key = 0;
    example_3_msg *msg = jbpf_map_lookup_reset_elem(&msg_cache, &key);
    if (!msg)
        return 0;

    msg = (example_3_msg*)in;

    jbpf_ringbuf_output(&srs_ce_ringbuf, msg, sizeof(example_3_msg));

    return 0;
}