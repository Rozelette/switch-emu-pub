#pragma once
#include <stdint.h>

#include "KLinkedListNode.h"
#include "structutils.h"

struct KMemoryBlock {
    KLinkedListNode<KMemoryBlock> memoryBlockList;
    UNKNOWN(0x10);
    uint64_t baseAddress;
    uint64_t numOfPages;
    uint32_t state;
    uint16_t ipcRefCount; // ?
    uint16_t deviceMapRefCount;
    uint8_t permission;
    UNKNOWN(0x1);
    UNKNOWN(0x1);
};
CHECK_OFFSET(KMemoryBlock, memoryBlockList, 0x0);
CHECK_OFFSET(KMemoryBlock, baseAddress, 0x20);
CHECK_OFFSET(KMemoryBlock, numOfPages, 0x28);
CHECK_OFFSET(KMemoryBlock, state, 0x30);
CHECK_OFFSET(KMemoryBlock, ipcRefCount, 0x34);
CHECK_OFFSET(KMemoryBlock, deviceMapRefCount, 0x36);
CHECK_OFFSET(KMemoryBlock, permission, 0x38);
CHECK_SIZE(KMemoryBlock, 0x40);
