// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#include <stdint.h>  // For UINT64_MAX
#include "jbpf_defs.h"
#include "jbpf_helper.h"
#include "simple_input.h"

jbpf_control_input_map(input_map, simple_input, 65536)  // ULTRA-LARGE buffer for O-RAN control commands

SEC("jbpf_ran_fapi") uint64_t jbpf_main(void* state)
{
    simple_input control_cmd;

    // O-RAN CONTROL LOOP: Process control commands with minimum latency
    while (jbpf_control_input_receive(&input_map, &control_cmd, sizeof(simple_input)) > 0) {
        uint64_t codelet2_receive_time_ns = jbpf_time_get_ns();

        // DETAILED O-RAN MEASUREMENTS:
        uint64_t codelet1_to_jrtc_ns = control_cmd.timestamp_ns - control_cmd.original_timestamp_ns;
        uint64_t jrtc_to_codelet2_ns = codelet2_receive_time_ns - control_cmd.timestamp_ns;

        // O-RAN TRANSPORT MEASUREMENTS: Individual and total latencies
        jbpf_printf_debug("O-RAN [C1→JRTC]: cnt:%d  %llu ns | [JRTC→C2]: %llu ns\n", 
                        control_cmd.cnt, codelet1_to_jrtc_ns, jrtc_to_codelet2_ns);
    }

    return 0;
}
