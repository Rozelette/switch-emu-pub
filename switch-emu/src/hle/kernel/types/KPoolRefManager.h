#pragma once
#include <stdint.h>

#include "structutils.h"

struct KPoolManager;

struct KPoolRefManager {
    KPoolManager* parent;
    void* poolBase;
    uint64_t maxPage;
    uint16_t* refCountTable;
};
CHECK_OFFSET(KPoolRefManager, parent, 0x0);
CHECK_OFFSET(KPoolRefManager, poolBase, 0x8);
CHECK_OFFSET(KPoolRefManager, maxPage, 0x10);
CHECK_OFFSET(KPoolRefManager, refCountTable, 0x18);
CHECK_SIZE(KPoolRefManager, 0x20);
