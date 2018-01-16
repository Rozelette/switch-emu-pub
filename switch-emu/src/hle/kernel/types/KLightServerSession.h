#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KLinkedListNode.h"
#include "structutils.h"

struct KLightSession;

struct KLightServerSession : KAutoObject {
    KLinkedListNode<> incomingConnections;
    KLightSession* parent;
    UNKNOWN(0x30);
};
CHECK_OFFSET(KLightServerSession, incomingConnections, 0x10);
CHECK_OFFSET(KLightServerSession, parent, 0x20);
CHECK_SIZE(KLightServerSession, 0x58);
