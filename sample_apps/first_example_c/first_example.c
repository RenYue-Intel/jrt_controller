// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <assert.h>
#include <stdint.h>  // For UINT64_MAX
#include <time.h>    // For clock_gettime
#include <stdio.h>   // For printf

#include "generated_data.h" // relevant header for generated data
#include "generated_data_2.h"
#include "simple_input.h"   // relevant header for simple input

#include "jrtc_app.h" // Include the header for JrtcCppApp

// ##########################################################################
// Define the state variables for the application
typedef struct
{
    JrtcApp* app;
    int received_counter;
    
    // Performance tracking variables
    uint64_t min_latency_ns;
    uint64_t max_latency_ns;
    uint64_t total_latency_ns;
    int latency_samples;
} AppStateVars_t;

// ############################
// enum for stream indexes
enum StreamIndex
{
    GENERATOR_OUT_SIDX = 0,
    GENERATOR_OUT_2_SIDX,
    SIMPLE_INPUT_IN_SIDX
};

// ##########################################################################
// Handler callback function (this function gets called by the C library)
// OPTIMIZED FOR MINIMAL LATENCY
void
app_handler(bool timeout, int stream_idx, jrtc_router_data_entry_t* data_entry, void* s)
{
    AppStateVars_t* state = (AppStateVars_t*)s;

    if (timeout) {
        // Timeout processing (not implemented in this example)
        return;
    }

    if (stream_idx == GENERATOR_OUT_SIDX) {
        state->received_counter++;

        // Extract RAN data from received entry - zero-copy access
        example_msg* ran_data = (example_msg*)data_entry->data;

        // O-RAN SAMPLING: Process every 32nd TTI for measurement
        // With single-message processing, this should have consistent low latency
        //if ((state->received_counter & 0x1F) == 0) { 
            struct timespec curr_time;
            clock_gettime(CLOCK_REALTIME, &curr_time);
            uint64_t jrtc_receive_time_ns = curr_time.tv_nsec + curr_time.tv_sec * 1.0e9;

            uint64_t delay = jrtc_receive_time_ns - ran_data->timestamp_ns;
            uint16_t ulcfg_nPDU = ran_data->nPDU;
            uint16_t ulcfg_nSFN = ran_data->nSFN;

            printf("received UL CONFIG API: ulcfg_nPDU[%u] ulcfg_nSFN[%u] | delay[%lu]\n", ulcfg_nPDU, ulcfg_nSFN, delay);

            // ULTRA-FAST AI/ML SIMULATION: Minimal processing overhead
            // In real O-RAN: This would be AI inference + control decision
            ////simple_input control_command = {
            ////    .cnt = ran_data->cnt,                        
            ////    .timestamp_ns = jrtc_receive_time_ns,        // JRTC processing time for JRTC→Codelet2 measurement
            ////    .original_timestamp_ns = ran_data->timestamp_ns  // Original codelet1 time for total loop measurement
            ////};

            // O-RAN CONTROL PATH: Send control command back to RAN (codelet2)
            ////jrtc_app_router_channel_send_input_msg(
            ////    state->app, SIMPLE_INPUT_IN_SIDX, &control_command, sizeof(control_command));
        //}
    }
    else if (stream_idx == GENERATOR_OUT_2_SIDX) {
        state->received_counter++;

        // Extract RAN data from received entry - zero-copy access
        example_2_msg* ran_data = (example_2_msg*)data_entry->data;

            struct timespec curr_time;
            clock_gettime(CLOCK_REALTIME, &curr_time);
            uint64_t jrtc_receive_time_ns = curr_time.tv_nsec + curr_time.tv_sec * 1.0e9;

            uint64_t delay = jrtc_receive_time_ns - ran_data->timestamp_ns;
            uint8_t    nRi = ran_data->nRi;
            int16_t    nTANanos = ran_data->nTANanos;
            int16_t    nSigPwr = ran_data->nSigPwr;
            int16_t    nNiPwr = ran_data->nNiPwr;

            printf("received SRS info: nRi[%u] nTANanos[%d] nSigPwr[%d] nNiPwr[%d] | delay[%lu]\n", nRi, nTANanos, nSigPwr, nNiPwr, delay);
    }

}

// ##########################################################################
// Main function to start the app (converted from jrtc_start_app)
void
jrtc_start_app(void* args)
{

    struct jrtc_app_env* env_ctx = (struct jrtc_app_env*)args;

    int jbpf_agent_device_id = 1;

    // Configuration for the application
    const JrtcStreamCfg_t streams[] = {// GENERATOR_OUT_SIDX
                                       {
                                           {JRTC_ROUTER_REQ_DEST_ANY  /*JRTC_ROUTER_REQ_DEST_NONE */,
                                            JRTC_ROUTER_REQ_DEVICE_ID_ANY /*jbpf_agent_device_id */ ,
                                            "FirstExample://jbpf_agent/data_generator_codeletset/codelet",
                                            "ringbuf"},
                                            true, // is_rx
                                            NULL
                                            /* &(JrtcAppChannelCfg_t){  // CREATE DIRECT INPUT CHANNEL
                                            //    .is_output = false,   // Input channel
                                            //    .num_elems = 32768,   // Large buffer for O-RAN high throughput
                                            //    .elem_size = sizeof(example_msg)
                                            } */
                                       },
                                       // GENERATOR_OUT_2_SIDX
                                       {
                                           {JRTC_ROUTER_REQ_DEST_ANY  /*JRTC_ROUTER_REQ_DEST_NONE */,
                                            JRTC_ROUTER_REQ_DEVICE_ID_ANY /*jbpf_agent_device_id */ ,
                                            "FirstExample://jbpf_agent/data_generator_codeletset_2/codelet_2",
                                            "ringbuf_2"},
                                            true, // is_rx
                                            NULL
                                            /* &(JrtcAppChannelCfg_t){  // CREATE DIRECT INPUT CHANNEL
                                            //    .is_output = false,   // Input channel
                                            //    .num_elems = 32768,   // Large buffer for O-RAN high throughput
                                            //    .elem_size = sizeof(example_msg)
                                            } */
                                       },
                                       // SIMPLE_INPUT_IN_SIDX
                                       {
                                           {JRTC_ROUTER_REQ_DEST_NONE,
                                            jbpf_agent_device_id,
                                            "FirstExample://jbpf_agent/simple_input_codeletset/codelet",
                                            "input_map"},
                                           false, // is_rx
                                           NULL   // No AppChannelCfg
                                       }};

    const JrtcAppCfg_t app_cfg = {
        "FirstExample",                       // context
        1,                                    // q_size - SINGLE MESSAGE: Force immediate processing
        sizeof(streams) / sizeof(streams[0]), // num_streams
        (JrtcStreamCfg_t*)streams,            // Pointer to the streams array
        10.0f,                                // initialization_timeout_secs
        0.0f,                                 // sleep_timeout_secs - ZERO: Maximum polling aggressiveness
        0.0f                                  // inactivity_timeout_secs - ZERO: No timeout checking
    };

    // Initialize the app with performance tracking
    AppStateVars_t state = {
        .app = NULL, 
        // .agg_cnt = 0,  // ← REMOVED: No longer needed
        .received_counter = 0,
        .min_latency_ns = UINT64_MAX,
        .max_latency_ns = 0,
        .total_latency_ns = 0,
        .latency_samples = 0
    };
    state.app = jrtc_app_create(env_ctx, (JrtcAppCfg_t*)&app_cfg, app_handler, &state);

    // start app - This is blocking until the app exists
    jrtc_app_run(state.app);

    // clean up app resources
    jrtc_app_destroy(state.app);
}
