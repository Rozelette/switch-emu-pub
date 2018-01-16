#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KServerSession.h"
#include "KClientSession.h"
#include "structutils.h"

struct KSession : public KAutoObject {
    KServerSession serverSide;
    KClientSession clientSide;
    bool hasInited;
};
CHECK_OFFSET(KSession, serverSide, 0x10);
CHECK_OFFSET(KSession, clientSide, 0x70);
CHECK_OFFSET(KSession, hasInited, 0xA8);
CHECK_SIZE(KSession, 0xB0);
