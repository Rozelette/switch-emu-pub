#pragma once
#include <stdint.h>

#include "structutils.h"

struct KIrqReciever;

struct KIrqEntry {
    KIrqReciever* receiver;
    uint8_t state0;
    uint8_t state1;
};
CHECK_OFFSET(KIrqEntry, receiver, 0x0);
CHECK_OFFSET(KIrqEntry, state0, 0x8);
CHECK_OFFSET(KIrqEntry, state1, 0x9);
CHECK_SIZE(KIrqEntry, 0x10);
