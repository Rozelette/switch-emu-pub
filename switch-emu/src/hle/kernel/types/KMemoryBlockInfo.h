#pragma once
#include <stdint.h>

#include "structutils.h"

struct KMemoryBlockInfo {
    uint64_t baseAddress;
    uint64_t size;
    uint32_t state;
    uint8_t permission;
    uint8_t attribute;
    UNKNOWN(0x2);
    uint16_t ipcRefCount; // ?
    uint16_t deviceMapRefCount;
};
CHECK_OFFSET(KMemoryBlockInfo, baseAddress, 0x0);
CHECK_OFFSET(KMemoryBlockInfo, size, 0x8);
CHECK_OFFSET(KMemoryBlockInfo, state, 0x10);
CHECK_OFFSET(KMemoryBlockInfo, permission, 0x14);
CHECK_OFFSET(KMemoryBlockInfo, attribute, 0x15);
CHECK_OFFSET(KMemoryBlockInfo, ipcRefCount, 0x18);
CHECK_OFFSET(KMemoryBlockInfo, deviceMapRefCount, 0x1A);
CHECK_SIZE(KMemoryBlockInfo, 0x20);
