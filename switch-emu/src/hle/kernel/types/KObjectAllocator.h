#pragma once
#include <stdint.h>

#include "KLockedList.h"
#include "KSlabHeap.h"
#include "structutils.h"

struct KObjectAllocator {
    KLockedList lockedList;
    KSlabHeap slabHeap;
};
CHECK_OFFSET(KObjectAllocator, lockedList, 0x0);
CHECK_OFFSET(KObjectAllocator, slabHeap, 0x28);
CHECK_SIZE(KObjectAllocator, 0x50);
