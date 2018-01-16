#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KServerPort.h"
#include "KClientPort.h"
#include "structutils.h"

struct KPort : public KAutoObject {
    KServerPort serverPort;
    KClientPort clientPort;
    UNKNOWN(0x8);
    bool hasInited;
    bool isLight; // ?
};
CHECK_OFFSET(KPort, serverPort, 0x10);
CHECK_OFFSET(KPort, clientPort, 0x60);
CHECK_OFFSET(KPort, hasInited, 0xA0);
CHECK_OFFSET(KPort, isLight, 0xA1);
CHECK_SIZE(KPort, 0xA8);
