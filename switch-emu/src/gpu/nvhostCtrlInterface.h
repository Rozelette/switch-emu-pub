#pragma once
#include "deviceInterface.h"

namespace gpu {

class NvhostCtrlInterface : public DeviceInterface {

    uint32_t GetConfig(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);

public:
    NvhostCtrlInterface() {
        registerFunctions({
            { 27, static_cast<InterfaceFunc>(&NvhostCtrlInterface::GetConfig), "GetConfig" },
        });
    }

    std::string getPath() {
        return "/dev/nvhost-ctrl";
    }

private:
    // TODO own .h?
    struct GetConfigArgs {
        char domainStr[0x41];
        char paramStr[0x41];
        char configStr[0x101];
    };
};

} // namespace gpu