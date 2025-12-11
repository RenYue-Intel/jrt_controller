# SRS CE Data Collection Codelet

This codelet collects Sounding Reference Signal (SRS) Channel Estimate (CE) data from FlexRAN L1 software for processing in the JRTC (JRT Controller) system.

## Overview

The SRS CE codelet captures channel estimation information from the FlexRAN L1 layer, including signal quality metrics, channel state information, and timing parameters. This data can be used for:

- AI/ML-based radio resource optimization
- Predictive maintenance and anomaly detection
- Real-time performance monitoring
- Advanced beamforming and MIMO optimization

## Data Structure

The codelet processes the following SRS Channel Estimate parameters:

### Input Structure (`srs_ce_info`)
- **ue_id**: UE (User Equipment) identifier
- **num_rx_ant**: Number of receive antennas (typically 1-4)
- **num_rb**: Number of resource blocks for SRS
- **snr_db**: Signal-to-Noise Ratio in dB (scaled by 10)
- **sinr_db**: Signal-to-Interference-plus-Noise Ratio in dB (scaled by 10)
- **rsrp_dbm**: Reference Signal Received Power in dBm (scaled by 10)
- **cqi**: Channel Quality Indicator (0-15)
- **timing_advance**: Timing advance value

### Output Structure (`srs_ce_msg`)
Same fields as input structure, plus:
- **timestamp_ns**: Timestamp when CE data was generated (nanoseconds)

## Architecture

```
FlexRAN L1 → srs_ce_hook → srs_ce_codelet → ringbuf_srs_ce → JRTC App
```

The codelet:
1. Receives SRS CE data via the `srs_ce_hook` hook point
2. Performs bounds checking for BPF verifier compliance
3. Copies data to output message with timestamp
4. Sends data through ring buffer to JRTC for processing

## Configuration

The codelet is configured via `codeletset.yaml`:

```yaml
codeletset_id: srs_ce_codeletset
codelet_descriptor:
  - codelet_name: srs_ce_codelet
    codelet_path: ${JRTC_PATH}/sample_apps/jbpf_codelets/networking.wireless.flexran.flexran-l1-sw/srs_ce_codelet.o
    hook_name: srs_ce_hook
    priority: 2
    runtime_threshold: 50000
    out_io_channel:
      - name: ringbuf_srs_ce
```

## Building

Prerequisites:
- Clang with BPF target support
- JRTC build environment (source `setup_jrtc_env.sh`)
- ctypesgen for Python bindings

Build the codelet:

```bash
cd sample_apps/jbpf_codelets/networking.wireless.flexran.flexran-l1-sw
make
```

This will generate:
- `srs_ce_codelet.o`: Compiled BPF object
- `srs_ce_data.py`: Python bindings for the data structures

## Integration with JRTC Applications

To consume SRS CE data in a JRTC application:

### C Application
```c
#include "srs_ce_data.h"

// In your handler callback
void app_handler(bool timeout, int stream_idx, jrtc_router_data_entry_t* data_entry, void* state) {
    if (stream_idx == SRS_CE_STREAM_IDX) {
        srs_ce_msg* ce_data = (srs_ce_msg*)data_entry->data;
        
        // Process SRS CE data
        printf("UE %u: SNR=%d.%d dB, SINR=%d.%d dB, CQI=%u\n",
               ce_data->ue_id,
               ce_data->snr_db / 10, abs(ce_data->snr_db % 10),
               ce_data->sinr_db / 10, abs(ce_data->sinr_db % 10),
               ce_data->cqi);
    }
}
```

### Python Application
```python
from srs_ce_data import srs_ce_msg

# In your message handler
def handle_srs_ce(data):
    ce = srs_ce_msg.from_buffer_copy(data)
    print(f"UE {ce.ue_id}: SNR={ce.snr_db/10:.1f} dB, "
          f"SINR={ce.sinr_db/10:.1f} dB, CQI={ce.cqi}")
```

## Performance Characteristics

- **Ring buffer size**: 32,768 entries for high-throughput scenarios
- **Priority**: 2 (standard priority for data collection)
- **Runtime threshold**: 50,000 (maximum execution time)
- **Latency**: Optimized for O-RAN real-time requirements

## Use Cases

1. **Radio Resource Management**: Use CQI and SINR data for dynamic scheduling decisions
2. **Handover Optimization**: Leverage RSRP and SNR for handover triggering
3. **Beam Management**: Use channel estimates for beamforming optimization
4. **Anomaly Detection**: Monitor sudden changes in signal quality metrics
5. **QoS Assurance**: Track per-UE channel quality for QoS enforcement

## License

Copyright (c) Microsoft Corporation.
Licensed under the MIT license.
