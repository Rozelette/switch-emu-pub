#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KMutex.h"
#include "structutils.h"

struct KResourceLimit : public KAutoObject {
    uint64_t currentValue[5];
    uint64_t limitValue[5];
    KMutex mutex;
};
CHECK_OFFSET(KResourceLimit, currentValue, 0x10);
CHECK_OFFSET(KResourceLimit, limitValue, 0x38);
CHECK_OFFSET(KResourceLimit, mutex, 0x60);
CHECK_SIZE(KResourceLimit, 0x68);
