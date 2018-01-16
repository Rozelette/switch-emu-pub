#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KReadableEvent.h"
#include "KWritableEvent.h"
#include "structutils.h"

class KProcess;

struct KEvent : public KAutoObject {
    KReadableEvent readableEvent;
    KWritableEvent writableEvent;
    KProcess* creator;
    bool hasInited;
};
CHECK_OFFSET(KEvent, readableEvent, 0x10);
CHECK_OFFSET(KEvent, writableEvent, 0x48);
CHECK_OFFSET(KEvent, creator, 0x60);
CHECK_OFFSET(KEvent, hasInited, 0x68);
CHECK_SIZE(KEvent, 0x70);