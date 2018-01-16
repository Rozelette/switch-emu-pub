#pragma once
#include <stdint.h>

#include "structutils.h"

struct KPageTableAllocator {
    UNKNOWN(0x8);
    uint16_t* refCountTable;
    void* baseAddr;
    uint64_t size;
    UNKNOWN(0x8);
    UNKNOWN(0x8);
};
CHECK_OFFSET(KPageTableAllocator, refCountTable, 0x8);
CHECK_OFFSET(KPageTableAllocator, baseAddr, 0x10);
CHECK_OFFSET(KPageTableAllocator, size, 0x18);
CHECK_SIZE(KPageTableAllocator, 0x30);
