#pragma once
#include <stdint.h>

#include "structutils.h"

struct KBufferDescriptor {
    uint64_t userVirtAddr;
    uint64_t size;
    UNKNOWN(0x8);
    uint64_t bufferMemoryState;
};
CHECK_OFFSET(KBufferDescriptor, userVirtAddr, 0x0);
CHECK_OFFSET(KBufferDescriptor, size, 0x8);
CHECK_OFFSET(KBufferDescriptor, bufferMemoryState, 0x18);
CHECK_SIZE(KBufferDescriptor, 0x20);

