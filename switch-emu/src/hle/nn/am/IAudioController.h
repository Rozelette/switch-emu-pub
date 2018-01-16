#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class IAudioController : public ::service::Interface {

    SERVICE_FUNC_STUB(SetExpectedMasterVolume);
    SERVICE_FUNC_STUB(GetMainAppletExpectedMasterVolume);
    SERVICE_FUNC_STUB(GetLibraryAppletExpectedMasterVolume);
    SERVICE_FUNC_STUB(ChangeMainAppletMasterVolume);
    SERVICE_FUNC_STUB(SetTransparentVolumeRate);

public:
    IAudioController() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IAudioController::SetExpectedMasterVolume), "SetExpectedMasterVolume" },
            { 1, static_cast<InterfaceFunc>(&IAudioController::GetMainAppletExpectedMasterVolume), "GetMainAppletExpectedMasterVolume" },
            { 2, static_cast<InterfaceFunc>(&IAudioController::GetLibraryAppletExpectedMasterVolume), "GetLibraryAppletExpectedMasterVolume" },
            { 3, static_cast<InterfaceFunc>(&IAudioController::ChangeMainAppletMasterVolume), "ChangeMainAppletMasterVolume" },
            { 4, static_cast<InterfaceFunc>(&IAudioController::SetTransparentVolumeRate), "SetTransparentVolumeRate" },
        });
    }

    std::string getName() {
        return "nn::am::service::IAudioController";
    }
};

} // namespace service

} // namespace am

} // namespace nn
