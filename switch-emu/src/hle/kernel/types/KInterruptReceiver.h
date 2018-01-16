#pragma once
#include <stdint.h>

#include "structutils.h"

struct KInterruptEvent;

struct KInterruptReceiver {
    uint64_t initiallyZero; // ???
    KInterruptEvent* eventPointer;
    int32_t irqId;

    virtual void dummy() {} // force creation of vtable
};
CHECK_OFFSET(KInterruptReceiver, initiallyZero, 0x8);
CHECK_OFFSET(KInterruptReceiver, eventPointer, 0x10);
CHECK_OFFSET(KInterruptReceiver, irqId, 0x18);
CHECK_SIZE(KInterruptReceiver, 0x20);
