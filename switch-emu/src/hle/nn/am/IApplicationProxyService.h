#pragma once
#include "hle\service.h"

// Applet services
namespace nn {

namespace am {

namespace service {

class IApplicationProxyService : public ::service::Interface {

    uint32_t OpenApplicationProxy(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);

public:
    IApplicationProxyService() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IApplicationProxyService::OpenApplicationProxy), "OpenApplicationProxy" },
        });

        addService(this, "appletOE");
    }

    std::string getName() {
        return "nn::am::service::IApplicationProxyService";
    }
};

} // namespace service

} // namespace am

} // namespace nn
