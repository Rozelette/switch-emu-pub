#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "structutils.h"

struct KSession;
struct KClientPort;
class KProcess;

struct KClientSession : KAutoObject {
    KSession* parent;
    bool hasInited;
    KClientPort* parentPort;
    UNKNOWN(0x8);
    KProcess* creatorProcess;
};
CHECK_OFFSET(KClientSession, parent, 0x10);
CHECK_OFFSET(KClientSession, hasInited, 0x18);
CHECK_OFFSET(KClientSession, parentPort, 0x20);
CHECK_OFFSET(KClientSession, creatorProcess, 0x30);
CHECK_SIZE(KClientSession, 0x38);
