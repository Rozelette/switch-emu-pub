#pragma once
#include <stdint.h>

#include "KIrqEntry.h"
#include "KMutex.h"
#include "structutils.h"

struct KIrqManager {
    KIrqEntry core0Irq[0x20];
    KIrqEntry core1Irq[0x20];
    KIrqEntry core2Irq[0x20];
    KIrqEntry core3Irq[0x20];
    KIrqEntry sharedIrqs[0xE0];
    KMutex mutex;
};
CHECK_OFFSET(KIrqManager, core0Irq, 0x0);
CHECK_OFFSET(KIrqManager, core1Irq, 0x200);
CHECK_OFFSET(KIrqManager, core2Irq, 0x400);
CHECK_OFFSET(KIrqManager, core3Irq, 0x600);
CHECK_OFFSET(KIrqManager, sharedIrqs, 0x800);
CHECK_OFFSET(KIrqManager, mutex, 0x1600);
CHECK_SIZE(KIrqManager, 0x1608);
