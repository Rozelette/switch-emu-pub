#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class IStorageAccessor : public ::service::Interface {

    uint32_t GetSize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(Write);
    uint32_t Read(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);

public:
    IStorageAccessor() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IStorageAccessor::GetSize), "GetSize" },
            { 10, static_cast<InterfaceFunc>(&IStorageAccessor::Write), "Write" },
            { 11, static_cast<InterfaceFunc>(&IStorageAccessor::Read), "Read" },
        });
    }

    std::string getName() {
        return "nn::am::service::IStorageAccessor";
    }
};

} // namespace service

} // namespace am

} // namespace nn
