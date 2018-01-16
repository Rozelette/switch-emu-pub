#pragma once
#include <stdint.h>

#include "KHandleEntry.h"
#include "KSpinLock.h"
#include "structutils.h"

class KProcessHandleTable {

public:
    /*
    KProcessHandleTable();

    KProcessHandleTable(uint16_t size_, uint16_t maxUsageAtOnce_);

    handle_t createHandle(ObjectPtr<KAutoObject> object);

    template<typename T>
    handle_t createHandle(ObjectPtr<T> object) {
        return createHandle(kernel::object_pointer_cast<KAutoObject>(object));
    }

    ObjectPtr<KAutoObject> lookupObject(handle_t handle);

    bool closeHandle(handle_t handle);
    */

private:
    KHandleEntry* tablePointer;
    KHandleEntry* nextFreeEntry;
    KHandleEntry internelTable[1024];
    uint16_t size;
    uint16_t maxUsageAtOnce;
    uint16_t idCounter;
    uint16_t numActiveSlots;
    UNKNOWN(0x28);
    KSpinLock lock;
    UNKNOWN(0x40);
};
CHECK_SIZE(KProcessHandleTable, 0x20C0);
