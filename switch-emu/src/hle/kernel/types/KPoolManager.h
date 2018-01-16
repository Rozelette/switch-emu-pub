#pragma once
#include <stdint.h>

#include "KPoolAllocator.h"
#include "structutils.h"

struct KPartitionInfo;

struct KPoolManager {
    KPoolAllocator allocator0;
    KPartitionInfo* partitionInfo0;
    KPoolAllocator allocator1;
    KPartitionInfo* partitionInfo1;
};
CHECK_OFFSET(KPoolManager, allocator0, 0);
CHECK_OFFSET(KPoolManager, partitionInfo0, 0x220);
CHECK_OFFSET(KPoolManager, allocator1, 0x228);
CHECK_OFFSET(KPoolManager, partitionInfo1, 0x448);
CHECK_SIZE(KPoolManager, 0x450);
