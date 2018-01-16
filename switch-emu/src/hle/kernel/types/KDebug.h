#pragma once
#include <stdint.h>

#include "KSynchronizationObject.h"
#include "KLinkedListNode.h"
#include "KMutex.h"
#include "structutils.h"

class KDebugEvent;
class KProcess;

struct KDebug : public KSynchronizationObject {
    KLinkedListNode<KDebugEvent> eventList;
    uint32_t flags;
    KProcess* process;
    KMutex mutex;
};
CHECK_OFFSET(KDebug, eventList, 0x28);
CHECK_OFFSET(KDebug, flags, 0x38);
CHECK_OFFSET(KDebug, process, 0x40);
CHECK_OFFSET(KDebug, mutex, 0x48);
CHECK_SIZE(KDebug, 0x50);
