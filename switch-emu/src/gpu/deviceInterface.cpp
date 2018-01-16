#include "deviceInterface.h"
#include "log.h"

namespace gpu {

void DeviceInterface::registerFunctions(const std::vector<FunctionEntry>& funcs) {
    mFunctions = funcs;
}

uint32_t DeviceInterface::handleIoctl(uint32_t cmd, const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    uint32_t id = cmd & 0xFF; // TODO check other fields
    for (auto& func : mFunctions) {
        if ((func.id == id) && (func.func != nullptr)) {
            logger::info("Program called %s.%s", getPath().c_str(), func.name);
            return (this->*func.func)(in, out);
        }
    }

    logger::error("Unknown/unimplemented command called for %s: %08X", getPath().c_str(), cmd);
    return ERR_NOT_IMPLEMENTED;
}

} // namespace gpu