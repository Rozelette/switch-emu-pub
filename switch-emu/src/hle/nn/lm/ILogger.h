#pragma once
#include "hle\service.h"

namespace nn {

namespace lm {

class ILogger : public service::Interface {

    uint32_t unk0(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(unk1);

public:
    ILogger() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ILogger::unk0), "unk0" },
            { 1, static_cast<InterfaceFunc>(&ILogger::unk1), "unk1" },
        });
    }

    std::string getName() {
        return "nn::lm::ILogger";
    }
};

} // namespace lm

} // namespace nn
