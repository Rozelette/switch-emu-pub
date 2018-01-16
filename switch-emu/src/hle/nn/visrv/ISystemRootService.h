#pragma once
#include "hle\service.h"

// Display Services
namespace nn {

namespace visrv {

namespace sf {

class ISystemRootService : public service::Interface {

    uint32_t GetDisplayService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetDisplayServiceWithProxyNameExchange);

public:
    ISystemRootService() {
        registerFunctions({
            { 1, static_cast<InterfaceFunc>(&ISystemRootService::GetDisplayService), "GetDisplayService" },
            { 3, static_cast<InterfaceFunc>(&ISystemRootService::GetDisplayServiceWithProxyNameExchange), "GetDisplayServiceWithProxyNameExchange" },
         });

        addService(this, "vi:s");
    }

    std::string getName() {
        return "nn::visrv::sf::ISystemRootService";
    }
};

} // namespace sf

} // namespace visrv

} // namespace nn