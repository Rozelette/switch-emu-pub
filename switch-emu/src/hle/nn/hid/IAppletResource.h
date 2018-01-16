#pragma once
#include "hle\service.h"

namespace nn {

namespace hid {

class IAppletResource : public service::Interface {

    uint32_t GetSharedMemoryHandle(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);

public:
    IAppletResource() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IAppletResource::GetSharedMemoryHandle), "GetSharedMemoryHandle" },
        });
    }

    std::string getName() {
        return "nn::hid::IAppletResource";
    }
};

} // namespace hid

} // namespace nn
