#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class IStorage : public ::service::Interface {

    uint32_t Open(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(OpenTransferStorage);

public:
    IStorage() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IStorage::Open), "Open" },
            { 1, static_cast<InterfaceFunc>(&IStorage::OpenTransferStorage), "OpenTransferStorage" },
        });
    }

    std::string getName() {
        return "nn::am::service::IStorage";
    }
};

} // namespace service

} // namespace am

} // namespace nn
