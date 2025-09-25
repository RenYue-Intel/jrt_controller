// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#include <stdint.h>  // For UINT64_MAX

typedef struct
{
    uint8_t    nRi;
    int16_t    nTANanos;
    int16_t    nSigPwr;
    int16_t    nNiPwr;
    uint64_t timestamp_ns;  // Timestamp when data was generated
} example_2_msg;

struct srs_info
{
    uint8_t    nRi;           // Selected RI based on SRS, range 1-4 for UE using layer 1-4
    int16_t    nTANanos;      // Reported Timing advance in nanoseconds. value -16800 -> 16800 nanosecondsã€‚0xFFFF is not vaild this field.
    int16_t    nSigPwr;       // Signal power in 1/256 dBm steps using L2 port index.
    int16_t    nNiPwr; 
};