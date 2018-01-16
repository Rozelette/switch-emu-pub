#pragma once
#include <stdint.h>

#include "KSynchronizationObject.h"
#include "KLinkedListNode.h"
#include "structutils.h"

struct KSession;
struct KSessionRequest;

struct KServerSession : KSynchronizationObject {
    KLinkedListNode<> incomingConnections;
    KSession* parent;
    KLinkedListNode<KSessionRequest> requestList;
    KSessionRequest* activeRequest;
    UNKNOWN(0x8);
};
CHECK_OFFSET(KServerSession, incomingConnections, 0x28);
CHECK_OFFSET(KServerSession, parent, 0x38);
CHECK_OFFSET(KServerSession, requestList, 0x40);
CHECK_OFFSET(KServerSession, activeRequest, 0x50);
CHECK_SIZE(KServerSession, 0x60);
