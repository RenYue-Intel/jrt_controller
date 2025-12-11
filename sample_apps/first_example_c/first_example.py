# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

import os
import sys
import time
import ctypes
from dataclasses import dataclass, field
from typing import Optional, Type

JRTC_APP_PATH = os.environ.get("JRTC_APP_PATH")
if JRTC_APP_PATH is None:
    raise ValueError("JRTC_APP_PATH not set")
sys.path.append(f"{JRTC_APP_PATH}")

from jrtc_app import (
    JRTC_ROUTER_REQ_DEST_ANY,
    JRTC_ROUTER_REQ_DEST_NONE,
    JRTC_ROUTER_REQ_DEVICE_ID_ANY,
    JrtcApp,
    JrtcAppCfg_t,
    JrtcStreamCfg_t,
    JrtcStreamIdCfg_t,
    jrtc_app_create,
    jrtc_app_destroy,
    jrtc_app_run,
    struct_jrtc_router_data_entry,
)

UINT64_MAX = (1 << 64) - 1

try:
    from generated_data import example_msg  # type: ignore[attr-defined]
except Exception:  # pragma: no cover - fallback when module is unavailable
    class example_msg(ctypes.Structure):
        _fields_ = [
            ("nPDU", ctypes.c_uint16),
            ("nSFN", ctypes.c_uint16),
            ("timestamp_ns", ctypes.c_uint64),
        ]

try:
    from generated_data_2 import example_2_msg  # type: ignore[attr-defined]
except Exception:  # pragma: no cover - fallback when module is unavailable
    class example_2_msg(ctypes.Structure):
        _fields_ = [
            ("nRi", ctypes.c_uint8),
            ("nTANanos", ctypes.c_int16),
            ("nSigPwr", ctypes.c_int16),
            ("nNiPwr", ctypes.c_int16),
            ("timestamp_ns", ctypes.c_uint64),
        ]

try:
    from generated_data_3 import example_3_msg, MAX_SRS_CE_BLOCKS, MAX_SRS_CE_BYTES  # type: ignore[attr-defined]
except Exception:  # pragma: no cover - fallback when module is unavailable
    MAX_SRS_CE_BLOCKS = 256
    MAX_SRS_CE_BYTES = 65535

    class jbpf_srs_ce_block(ctypes.Structure):
        _fields_ = [
            ("offset", ctypes.c_uint32),
            ("size", ctypes.c_uint32),
        ]

    class example_3_msg(ctypes.Structure):
        _fields_ = [
            ("cell_id", ctypes.c_uint16),
            ("slot", ctypes.c_uint16),
            ("start_ant", ctypes.c_uint16),
            ("ant_count", ctypes.c_uint16),
            ("ue_count", ctypes.c_uint16),
            ("port_count", ctypes.c_uint16),
            ("timestamp_ns", ctypes.c_uint64),
            ("block_count", ctypes.c_uint32),
            ("payload_len", ctypes.c_uint32),
            ("blocks", jbpf_srs_ce_block * MAX_SRS_CE_BLOCKS),
            ("payload", ctypes.c_uint8 * MAX_SRS_CE_BYTES),
        ]

@dataclass
class AppStateVars:
    app: Optional[JrtcApp] = None
    received_counter: int = 0
    min_latency_ns: int = field(default_factory=lambda: UINT64_MAX)
    max_latency_ns: int = 0
    total_latency_ns: int = 0
    latency_samples: int = 0

    def record_latency(self, latency_ns: int) -> None:
        if latency_ns < 0:
            return
        if latency_ns < self.min_latency_ns:
            self.min_latency_ns = latency_ns
        if latency_ns > self.max_latency_ns:
            self.max_latency_ns = latency_ns
        self.total_latency_ns += latency_ns
        self.latency_samples += 1

GENERATOR_OUT_SIDX = 0
GENERATOR_OUT_2_SIDX = 1
GENERATOR_OUT_3_SIDX = 2
SIMPLE_INPUT_IN_SIDX = 3

def _cast_data_entry(
    data_entry: struct_jrtc_router_data_entry, struct_type: Type[ctypes.Structure]
) -> Optional[ctypes.Structure]:
    data_ptr = getattr(data_entry, "data", None)
    if not data_ptr:
        return None
    expected_size = ctypes.sizeof(struct_type)
    data_len = getattr(data_entry, "data_len", expected_size)
    if data_len < expected_size:
        return None
    return ctypes.cast(data_ptr, ctypes.POINTER(struct_type)).contents

