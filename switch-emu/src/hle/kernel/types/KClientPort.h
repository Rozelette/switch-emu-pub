#pragma once
#include <stdint.h>

#include "KSynchronizationObject.h"
#include "structutils.h"

struct KPort;

struct KClientPort : public KSynchronizationObject {
    uint32_t numSessions;
    uint32_t maxSessions;
    KPort* parent;
};
CHECK_OFFSET(KClientPort, numSessions, 0x28);
CHECK_OFFSET(KClientPort, maxSessions, 0x2C);
CHECK_OFFSET(KClientPort, parent, 0x30);
CHECK_SIZE(KClientPort, 0x38);
