#pragma once
#include <stdint.h>

#include "structutils.h"

class KProcess;

struct KTlsPageManager {
    uint64_t tlsPagePtr;
    KProcess* process;
    uint32_t usage;
    bool isSlotFree[8];
};
CHECK_OFFSET(KTlsPageManager, tlsPagePtr, 0x0);
CHECK_OFFSET(KTlsPageManager, process, 0x8);
CHECK_OFFSET(KTlsPageManager, usage, 0x10);
CHECK_OFFSET(KTlsPageManager, isSlotFree, 0x14);
CHECK_SIZE(KTlsPageManager, 0x20);
