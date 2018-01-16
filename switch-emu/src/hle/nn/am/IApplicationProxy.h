#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class IApplicationProxy : public ::service::Interface {

    uint32_t GetCommonStateGetter(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetSelfController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetWindowController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetAudioController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetDisplayController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetProcessWindingController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetLibraryAppletCreator(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetApplicationFunctions(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetDebugFunctions(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);

public:
    IApplicationProxy() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IApplicationProxy::GetCommonStateGetter), "GetCommonStateGetter" },
            { 1, static_cast<InterfaceFunc>(&IApplicationProxy::GetSelfController) , "GetSelfController" },
            { 2, static_cast<InterfaceFunc>(&IApplicationProxy::GetWindowController) , "GetWindowController" },
            { 3, static_cast<InterfaceFunc>(&IApplicationProxy::GetAudioController) , "GetAudioController" },
            { 4, static_cast<InterfaceFunc>(&IApplicationProxy::GetDisplayController) , "GetDisplayController" },
            { 10, static_cast<InterfaceFunc>(&IApplicationProxy::GetProcessWindingController) , "GetProcessWindingController" },
            { 11, static_cast<InterfaceFunc>(&IApplicationProxy::GetLibraryAppletCreator) , "GetLibraryAppletCreator" },
            { 20, static_cast<InterfaceFunc>(&IApplicationProxy::GetApplicationFunctions) , "GetApplicationFunctions" },
            { 1000, static_cast<InterfaceFunc>(&IApplicationProxy::GetDebugFunctions) , "GetDebugFunctions" },
        });
    }

    std::string getName() {
        return "nn::am::service::IApplicationProxy";
    }
};

} // namespace service

} // namespace am

} // namespace nn
