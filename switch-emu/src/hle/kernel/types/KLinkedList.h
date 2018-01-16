#pragma once
#include <stdint.h>

#include "KLinkedListNode.h"
#include "KLinkedListEntry.h"
#include "structutils.h"

template<typename T>
struct KLinkedList {
    uint64_t size;
    KLinkedListNode<KLinkedListEntry<T>> bounds; // TODO why not just have seperate head/tail data members?
};
CHECK_SIZE(KLinkedList<void>, 0x18);
