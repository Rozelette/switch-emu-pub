#pragma once
#include "hle\service.h"

// Applet services
namespace nn {

namespace am {

namespace service {

class IAllSystemAppletProxiesService : public ::service::Interface {

    SERVICE_FUNC_STUB(OpenSystemAppletProxy);
    uint32_t OpenLibraryAppletProxyOld(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(OpenLibraryAppletProxy);
    SERVICE_FUNC_STUB(OpenOverlayAppletProxy);
    SERVICE_FUNC_STUB(OpenSystemApplicationProxy);
    SERVICE_FUNC_STUB(CreateSelfLibraryAppletCreatorForDevelop);

public:
    IAllSystemAppletProxiesService() {
        registerFunctions({
            { 100, static_cast<InterfaceFunc>(&IAllSystemAppletProxiesService::OpenSystemAppletProxy), "OpenSystemAppletProxy" },
            { 200, static_cast<InterfaceFunc>(&IAllSystemAppletProxiesService::OpenLibraryAppletProxyOld), "OpenLibraryAppletProxyOld" },
            { 201, static_cast<InterfaceFunc>(&IAllSystemAppletProxiesService::OpenLibraryAppletProxy), "OpenLibraryAppletProxy" },
            { 300, static_cast<InterfaceFunc>(&IAllSystemAppletProxiesService::OpenOverlayAppletProxy), "OpenOverlayAppletProxy" },
            { 350, static_cast<InterfaceFunc>(&IAllSystemAppletProxiesService::OpenSystemApplicationProxy), "OpenSystemApplicationProxy" },
            { 400, static_cast<InterfaceFunc>(&IAllSystemAppletProxiesService::CreateSelfLibraryAppletCreatorForDevelop), "CreateSelfLibraryAppletCreatorForDevelop" },
         });

        addService(this, "appletAE");
    }

    std::string getName() {
        return "nn::am::service::IAllSystemAppletProxiesService";
    }
};

} // namespace service

} // namespace am

} // namespace nn
