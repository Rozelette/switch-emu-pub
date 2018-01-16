#pragma once
#include <stdint.h>

#include "KReadableEvent.h"
#include "structutils.h"

struct KInterruptReceiver;

struct KInterruptEvent : public KReadableEvent {
    KInterruptReceiver* receiver;
    uint32_t irqId;
    bool isRegistered;
};
CHECK_OFFSET(KInterruptEvent, receiver, 0x38);
CHECK_OFFSET(KInterruptEvent, irqId, 0x40);
CHECK_OFFSET(KInterruptEvent, isRegistered, 0x44);
CHECK_SIZE(KInterruptEvent, 0x48);
