#pragma once
#include "hle\service.h"

namespace nn {

namespace visrv {

namespace sf {

class ISystemDisplayService : public service::Interface {

    SERVICE_FUNC_STUB(GetZOrderCountMin);
    SERVICE_FUNC_STUB(GetZOrderCountMax);
    SERVICE_FUNC_STUB(GetDisplayLogicalResolution);
    SERVICE_FUNC_STUB(SetDisplayMagnification);
    SERVICE_FUNC_STUB(SetLayerPosition);
    SERVICE_FUNC_STUB(SetLayerSize);
    SERVICE_FUNC_STUB(GetLayerZ);
    uint32_t SetLayerZ(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(SetLayerVisibility);
    SERVICE_FUNC_STUB(SetLayerAlpha);
    SERVICE_FUNC_STUB(CreateStrayLayer);
    SERVICE_FUNC_STUB(OpenIndirectLayer);
    SERVICE_FUNC_STUB(CloseIndirectLayer);
    SERVICE_FUNC_STUB(FlipIndirectLayer);
    SERVICE_FUNC_STUB(ListDisplayModes);
    SERVICE_FUNC_STUB(ListDisplayRgbRanges);
    SERVICE_FUNC_STUB(ListDisplayContentTypes);
    SERVICE_FUNC_STUB(GetDisplayMode);
    SERVICE_FUNC_STUB(SetDisplayMode);
    SERVICE_FUNC_STUB(GetDisplayUnderscan);
    SERVICE_FUNC_STUB(SetDisplayUnderscan);
    SERVICE_FUNC_STUB(GetDisplayContentType);
    SERVICE_FUNC_STUB(SetDisplayContentType);
    SERVICE_FUNC_STUB(GetDisplayRgbRange);
    SERVICE_FUNC_STUB(SetDisplayRgbRange);
    SERVICE_FUNC_STUB(GetDisplayCmuMode);
    SERVICE_FUNC_STUB(SetDisplayCmuMode);
    SERVICE_FUNC_STUB(GetDisplayContrastRatio);
    SERVICE_FUNC_STUB(SetDisplayContrastRatio);
    SERVICE_FUNC_STUB(GetDisplayGamma);
    SERVICE_FUNC_STUB(SetDisplayGamma);
    SERVICE_FUNC_STUB(GetDisplayCmuLuma);
    SERVICE_FUNC_STUB(SetDisplayCmuLuma);

public:
    ISystemDisplayService() {
        registerFunctions({
            { 1200, static_cast<InterfaceFunc>(&ISystemDisplayService::GetZOrderCountMin), "GetZOrderCountMin" },
            { 1202, static_cast<InterfaceFunc>(&ISystemDisplayService::GetZOrderCountMax), "GetZOrderCountMax" },
            { 1203, static_cast<InterfaceFunc>(&ISystemDisplayService::GetDisplayLogicalResolution), "GetDisplayLogicalResolution" },
            { 1204, static_cast<InterfaceFunc>(&ISystemDisplayService::SetDisplayMagnification), "SetDisplayMagnification" },
            { 2201, static_cast<InterfaceFunc>(&ISystemDisplayService::SetLayerPosition), "SetLayerPosition" },
            { 2203, static_cast<InterfaceFunc>(&ISystemDisplayService::SetLayerSize), "SetLayerSize" },
            { 2204, static_cast<InterfaceFunc>(&ISystemDisplayService::GetLayerZ), "GetLayerZ" },
            { 2205, static_cast<InterfaceFunc>(&ISystemDisplayService::SetLayerZ), "SetLayerZ" },
            { 2207, static_cast<InterfaceFunc>(&ISystemDisplayService::SetLayerVisibility), "SetLayerVisibility" },
            { 2209, static_cast<InterfaceFunc>(&ISystemDisplayService::SetLayerAlpha), "SetLayerAlpha" },
            { 2312, static_cast<InterfaceFunc>(&ISystemDisplayService::CreateStrayLayer), "CreateStrayLayer" },
            { 2400, static_cast<InterfaceFunc>(&ISystemDisplayService::OpenIndirectLayer), "OpenIndirectLayer" },
            { 2401, static_cast<InterfaceFunc>(&ISystemDisplayService::CloseIndirectLayer), "CloseIndirectLayer" },
            { 2402, static_cast<InterfaceFunc>(&ISystemDisplayService::FlipIndirectLayer), "FlipIndirectLayer" },
            { 3000, static_cast<InterfaceFunc>(&ISystemDisplayService::ListDisplayModes), "ListDisplayModes" },
            { 3001, static_cast<InterfaceFunc>(&ISystemDisplayService::ListDisplayRgbRanges), "ListDisplayRgbRanges" },
            { 3002, static_cast<InterfaceFunc>(&ISystemDisplayService::ListDisplayContentTypes), "ListDisplayContentTypes" },
            { 3200, static_cast<InterfaceFunc>(&ISystemDisplayService::GetDisplayMode), "GetDisplayMode" },
            { 3201, static_cast<InterfaceFunc>(&ISystemDisplayService::SetDisplayMode), "SetDisplayMode" },
            { 3202, static_cast<InterfaceFunc>(&ISystemDisplayService::GetDisplayUnderscan), "GetDisplayUnderscan" },
            { 3203, static_cast<InterfaceFunc>(&ISystemDisplayService::SetDisplayUnderscan), "SetDisplayUnderscan" },
            { 3204, static_cast<InterfaceFunc>(&ISystemDisplayService::GetDisplayContentType), "GetDisplayContentType" },
            { 3205, static_cast<InterfaceFunc>(&ISystemDisplayService::SetDisplayContentType), "SetDisplayContentType" },
            { 3206, static_cast<InterfaceFunc>(&ISystemDisplayService::GetDisplayRgbRange), "GetDisplayRgbRange" },
            { 3207, static_cast<InterfaceFunc>(&ISystemDisplayService::SetDisplayRgbRange), "SetDisplayRgbRange" },
            { 3208, static_cast<InterfaceFunc>(&ISystemDisplayService::GetDisplayCmuMode), "GetDisplayCmuMode" },
            { 3209, static_cast<InterfaceFunc>(&ISystemDisplayService::SetDisplayCmuMode), "SetDisplayCmuMode" },
            { 3210, static_cast<InterfaceFunc>(&ISystemDisplayService::GetDisplayContrastRatio), "GetDisplayContrastRatio" },
            { 3211, static_cast<InterfaceFunc>(&ISystemDisplayService::SetDisplayContrastRatio), "SetDisplayContrastRatio" },
            { 3214, static_cast<InterfaceFunc>(&ISystemDisplayService::GetDisplayGamma), "GetDisplayGamma" },
            { 3215, static_cast<InterfaceFunc>(&ISystemDisplayService::SetDisplayGamma), "SetDisplayGamma" },
            { 3216, static_cast<InterfaceFunc>(&ISystemDisplayService::GetDisplayCmuLuma), "GetDisplayCmuLuma" },
            { 3217, static_cast<InterfaceFunc>(&ISystemDisplayService::SetDisplayCmuLuma), "SetDisplayCmuLuma" },
        });
    }

    std::string getName() {
        return "nn::visrv::sf::ISystemDisplayService";
    }
};

} // namespace sf

} // namespace visrv

} // namespace nn
