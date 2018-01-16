#pragma once
#include <stdint.h>

#include "KLinkedListNode.h"
#include "structutils.h"

struct KDebugEvent {
    KLinkedListNode<KDebugEvent> eventList;
    uint32_t eventType;
    uint32_t threadId;
    uint32_t flags;
    UNKNOWN(0x2);
    UNKNOWN(0x1);
    UNKNOWN(0x41);
};
CHECK_OFFSET(KDebugEvent, eventList, 0x0);
CHECK_OFFSET(KDebugEvent, eventType, 0x10);
CHECK_OFFSET(KDebugEvent, threadId, 0x14);
CHECK_OFFSET(KDebugEvent, flags, 0x18);
CHECK_SIZE(KDebugEvent, 0x60);

