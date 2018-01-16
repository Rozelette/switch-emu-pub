#pragma once
#include "hle\service.h"

// APM services
namespace nn {

namespace apm {

class IManager : public service::Interface {

    uint32_t OpenSession(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetPerformanceMode);

public:
    IManager() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IManager::OpenSession), "OpenSession" },
            { 1, static_cast<InterfaceFunc>(&IManager::GetPerformanceMode), "GetPerformanceMode" },
        });

        addService(this, "apm");
    }

    std::string getName() {
        return "nn::apm::IManager";
    }
};

} // namespace apm

} // namespace nn
