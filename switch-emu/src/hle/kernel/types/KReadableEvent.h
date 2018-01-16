#pragma once
#include <stdint.h>

#include "KSynchronizationObject.h"
#include "structutils.h"

struct KEvent;

struct KReadableEvent : public KSynchronizationObject {
    bool unk;
    KEvent* parent;
};
CHECK_OFFSET(KReadableEvent, unk, 0x28);
CHECK_OFFSET(KReadableEvent, parent, 0x30);
CHECK_SIZE(KReadableEvent, 0x38);
