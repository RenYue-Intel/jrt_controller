# Converted Python Application - first_example_converted.py

Copyright (c) Microsoft Corporation.
Licensed under the MIT license.

## Overview

This is a Python conversion of the C application `first_example.c`. The converted application demonstrates a jrt-controller app that:

1. Receives data from two different data generator codelets (data_generator and data_generator_2)
2. Processes RAN (Radio Access Network) data with minimal latency
3. Tracks performance metrics including latency measurements
4. Can optionally send control commands to the simple_input codelet

## Key Features Converted from C

### Data Structures
- **AppStateVars**: Python dataclass with performance tracking fields
  - `received_counter`: Counter for received messages
  - `min_latency_ns`, `max_latency_ns`, `total_latency_ns`: Performance metrics
  - `latency_samples`: Number of latency samples collected

### Stream Configuration
The application configures 3 streams:
1. **GENERATOR_OUT_SIDX** (stream 0): Receives UL CONFIG API data from data_generator codelet
2. **GENERATOR_OUT_2_SIDX** (stream 1): Receives SRS info from data_generator_2 codelet  
3. **SIMPLE_INPUT_IN_SIDX** (stream 2): Sends control commands to simple_input codelet (output stream)

### Handler Function
The `app_handler` function processes:
- **UL CONFIG API data**: Extracts nPDU, nSFN, and timestamp, calculates delay
- **SRS info data**: Extracts nRi, nTANanos, nSigPwr, nNiPwr, and timestamp, calculates delay

### Time Measurement
- C's `clock_gettime(CLOCK_REALTIME)` converted to Python's `time.time_ns()`
- Provides nanosecond-precision timestamps for latency measurements

### Configuration Optimizations
- Queue size: 1 (single message processing for immediate handling)
- Sleep timeout: 0.0 (maximum polling aggressiveness)
- Inactivity timeout: 0.0 (no timeout checking)

## Differences from Original C Version

1. **Language**: Converted from C to Python using ctypes for C interoperability
2. **Data Access**: Uses ctypes casting to access C structures (example_msg, example_2_msg)
3. **Time Functions**: Uses `time.time_ns()` instead of `clock_gettime()`
4. **Initialization**: Uses `sys.maxsize` instead of UINT64_MAX for initial min_latency
5. **Comments**: Preserved all comments from C version including O-RAN specific notes

## Files Created

- `first_example_converted.py`: Main Python application (converted from first_example.c)
- `deployment_converted.yaml`: Deployment configuration for the converted app

## Dependencies

The converted application requires:
- Python 3.7+ (for `time.time_ns()` support)
- jrtc_app Python bindings
- Generated data structures:
  - `generated_data.py` (for example_msg)
  - `generated_data_2.py` (for example_2_msg)
  - `simple_input.py` (for simple_input structure)

## Usage

### Prerequisites

Before running the converted application, ensure:
1. Jbpf is built (see main README.md)
2. jrt-controller is built
3. Required codelets are built:
   - data_generator
   - data_generator_2
   - simple_input

### Building Required Components

```sh
cd $JRTC_PATH/sample_apps/first_example_c
source ../../setup_jrtc_env.sh

# Build the codelets
make -C ../jbpf_codelets/data_generator
make -C ../jbpf_codelets/data_generator_2
make -C ../jbpf_codelets/simple_input

# Build the Jbpf IPC agent
make -C simple_agent_ipc
```

### Running the Converted Application

Follow the same steps as the original C application, but use the converted deployment file:

1. **Terminal 1 - Run jrtc**:
   ```sh
   cd $JRTC_PATH/sample_apps/first_example_c
   source ../../setup_jrtc_env.sh
   sudo -E ./run_jrtc.sh
   ```

2. **Terminal 2 - Run Jbpf IPC agent**:
   ```sh
   cd $JRTC_PATH/sample_apps/first_example_c
   source ../../setup_jrtc_env.sh
   sudo -E ./run_simple_agent_ipc.sh
   ```

3. **Terminal 3 - Run reverse proxy**:
   ```sh
   cd $JRTC_PATH/sample_apps/first_example_c
   source ../../setup_jrtc_env.sh
   sudo -E ./run_reverse_proxy.sh
   ```

4. **Terminal 4 - Run decoder**:
   ```sh
   cd $JRTC_PATH/sample_apps/first_example_c
   source ../../setup_jrtc_env.sh
   sudo -E ./run_decoder.sh
   ```

5. **Terminal 5 - Load the converted app**:
   ```sh
   cd $JRTC_PATH/sample_apps/first_example_c
   source ../../setup_jrtc_env.sh
   # Use jrtc-ctl to load deployment_converted.yaml
   sudo -E jrtc-ctl load deployment_converted.yaml
   ```

### Expected Output

When running successfully, you should see:
- UL CONFIG API messages with PDU, SFN, and delay information
- SRS info messages with RI, timing advance, signal power, and delay information

Example:
```
received UL CONFIG API: ulcfg_nPDU[10] ulcfg_nSFN[512] | delay[1234567]
received SRS info: nRi[2] nTANanos[-500] nSigPwr[-85] nNiPwr[-95] | delay[1234890]
```

## Technical Notes

### Ctypes Usage

The conversion uses ctypes to interact with C data structures:

```python
# Casting data pointer to example_msg structure
ran_data = ctypes.cast(data_entry.data, ctypes.POINTER(example_msg)).contents
```

This provides zero-copy access to the data, maintaining performance similar to the C version.

### Performance Considerations

The Python version maintains the same performance optimizations as the C version:
- Zero-copy data access via ctypes
- Immediate message processing (queue size = 1)
- No sleep timeouts (maximum responsiveness)
- Nanosecond-precision timing

### Future Enhancements

The control path logic is commented out (as in the C version). To enable it:
1. Uncomment the control_command creation code
2. Uncomment the `jrtc_app_router_channel_send_input_msg` call
3. Ensure the simple_input codelet is configured to receive input

## Troubleshooting

1. **Import Errors**: Ensure JRTC_APP_PATH environment variable is set
2. **Missing Modules**: Verify all Python bindings are generated (generated_data.py, etc.)
3. **Time Function**: Requires Python 3.7+ for `time.time_ns()` support
4. **Permissions**: Run with sudo for IPC socket access

## Comparison with Template

This conversion extends the basic template (`first_example_py/first_example.py`) with:
- Additional input stream (GENERATOR_OUT_2_SIDX)
- Performance tracking variables
- More detailed logging with specific field values
- O-RAN specific processing logic
- Nanosecond-precision latency measurements

## References

- Original C file: `sample_apps/first_example_c/first_example.c`
- Python template: `sample_apps/first_example_py/first_example.py`
- JRTC documentation: `docs/understand_example_apps.md`
