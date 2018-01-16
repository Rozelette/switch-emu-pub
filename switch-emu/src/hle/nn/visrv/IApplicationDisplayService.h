#pragma once
#include <map>

#include "hle\service.h"
#include "hle\nn\vi\Display.h"

namespace nn {

namespace visrv {

namespace sf {

class IApplicationDisplayService : public service::Interface {

    uint32_t GetRelayService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetSystemDisplayService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetManagerDisplayService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetIndirectDisplayTransactionService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(ListDisplays);
    uint32_t OpenDisplay(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(OpenDefaultDisplay);
    SERVICE_FUNC_STUB(CloseDisplay);
    SERVICE_FUNC_STUB(SetDisplayEnabled);
    SERVICE_FUNC_STUB(GetDisplayResolution);
    uint32_t OpenLayer(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(CloseLayer);
    uint32_t CreateStrayLayer(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(DestroyStrayLayer);
    uint32_t SetLayerScalingMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetIndirectLayerImageMap);
    SERVICE_FUNC_STUB(GetIndirectLayerImageCropMap);
    SERVICE_FUNC_STUB(GetIndirectLayerImageRequiredMemoryInfo);
    uint32_t GetDisplayVsyncEvent(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetDisplayVsyncEventForDebug);

public:
    IApplicationDisplayService() {
        registerFunctions({
            { 100, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetRelayService), "GetRelayService" },
            { 101, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetSystemDisplayService), "GetSystemDisplayService" },
            { 102, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetManagerDisplayService), "GetManagerDisplayService" },
            { 103, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetIndirectDisplayTransactionService), "GetIndirectDisplayTransactionService" },
            { 1000, static_cast<InterfaceFunc>(&IApplicationDisplayService::ListDisplays), "ListDisplays" },
            { 1010, static_cast<InterfaceFunc>(&IApplicationDisplayService::OpenDisplay), "OpenDisplay" },
            { 1011, static_cast<InterfaceFunc>(&IApplicationDisplayService::OpenDefaultDisplay), "OpenDefaultDisplay" },
            { 1020, static_cast<InterfaceFunc>(&IApplicationDisplayService::CloseDisplay), "CloseDisplay" },
            { 1101, static_cast<InterfaceFunc>(&IApplicationDisplayService::SetDisplayEnabled), "SetDisplayEnabled" },
            { 1102, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetDisplayResolution), "GetDisplayResolution" },
            { 2020, static_cast<InterfaceFunc>(&IApplicationDisplayService::OpenLayer), "OpenLayer" },
            { 2021, static_cast<InterfaceFunc>(&IApplicationDisplayService::CloseLayer), "CloseLayer" },
            { 2030, static_cast<InterfaceFunc>(&IApplicationDisplayService::CreateStrayLayer), "CreateStrayLayer" },
            { 2031, static_cast<InterfaceFunc>(&IApplicationDisplayService::DestroyStrayLayer), "DestroyStrayLayer" },
            { 2101, static_cast<InterfaceFunc>(&IApplicationDisplayService::SetLayerScalingMode), "SetLayerScalingMode" },
            { 2450, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetIndirectLayerImageMap), "GetIndirectLayerImageMap" },
            { 2451, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetIndirectLayerImageCropMap), "GetIndirectLayerImageCropMap" },
            { 2460, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetIndirectLayerImageRequiredMemoryInfo), "GetIndirectLayerImageRequiredMemoryInfo" },
            { 5202, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetDisplayVsyncEvent), "GetDisplayVsyncEvent" },
            { 5203, static_cast<InterfaceFunc>(&IApplicationDisplayService::GetDisplayVsyncEventForDebug), "GetDisplayVsyncEventForDebug" },
        });
    }

    std::string getName() {
        return "nn::visrv::sf::IApplicationDisplayService";
    }
};

} // namespace sf

} // namespace visrv

} // namespace nn
