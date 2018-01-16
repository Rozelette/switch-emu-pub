#pragma once
#include <stdint.h>

#include "KPool.h"
#include "structutils.h"

struct KPoolAllocator {
    void* memoryBase;
    uint64_t memorySize;
    KPool pools[8];
    int32_t numPools;
    UNKNOWN(0xC);
};
CHECK_OFFSET(KPoolAllocator, memoryBase, 0x0);
CHECK_OFFSET(KPoolAllocator, memorySize, 0x8);
CHECK_OFFSET(KPoolAllocator, pools, 0x10);
CHECK_OFFSET(KPoolAllocator, numPools, 0x210);
CHECK_SIZE(KPoolAllocator, 0x220);
