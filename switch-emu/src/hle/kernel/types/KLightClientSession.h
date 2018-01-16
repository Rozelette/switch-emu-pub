#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "structutils.h"

struct KLightSession;
struct KClientPort;
class KProcess;

struct KLightClientSession : KAutoObject {
    KLightSession* parent;
    bool hasInited;
    KClientPort* parentPort;
    UNKNOWN(0x8);
    KProcess* creatorProcess;
};
CHECK_OFFSET(KLightClientSession, parent, 0x10);
CHECK_OFFSET(KLightClientSession, hasInited, 0x18);
CHECK_OFFSET(KLightClientSession, parentPort, 0x20);
CHECK_OFFSET(KLightClientSession, creatorProcess, 0x30);
CHECK_SIZE(KLightClientSession, 0x38);
