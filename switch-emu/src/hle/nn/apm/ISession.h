#pragma once
#include "hle\service.h"

namespace nn {

namespace apm {

class ISession : public service::Interface {

    uint32_t SetPerformanceConfiguration(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetPerformanceConfiguration);

public:
    ISession() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ISession::SetPerformanceConfiguration), "SetPerformanceConfiguration" },
            { 0, static_cast<InterfaceFunc>(&ISession::GetPerformanceConfiguration), "GetPerformanceConfiguration" },
        });
    }

    std::string getName() {
        return "nn::apm::ISession";
    }
};

} // namespace apm

} // namespace nn
