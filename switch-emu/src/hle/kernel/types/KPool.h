#pragma once
#include <stdint.h>

#include "structutils.h"

struct KPoolHeader;

struct KPool {
    KPoolHeader* firstFreeChunk;
    UNKNOWN(0x8);
    UNKNOWN(0x8);
    void* singletonTableEntryPointer;
    KPool* parent;
    UNKNOWN(0x8);
    uint64_t dmaProtectionKey[2];
};
CHECK_OFFSET(KPool, firstFreeChunk, 0x0);
CHECK_OFFSET(KPool, singletonTableEntryPointer, 0x18);
CHECK_OFFSET(KPool, parent, 0x20);
CHECK_OFFSET(KPool, dmaProtectionKey, 0x30);
CHECK_SIZE(KPool, 0x40);
