// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "jbpf_defs.h"
#include "jbpf_helper.h"

#include "srs_ce_data.h"

jbpf_ringbuf_map(ringbuf_srs_ce, srs_ce_msg, 32768)  // Large ring buffer for high-throughput (32K entries)

struct jbpf_load_map_def SEC("maps") ringbuf_msg_srs_ce = {
    .type = JBPF_MAP_TYPE_ARRAY,
    .key_size = sizeof(int),
    .value_size = sizeof(srs_ce_msg),
    .max_entries = 1,
};

struct jbpf_load_map_def SEC("maps") counter_srs_ce = {
    .type = JBPF_MAP_TYPE_ARRAY,
    .key_size = sizeof(int),
    .value_size = sizeof(uint32_t),
    .max_entries = 1,
};

// TTI timing control map for RAN-realistic data generation
struct jbpf_load_map_def SEC("maps") tti_control_srs_ce = {
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
    if (generic_ctx->data + sizeof(struct srs_ce_info) > generic_ctx->data_end) 
    {
        return 0;  // Data too small, exit safely
    }

    struct srs_ce_info *srs_ce = (struct srs_ce_info *)(void *)(generic_ctx->data);

    /* O-RAN OPTIMIZED: Build minimal message for fastest JRTC processing */
    srs_ce_msg* out;
    c = jbpf_map_lookup_reset_elem(&ringbuf_msg_srs_ce, &index);
    if (!c)
        return 0;
    out = (srs_ce_msg*)c;
    
    // Copy SRS CE data to output message
    out->ue_id = srs_ce->ue_id;
    out->num_rx_ant = srs_ce->num_rx_ant;
    out->num_rb = srs_ce->num_rb;
    out->snr_db = srs_ce->snr_db;
    out->sinr_db = srs_ce->sinr_db;
    out->rsrp_dbm = srs_ce->rsrp_dbm;
    out->cqi = srs_ce->cqi;
    out->timing_advance = srs_ce->timing_advance;
    out->timestamp_ns = jbpf_time_get_ns();

    // O-RAN CRITICAL PATH: Send to JRTC for AI/ML processing
    jbpf_ringbuf_output(&ringbuf_srs_ce, out, sizeof(srs_ce_msg));

    return 0;
}
