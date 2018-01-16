#pragma once
#include <stdint.h>

#include "KMutex.h"
#include "KLinkedListNode.h"
#include "structutils.h"

struct KLockedList {
    uint64_t count;
    KLinkedListNode<> list;
    KMutex mutex;
    uint64_t maxCount;
};
CHECK_OFFSET(KLockedList, count, 0);
CHECK_OFFSET(KLockedList, list, 0x8);
CHECK_OFFSET(KLockedList, mutex, 0x18);
CHECK_OFFSET(KLockedList, maxCount, 0x20);
CHECK_SIZE(KLockedList, 0x28);
