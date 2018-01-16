#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KLightServerSession.h"
#include "KLightClientSession.h"
#include "structutils.h"

struct KLightSession : public KAutoObject {
    KLightServerSession serverSide;
    KLightClientSession clientSide;
    bool hasInited;
};
CHECK_OFFSET(KLightSession, serverSide, 0x10);
CHECK_OFFSET(KLightSession, clientSide, 0x68);
CHECK_OFFSET(KLightSession, hasInited, 0xA0);
CHECK_SIZE(KLightSession, 0xA8);
