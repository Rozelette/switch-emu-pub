#pragma once
#include <stdint.h>
#include <type_traits>

#include "structutils.h"

template<typename T = void>
class KLinkedListNode {
    using NodeType = std::conditional_t<std::is_same<T, void>::value, KLinkedListNode<>, T>;
    NodeType* prev;
    NodeType* next;
};
CHECK_SIZE(KLinkedListNode<>, 0x10);
