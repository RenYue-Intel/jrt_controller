// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#include <stdint.h>  // For UINT64_MAX

typedef struct
{
    uint16_t   nPDU;
    uint16_t   nSFN;
    uint64_t timestamp_ns;  // Timestamp when data was generated
} example_msg;

struct ul_cfg
{
    uint16_t   nPDU;       // Number of PDUs that are included in this message.Range 0 -> 255
    uint16_t   nSFN;
};