def app_handler(
    timeout: bool,
    stream_idx: int,
    data_entry: struct_jrtc_router_data_entry,
    state: AppStateVars,
) -> None:
    if timeout:
        return

    if stream_idx == GENERATOR_OUT_SIDX:
        state.received_counter += 1
        ran_data = _cast_data_entry(data_entry, example_msg)
        if ran_data is None:
            return

        curr_time_ns = time.time_ns()
        delay_ns = curr_time_ns - int(ran_data.timestamp_ns)
        state.record_latency(delay_ns)

        print(
            "received UL CONFIG API: ulcfg_nPDU[{0}] ulcfg_nSFN[{1}] | delay[{2}]".format(
                int(ran_data.nPDU), int(ran_data.nSFN), max(delay_ns, 0)
            ),
            flush=True,
        )

    elif stream_idx == GENERATOR_OUT_2_SIDX:
        state.received_counter += 1
        ran_data = _cast_data_entry(data_entry, example_2_msg)
        if ran_data is None:
            return

        curr_time_ns = time.time_ns()
        delay_ns = curr_time_ns - int(ran_data.timestamp_ns)
        state.record_latency(delay_ns)

        print(
            "received SRS info: nRi[{0}] nTANanos[{1}] nSigPwr[{2}] nNiPwr[{3}] | delay[{4}]".format(
                int(ran_data.nRi),
                int(ran_data.nTANanos),
                int(ran_data.nSigPwr),
                int(ran_data.nNiPwr),
                max(delay_ns, 0),
            ),
            flush=True,
        )

    elif stream_idx == GENERATOR_OUT_3_SIDX:
        state.received_counter += 1
        ran_srsce_recv = _cast_data_entry(data_entry, example_3_msg)
        if ran_srsce_recv is None:
            return

        curr_time_ns = time.time_ns()
        delay_ns = curr_time_ns - int(ran_srsce_recv.timestamp_ns)
        state.record_latency(delay_ns)

        block_count = int(ran_srsce_recv.block_count)
        payload_len = int(ran_srsce_recv.payload_len)
        print(
            "received SRS CE header and payload: block_count[{0}] payload_len[{1}] | delay[{2}]".format(
                block_count, payload_len, max(delay_ns, 0)
            ),
            flush=True,
        )

        if payload_len > 0:
            bytes_to_dump = min(payload_len, 16, MAX_SRS_CE_BYTES)
            payload_slice = bytes(ran_srsce_recv.payload[:bytes_to_dump])
            hex_dump = " ".join(f"{byte:02X}" for byte in payload_slice)
            print(f"payload[0:{bytes_to_dump}): {hex_dump}", flush=True)


def jrtc_start_app(capsule) -> None:
    print("Starting FirstExample app...", flush=True)

    jbpf_agent_device_id = 1

    streams = [
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(
                JRTC_ROUTER_REQ_DEST_ANY,
                JRTC_ROUTER_REQ_DEVICE_ID_ANY,
                b"FirstExample://jbpf_agent/data_generator_codeletset/codelet",
                b"ringbuf",
            ),
            True,
            None,
        ),
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(
                JRTC_ROUTER_REQ_DEST_ANY,
                JRTC_ROUTER_REQ_DEVICE_ID_ANY,
                b"FirstExample://jbpf_agent/data_generator_codeletset_2/codelet_2",
                b"ringbuf_2",
            ),
            True,
            None,
        ),
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(
                JRTC_ROUTER_REQ_DEST_ANY,
                JRTC_ROUTER_REQ_DEVICE_ID_ANY,
                b"FirstExample://jbpf_agent/data_generator_codeletset_3/codelet_3",
                b"srs_ce_ringbuf",
            ),
            True,
            None,
        ),
    ]

    streams_array = (JrtcStreamCfg_t * len(streams))(*streams)

    app_cfg = JrtcAppCfg_t(
        b"FirstExample",
        1,
        len(streams),
        streams_array,
        10.0,
        0.0,
        0.0,
    )

    state = AppStateVars()
    state.app = jrtc_app_create(capsule, app_cfg, app_handler, state, log_level="INFO")

    jrtc_app_run(state.app)
    jrtc_app_destroy(state.app)
    state.app = None

