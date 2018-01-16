#pragma once
#include <stdint.h>

#include "common\structutils.h"

struct NRR_header {
    uint32_t magic;
    UNKNOWN(0x1C);
    uint64_t titleID_mask;
    uint64_t titleID_pattern;
    uint8_t modulus[0x100];
    uint8_t firstSignature[0x100];
    uint8_t secondSignature[0x100];
    uint64_t titleID;
    uint32_t size;
    UNKNOWN(0x04);
    uint32_t hashOff;
    uint32_t hashCount;
    UNKNOWN(0x08);
    // hashes follow
};

CHECK_OFFSET(NRR_header, magic, 0x0);
CHECK_OFFSET(NRR_header, titleID_mask, 0x20);
CHECK_OFFSET(NRR_header, titleID_pattern, 0x28);
CHECK_OFFSET(NRR_header, modulus, 0x30);
CHECK_OFFSET(NRR_header, firstSignature, 0x130);
CHECK_OFFSET(NRR_header, secondSignature, 0x230);
CHECK_OFFSET(NRR_header, titleID, 0x330);
CHECK_OFFSET(NRR_header, size, 0x338);
CHECK_OFFSET(NRR_header, hashOff, 0x340);
CHECK_OFFSET(NRR_header, hashCount, 0x344);
CHECK_SIZE(NRR_header, 0x350);
