#pragma once
#include "hle\service.h"

namespace nn {

namespace visrv {

namespace sf {

class IManagerDisplayService : public service::Interface {

    SERVICE_FUNC_STUB(GetDisplayResolution);
    uint32_t CreateManagedLayer(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(DestroyManagedLayer);
    SERVICE_FUNC_STUB(CreateIndirectLayer);
    SERVICE_FUNC_STUB(DestroyIndirectLayer);
    SERVICE_FUNC_STUB(CreateIndirectProducerEndPoint);
    SERVICE_FUNC_STUB(DestroyIndirectProducerEndPoint);
    SERVICE_FUNC_STUB(CreateIndirectConsumerEndPoint);
    SERVICE_FUNC_STUB(DestroyIndirectConsumerEndPoint);
    SERVICE_FUNC_STUB(AcquireLayerTexturePresentingEvent);
    SERVICE_FUNC_STUB(ReleaseLayerTexturePresentingEvent);
    SERVICE_FUNC_STUB(GetDisplayHotplugEvent);
    SERVICE_FUNC_STUB(GetDisplayHotplugState);
    SERVICE_FUNC_STUB(SetDisplayAlpha);
    SERVICE_FUNC_STUB(SetDisplayLayerStack);
    SERVICE_FUNC_STUB(SetDisplayPowerState);
    uint32_t AddToLayerStack(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(RemoveFromLayerStack);
    SERVICE_FUNC_STUB(SetLayerVisibility);
    SERVICE_FUNC_STUB(SetContentVisibility);
    SERVICE_FUNC_STUB(SetConductorLayer);
    SERVICE_FUNC_STUB(SetIndirectProducerFlipOffset);

public:
    IManagerDisplayService() {
        registerFunctions({
            { 1102, static_cast<InterfaceFunc>(&IManagerDisplayService::GetDisplayResolution), "GetDisplayResolution" },
            { 2010, static_cast<InterfaceFunc>(&IManagerDisplayService::CreateManagedLayer), "CreateManagedLayer" },
            { 2011, static_cast<InterfaceFunc>(&IManagerDisplayService::DestroyManagedLayer), "DestroyManagedLayer" },
            { 2050, static_cast<InterfaceFunc>(&IManagerDisplayService::CreateIndirectLayer), "CreateIndirectLayer" },
            { 2051, static_cast<InterfaceFunc>(&IManagerDisplayService::DestroyIndirectLayer), "DestroyIndirectLayer" },
            { 2052, static_cast<InterfaceFunc>(&IManagerDisplayService::CreateIndirectProducerEndPoint), "CreateIndirectProducerEndPoint" },
            { 2053, static_cast<InterfaceFunc>(&IManagerDisplayService::DestroyIndirectProducerEndPoint), "DestroyIndirectProducerEndPoint" },
            { 2054, static_cast<InterfaceFunc>(&IManagerDisplayService::CreateIndirectConsumerEndPoint), "CreateIndirectConsumerEndPoint" },
            { 2055, static_cast<InterfaceFunc>(&IManagerDisplayService::DestroyIndirectConsumerEndPoint), "DestroyIndirectConsumerEndPoint" },
            { 2300, static_cast<InterfaceFunc>(&IManagerDisplayService::AcquireLayerTexturePresentingEvent), "AcquireLayerTexturePresentingEvent" },
            { 2301, static_cast<InterfaceFunc>(&IManagerDisplayService::ReleaseLayerTexturePresentingEvent), "ReleaseLayerTexturePresentingEvent" },
            { 2302, static_cast<InterfaceFunc>(&IManagerDisplayService::GetDisplayHotplugEvent), "GetDisplayHotplugEvent" },
            { 2402, static_cast<InterfaceFunc>(&IManagerDisplayService::GetDisplayHotplugState), "GetDisplayHotplugState" },
            { 4201, static_cast<InterfaceFunc>(&IManagerDisplayService::SetDisplayAlpha), "SetDisplayAlpha" },
            { 4203, static_cast<InterfaceFunc>(&IManagerDisplayService::SetDisplayLayerStack), "SetDisplayLayerStack" },
            { 4205, static_cast<InterfaceFunc>(&IManagerDisplayService::SetDisplayPowerState), "SetDisplayPowerState" },
            { 6000, static_cast<InterfaceFunc>(&IManagerDisplayService::AddToLayerStack), "AddToLayerStack" },
            { 6001, static_cast<InterfaceFunc>(&IManagerDisplayService::RemoveFromLayerStack), "RemoveFromLayerStack" },
            { 6002, static_cast<InterfaceFunc>(&IManagerDisplayService::SetLayerVisibility), "SetLayerVisibility" },
            { 7000, static_cast<InterfaceFunc>(&IManagerDisplayService::SetContentVisibility), "SetContentVisibility" },
            { 8000, static_cast<InterfaceFunc>(&IManagerDisplayService::SetConductorLayer), "SetConductorLayer" },
            { 8100, static_cast<InterfaceFunc>(&IManagerDisplayService::SetIndirectProducerFlipOffset), "SetIndirectProducerFlipOffset" },
        });
    }

    std::string getName() {
        return "nn::visrv::sf::IManagerDisplayService";
    }
};

} // namespace sf

} // namespace visrv

} // namespace nn
