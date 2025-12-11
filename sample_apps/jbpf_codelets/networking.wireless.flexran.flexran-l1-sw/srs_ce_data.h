// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#include <stdint.h>

// SRS Channel Estimate (CE) data structure
// Based on 3GPP specifications and FlexRAN L1 requirements
typedef struct
{
    uint16_t   ue_id;         // UE identifier
    uint8_t    num_rx_ant;    // Number of receive antennas (typically 1-4)
    uint8_t    num_rb;        // Number of resource blocks for SRS
    int16_t    snr_db;        // Signal-to-Noise Ratio in dB (scaled by 10)
    int16_t    sinr_db;       // Signal-to-Interference-plus-Noise Ratio in dB (scaled by 10)
    int16_t    rsrp_dbm;      // Reference Signal Received Power in dBm (scaled by 10)
    uint8_t    cqi;           // Channel Quality Indicator (0-15)
    uint16_t   timing_advance; // Timing advance value
    uint64_t   timestamp_ns;  // Timestamp when CE data was generated
} srs_ce_msg;

// Input structure for SRS CE processing
struct srs_ce_info
{
    uint16_t   ue_id;         // UE identifier
    uint8_t    num_rx_ant;    // Number of receive antennas
    uint8_t    num_rb;        // Number of resource blocks
    int16_t    snr_db;        // SNR in dB (scaled by 10)
    int16_t    sinr_db;       // SINR in dB (scaled by 10)
    int16_t    rsrp_dbm;      // RSRP in dBm (scaled by 10)
    uint8_t    cqi;           // Channel Quality Indicator
    uint16_t   timing_advance; // Timing advance
};
