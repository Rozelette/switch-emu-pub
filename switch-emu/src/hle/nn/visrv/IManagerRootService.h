#pragma once
#include "hle\service.h"

// Display Services
namespace nn {

namespace visrv {

namespace sf {

class IManagerRootService : public service::Interface {

    uint32_t GetDisplayService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetDisplayServiceWithProxyNameExchange);

public:
    IManagerRootService() {
        registerFunctions({
            { 2, static_cast<InterfaceFunc>(&IManagerRootService::GetDisplayService), "GetDisplayService" },
            { 3, static_cast<InterfaceFunc>(&IManagerRootService::GetDisplayServiceWithProxyNameExchange), "GetDisplayServiceWithProxyNameExchange" },
        });

        addService(this, "vi:m");
    }

    std::string getName() {
        return "nn::visrv::sf::IManagerRootService";
    }
};

} // namespace sf

} // namespace visrv

} // namespace nn