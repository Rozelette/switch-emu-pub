#pragma once
#include "hle\service.h"

// Applet services
namespace nn {

namespace am {

namespace service {

class ILibraryAppletProxy : public ::service::Interface {

    SERVICE_FUNC_STUB(GetCommonStateGetter);
    uint32_t GetSelfController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetWindowController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetAudioController);
    SERVICE_FUNC_STUB(GetDisplayController);
    SERVICE_FUNC_STUB(GetProcessWindingController);
    SERVICE_FUNC_STUB(GetLibraryAppletCreator);
    SERVICE_FUNC_STUB(OpenLibraryAppletSelfAccessor);
    SERVICE_FUNC_STUB(GetDebugFunctions);

public:
    ILibraryAppletProxy() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ILibraryAppletProxy::GetCommonStateGetter), "GetCommonStateGetter" },
            { 1, static_cast<InterfaceFunc>(&ILibraryAppletProxy::GetSelfController), "GetSelfController" },
            { 2, static_cast<InterfaceFunc>(&ILibraryAppletProxy::GetWindowController), "GetWindowController" },
            { 3, static_cast<InterfaceFunc>(&ILibraryAppletProxy::GetAudioController), "GetAudioController" },
            { 4, static_cast<InterfaceFunc>(&ILibraryAppletProxy::GetDisplayController), "GetDisplayController" },
            { 10, static_cast<InterfaceFunc>(&ILibraryAppletProxy::GetProcessWindingController), "GetProcessWindingController" },
            { 11, static_cast<InterfaceFunc>(&ILibraryAppletProxy::GetLibraryAppletCreator), "GetLibraryAppletCreator" },
            { 20, static_cast<InterfaceFunc>(&ILibraryAppletProxy::OpenLibraryAppletSelfAccessor), "OpenLibraryAppletSelfAccessor" },
            { 1000, static_cast<InterfaceFunc>(&ILibraryAppletProxy::GetDebugFunctions), "GetDebugFunctions" },
         });
    }

    std::string getName() {
        return "nn::am::service::ILibraryAppletProxy";
    }
};

} // namespace service

} // namespace am

} // namespace nn
