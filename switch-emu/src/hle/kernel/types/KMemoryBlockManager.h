#pragma once
#include <stdint.h>

#include "structutils.h"

class KMemoryBlock;

struct KMemoryBlockManager {
    KMemoryBlock* memoryBlockList;
    uint64_t maxAddr;
    uint64_t minAddr;
};
CHECK_OFFSET(KMemoryBlockManager, memoryBlockList, 0x0);
CHECK_OFFSET(KMemoryBlockManager, maxAddr, 0x8);
CHECK_OFFSET(KMemoryBlockManager, minAddr, 0x10);
CHECK_SIZE(KMemoryBlockManager, 0x18);

