# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.
import os
import sys
import ctypes
import time
from dataclasses import dataclass

JRTC_APP_PATH = os.environ.get("JRTC_APP_PATH")
if JRTC_APP_PATH is None:
    raise ValueError("JRTC_APP_PATH not set")
sys.path.append(f"{JRTC_APP_PATH}")

import jrtc_app
from jrtc_app import *

# Import the necessary modules for data structures
from generated_data import example_msg
from generated_data_2 import example_2_msg
from simple_input import simple_input
from jrtc_bindings import (
    struct_jrtc_router_data_entry,
)


##########################################################################
# Define the state variables for the application
@dataclass
class AppStateVars:
    app: JrtcApp
    received_counter: int
    # Performance tracking variables
    min_latency_ns: int
    max_latency_ns: int
    total_latency_ns: int
    latency_samples: int


##########################################################################
# Stream indexes enum (translated from C enum)
GENERATOR_OUT_SIDX = 0
GENERATOR_OUT_2_SIDX = 1
SIMPLE_INPUT_IN_SIDX = 2


##########################################################################
def app_handler(
    timeout: bool,
    stream_idx: int,
    data_entry: struct_jrtc_router_data_entry,
    state: AppStateVars,
):
    """
    Handler callback function (this function gets called by the C library)
    OPTIMIZED FOR MINIMAL LATENCY
    """
    if timeout:
        # Timeout processing (not implemented in this example)
        return

    if stream_idx == GENERATOR_OUT_SIDX:
        state.received_counter += 1

        # Extract RAN data from received entry - zero-copy access
        # Cast the data pointer to example_msg structure
        ran_data = ctypes.cast(data_entry.data, ctypes.POINTER(example_msg)).contents

        # O-RAN SAMPLING: Process every 32nd TTI for measurement
        # With single-message processing, this should have consistent low latency
        # Get current time in nanoseconds (Python equivalent of clock_gettime)
        curr_time_ns = time.time_ns()  # Python 3.7+ provides time_ns()

        delay = curr_time_ns - ran_data.timestamp_ns
        ulcfg_nPDU = ran_data.nPDU
        ulcfg_nSFN = ran_data.nSFN

        print(
            f"received UL CONFIG API: ulcfg_nPDU[{ulcfg_nPDU}] ulcfg_nSFN[{ulcfg_nSFN}] | delay[{delay}]",
            flush=True,
        )

        # ULTRA-FAST AI/ML SIMULATION: Minimal processing overhead
        # In real O-RAN: This would be AI inference + control decision
        # Commented out as in C version:
        # control_command = simple_input()
        # control_command.cnt = ran_data.cnt
        # control_command.timestamp_ns = curr_time_ns  # JRTC processing time for JRTC→Codelet2 measurement
        # control_command.original_timestamp_ns = ran_data.timestamp_ns  # Original codelet1 time for total loop measurement

        # O-RAN CONTROL PATH: Send control command back to RAN (codelet2)
        # data_to_send = bytes(control_command)
        # res = jrtc_app_router_channel_send_input_msg(
        #     state.app, SIMPLE_INPUT_IN_SIDX, data_to_send, len(data_to_send)
        # )

    elif stream_idx == GENERATOR_OUT_2_SIDX:
        state.received_counter += 1

        # Extract RAN data from received entry - zero-copy access
        # Cast the data pointer to example_2_msg structure
        ran_data = ctypes.cast(data_entry.data, ctypes.POINTER(example_2_msg)).contents

        # Get current time in nanoseconds
        curr_time_ns = time.time_ns()

        delay = curr_time_ns - ran_data.timestamp_ns
        nRi = ran_data.nRi
        nTANanos = ran_data.nTANanos
        nSigPwr = ran_data.nSigPwr
        nNiPwr = ran_data.nNiPwr

        print(
            f"received SRS info: nRi[{nRi}] nTANanos[{nTANanos}] nSigPwr[{nSigPwr}] nNiPwr[{nNiPwr}] | delay[{delay}]",
            flush=True,
        )


##########################################################################
# Main function to start the app (converted from jrtc_start_app)
def jrtc_start_app(capsule):
    print("Starting FirstExample app...", flush=True)

    jbpf_agent_device_id = 1

    # Configuration for the application (3 streams)
    streams = [
        # GENERATOR_OUT_SIDX
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(
                JRTC_ROUTER_REQ_DEST_ANY,
                JRTC_ROUTER_REQ_DEVICE_ID_ANY,
                b"FirstExample://jbpf_agent/data_generator_codeletset/codelet",
                b"ringbuf",
            ),
            True,  # is_rx
            None,  # No AppChannelCfg
        ),
        # GENERATOR_OUT_2_SIDX
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(
                JRTC_ROUTER_REQ_DEST_ANY,
                JRTC_ROUTER_REQ_DEVICE_ID_ANY,
                b"FirstExample://jbpf_agent/data_generator_codeletset_2/codelet_2",
                b"ringbuf_2",
            ),
            True,  # is_rx
            None,  # No AppChannelCfg
        ),
        # SIMPLE_INPUT_IN_SIDX
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(
                JRTC_ROUTER_REQ_DEST_NONE,
                jbpf_agent_device_id,
                b"FirstExample://jbpf_agent/simple_input_codeletset/codelet",
                b"input_map",
            ),
            False,  # is_rx
            None,  # No AppChannelCfg
        ),
    ]

    app_cfg = JrtcAppCfg_t(
        b"FirstExample",  # context
        1,  # q_size - SINGLE MESSAGE: Force immediate processing
        len(streams),  # num_streams
        (JrtcStreamCfg_t * len(streams))(*streams),  # streams
        10.0,  # initialization_timeout_secs
        0.0,  # sleep_timeout_secs - ZERO: Maximum polling aggressiveness
        0.0,  # inactivity_timeout_secs - ZERO: No timeout checking
    )

    # Initialize the app with performance tracking
    # Using sys.maxsize as Python equivalent of UINT64_MAX for initial min_latency
    state = AppStateVars(
        app=None,
        received_counter=0,
        min_latency_ns=sys.maxsize,
        max_latency_ns=0,
        total_latency_ns=0,
        latency_samples=0,
    )
    state.app = jrtc_app_create(capsule, app_cfg, app_handler, state, log_level="INFO")

    # run the app - This is blocking until the app exists
    jrtc_app_run(state.app)

    # clean up app resources
    jrtc_app_destroy(state.app)
