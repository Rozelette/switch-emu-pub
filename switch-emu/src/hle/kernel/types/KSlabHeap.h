#pragma once
#include <stdint.h>

#include "structutils.h"

struct KSlabHeap {
    void* freeListHead;
    UNKNOWN(0x8);
    void* heapPointer;
    uint64_t totalHeapSize;
    uint64_t singleObjectSize;
};
CHECK_OFFSET(KSlabHeap, freeListHead, 0x0);
CHECK_OFFSET(KSlabHeap, heapPointer, 0x10);
CHECK_OFFSET(KSlabHeap, totalHeapSize, 0x18);
CHECK_OFFSET(KSlabHeap, singleObjectSize, 0x20);
CHECK_SIZE(KSlabHeap, 0x28);
