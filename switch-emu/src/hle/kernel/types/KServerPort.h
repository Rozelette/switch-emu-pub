#pragma once
#include <stdint.h>

#include "KSynchronizationObject.h"
#include "KLinkedListNode.h"
#include "structutils.h"

struct KPort;

struct KServerPort : public KSynchronizationObject {
    KLinkedListNode<> incomingConnections;
    KLinkedListNode<> incomingLightConnections;
    KPort* parent;
};
CHECK_OFFSET(KServerPort, incomingConnections, 0x28);
CHECK_OFFSET(KServerPort, incomingLightConnections, 0x38);
CHECK_OFFSET(KServerPort, parent, 0x48);
CHECK_SIZE(KServerPort, 0x50);
