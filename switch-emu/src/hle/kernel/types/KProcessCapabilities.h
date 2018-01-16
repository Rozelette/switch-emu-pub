#pragma once
#include <stdint.h>

#include "structutils.h"

struct KProcessCapabilities {
    uint8_t svcAccessMask[16];
    uint8_t irqAccessMask[128];
    uint64_t allowedCpuIdBitmask;
    uint64_t allowedThreadPrioBitmask;
    uint32_t debuggingFlags; // (bit0: can be debugged, bit1: can debug others)
    uint32_t handleTableSize;
    uint32_t kernelReleaseVersion;
    uint32_t applicationType;
};
CHECK_OFFSET(KProcessCapabilities, svcAccessMask, 0x0);
CHECK_OFFSET(KProcessCapabilities, irqAccessMask, 0x10);
CHECK_OFFSET(KProcessCapabilities, allowedCpuIdBitmask, 0x90);
CHECK_OFFSET(KProcessCapabilities, allowedThreadPrioBitmask, 0x98);
CHECK_OFFSET(KProcessCapabilities, debuggingFlags, 0xA0);
CHECK_OFFSET(KProcessCapabilities, handleTableSize, 0xA4);
CHECK_OFFSET(KProcessCapabilities, kernelReleaseVersion, 0xA8);
CHECK_OFFSET(KProcessCapabilities, applicationType, 0xAC);
CHECK_SIZE(KProcessCapabilities, 0xB0);
