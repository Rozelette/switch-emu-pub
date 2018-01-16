#pragma once
#include <stdint.h>

#include "KLinkedListNode.h"
#include "structutils.h"

template<typename T>
struct KLinkedListEntry {
    KLinkedListNode<KLinkedListEntry<T>> nodeForList;
    T data;
};
CHECK_OFFSET(KLinkedListEntry<uint64_t>, nodeForList, 0x0);
CHECK_OFFSET(KLinkedListEntry<uint64_t>, data, 0x10);
CHECK_SIZE(KLinkedListEntry<uint64_t>, 0x18);
