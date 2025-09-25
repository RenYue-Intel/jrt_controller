// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <string.h>

#include "jbpf_defs.h"
#include "jbpf_helper.h"

#include "generated_data_2.h"

jbpf_ringbuf_map(ringbuf_2, example_2_msg, 32768)  // Large ring buffer for high-throughput (32K entries)

struct jbpf_load_map_def SEC("maps") ringbuf_msg_2 = {
    .type = JBPF_MAP_TYPE_ARRAY,
    .key_size = sizeof(int),
    .value_size = sizeof(example_2_msg),
    .max_entries = 1,
};

struct jbpf_load_map_def SEC("maps") counter_2 = {
    .type = JBPF_MAP_TYPE_ARRAY,
    .key_size = sizeof(int),
    .value_size = sizeof(uint32_t),
    .max_entries = 1,
};

// TTI timing control map for RAN-realistic data generation
struct jbpf_load_map_def SEC("maps") tti_control_2 = {
    .type = JBPF_MAP_TYPE_ARRAY,
    .key_size = sizeof(int),
    .value_size = sizeof(uint64_t),
    .max_entries = 2,  // [0]=last_send_time, [1]=tti_interval_ns
};

SEC("jbpf_ran_fapi")
uint64_t
jbpf_main(void* state)
{
    void* c;
    uint64_t index = 0;

    struct jbpf_generic_ctx *generic_ctx = (struct jbpf_generic_ctx *)state;

    // BPF VERIFIER FIX: Add bounds checking before accessing data
    if (generic_ctx->data + sizeof(struct srs_info) > generic_ctx->data_end) 
    {
        return 0;  // Data too small, exit safely
    }

    struct srs_info *testcfg = (struct srs_info *)(void *)(generic_ctx->data);


    /* O-RAN OPTIMIZED: Build minimal message for fastest JRTC processing */
    example_2_msg* out;
    c = jbpf_map_lookup_reset_elem(&ringbuf_msg_2, &index);
    if (!c)
        return 0;
    out = (example_2_msg*)c;
    out->nRi = testcfg->nRi;
    out->nTANanos = testcfg->nTANanos;
    out->nTANanos = testcfg->nTANanos;
    out->nSigPwr =  testcfg->nSigPwr;
    out->nNiPwr  = testcfg->nNiPwr;
    out->timestamp_ns = jbpf_time_get_ns();

    // O-RAN CRITICAL PATH: Send to JRTC for AI/ML processing
    jbpf_ringbuf_output(&ringbuf_2, out, sizeof(example_2_msg));

    return 0;
}
