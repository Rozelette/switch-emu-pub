#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KMutex.h"
#include "KSmmuManager.h"
#include "structutils.h"

struct KDeviceAddressSpace : public KAutoObject {
    KMutex mutex;
    KSmmuManager manager;
    uint64_t baseAddress;
    uint64_t size;
    bool hasInited;
};
CHECK_OFFSET(KDeviceAddressSpace, mutex, 0x10);
CHECK_OFFSET(KDeviceAddressSpace, manager, 0x18);
CHECK_OFFSET(KDeviceAddressSpace, baseAddress, 0x58);
CHECK_OFFSET(KDeviceAddressSpace, size, 0x60);
CHECK_OFFSET(KDeviceAddressSpace, hasInited, 0x68);
CHECK_SIZE(KDeviceAddressSpace, 0x70);

