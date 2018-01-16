#include <vector>
#include <optional>
#include <map>

#include "handleManager.h"
#include "log.h"

namespace kernel::handleman {

// TODO should use table in KProcess
std::vector<KHandleEntry> gHandleTable;
std::map<uint32_t, ObjectPtr<KAutoObject>> gCompressedPointerMap;

// TODO not this
handle_t gHandleCounter = 1; // TODO how are handles generator? Debug sdk asserts that the handle for the main thread < 0x40
uint32_t gCompressedpointerCounter = 0;

handle_t createHandle(ObjectPtr<KAutoObject> object) {
    handle_t handle = gHandleCounter++;
    uint32_t compressedPointer = gCompressedpointerCounter++;

    KHandleEntry newEntry;
    newEntry.handleId = static_cast<uint16_t>(handle);
    newEntry.objectCompressedPointer = compressedPointer;
    newEntry.objectType = object->getType();

    gHandleTable.push_back(newEntry);
    gCompressedPointerMap[compressedPointer] = object;

    return handle;
}

ObjectPtr<KAutoObject> lookupObject(handle_t handle) {
    for (const auto& entry : gHandleTable) {
        if (entry.handleId == static_cast<uint16_t>(handle)) {
            return gCompressedPointerMap[entry.objectCompressedPointer];
        }
    }

    return ObjectPtr<KAutoObject>();
}

bool closeHandle(handle_t handle) {
    for (auto i = 0u; i < gHandleTable.size(); i++) {
        if (gHandleTable[i].handleId == static_cast<uint16_t>(handle)) {
            logger::info("Closing handle %d", handle);
            gCompressedPointerMap.erase(gHandleTable[i].objectCompressedPointer);
            gHandleTable.erase(gHandleTable.begin() + i);
            return true;
        }
    }
    return false;
}

} // namespace kernel::handleman
