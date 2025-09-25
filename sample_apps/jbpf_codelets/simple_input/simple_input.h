// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#include <stdint.h>  // For UINT64_MAX

typedef struct
{
    int cnt;  // Original counter value from data generator
    uint64_t timestamp_ns;       // JRTC processing timestamp (for JRTC→Codelet2 measurement)
    uint64_t original_timestamp_ns; // Original codelet1 timestamp (for total loop measurement)
} simple_input;