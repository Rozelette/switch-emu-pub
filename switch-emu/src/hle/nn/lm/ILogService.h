#pragma once
#include "hle\service.h"

// Log Manager
namespace nn {

namespace lm {

class ILogService : public service::Interface {
    uint32_t Initialize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);

public:
    ILogService() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ILogService::Initialize), "Initialize" },
        });

        addService(this, "lm");
    }

    std::string getName() {
        return "nn::lm::ILogService";
    }
};

} // namespace lm

} // namespace nn