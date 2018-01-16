#pragma once
#include <stdint.h>

#include "KLinkedListNode.h"
#include "structutils.h"

class KSharedMemory;

struct KSharedMemoryRef {
    KLinkedListNode<KSharedMemoryRef> mappedSharedMemoryList;
    KSharedMemory* sharedMem;
    uint64_t initiallyZero; // ???
    UNKNOWN(0x20);
};
CHECK_OFFSET(KSharedMemoryRef, mappedSharedMemoryList, 0x0);
CHECK_OFFSET(KSharedMemoryRef, sharedMem, 0x10);
CHECK_OFFSET(KSharedMemoryRef, initiallyZero, 0x18);
CHECK_SIZE(KSharedMemoryRef, 0x40);

