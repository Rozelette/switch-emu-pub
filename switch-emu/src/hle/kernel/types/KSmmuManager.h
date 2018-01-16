#pragma once
#include <stdint.h>

#include "structutils.h"

struct KSmmuManager {
    uint8_t pageTableAsids[4];
    uint64_t pageDirectories[4];
    uint64_t attachedDevicesMask;
    uint32_t regVal32Bit;
    uint32_t regVal32BitEmpty;
    uint32_t regVal36Bit;
    uint32_t regVal36BitEmpty;
};
CHECK_OFFSET(KSmmuManager, pageTableAsids, 0x0);
CHECK_OFFSET(KSmmuManager, pageDirectories, 0x8);
CHECK_OFFSET(KSmmuManager, attachedDevicesMask, 0x28);
CHECK_OFFSET(KSmmuManager, regVal32Bit, 0x30);
CHECK_OFFSET(KSmmuManager, regVal32BitEmpty, 0x34);
CHECK_OFFSET(KSmmuManager, regVal36Bit, 0x38);
CHECK_OFFSET(KSmmuManager, regVal36BitEmpty, 0x3C);
CHECK_SIZE(KSmmuManager, 0x40);
