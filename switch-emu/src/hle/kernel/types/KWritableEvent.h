#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "structutils.h"

struct KEvent;

struct KWritableEvent : public KAutoObject {
    KEvent* parent;
};
CHECK_OFFSET(KWritableEvent, parent, 0x10);
CHECK_SIZE(KWritableEvent, 0x18);
