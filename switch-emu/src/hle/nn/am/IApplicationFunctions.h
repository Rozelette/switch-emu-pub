#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class IApplicationFunctions : public ::service::Interface {

    uint32_t PopLaunchParameter(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(CreateApplicationAndPushAndRequestToStart);
    SERVICE_FUNC_STUB(CreateApplicationAndPushAndRequestToStartForQuest);
    SERVICE_FUNC_STUB(EnsureSaveData);
    uint32_t GetDesiredLanguage(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t SetTerminateResult(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetDisplayVersion);
    SERVICE_FUNC_STUB(GetLaunchStorageInfoForDebug);
    SERVICE_FUNC_STUB(ExtendSaveData);
    SERVICE_FUNC_STUB(GetSaveDataSize);
    SERVICE_FUNC_STUB(BeginBlockingHomeButtonShortAndLongPressed);
    SERVICE_FUNC_STUB(EndBlockingHomeButtonShortAndLongPressed);
    SERVICE_FUNC_STUB(BeginBlockingHomeButton);
    SERVICE_FUNC_STUB(EndBlockingHomeButton);
    uint32_t NotifyRunning(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetPseudoDeviceId);
    SERVICE_FUNC_STUB(SetMediaPlaybackStateForApplication);
    SERVICE_FUNC_STUB(IsGamePlayRecordingSupported);
    SERVICE_FUNC_STUB(InitializeGamePlayRecording);
    SERVICE_FUNC_STUB(SetGamePlayRecordingState);
    SERVICE_FUNC_STUB(RequestToShutdown);
    SERVICE_FUNC_STUB(RequestToReboot);

public:
    IApplicationFunctions() {
        registerFunctions({
            { 1, static_cast<InterfaceFunc>(&IApplicationFunctions::PopLaunchParameter), "PopLaunchParameter" },
            { 10, static_cast<InterfaceFunc>(&IApplicationFunctions::CreateApplicationAndPushAndRequestToStart), "CreateApplicationAndPushAndRequestToStart" },
            { 11, static_cast<InterfaceFunc>(&IApplicationFunctions::CreateApplicationAndPushAndRequestToStartForQuest), "CreateApplicationAndPushAndRequestToStartForQuest" },
            { 20, static_cast<InterfaceFunc>(&IApplicationFunctions::EnsureSaveData), "EnsureSaveData" },
            { 21, static_cast<InterfaceFunc>(&IApplicationFunctions::GetDesiredLanguage), "GetDesiredLanguage" },
            { 22, static_cast<InterfaceFunc>(&IApplicationFunctions::SetTerminateResult), "SetTerminateResult" },
            { 23, static_cast<InterfaceFunc>(&IApplicationFunctions::GetDisplayVersion), "GetDisplayVersion" },
            { 24, static_cast<InterfaceFunc>(&IApplicationFunctions::GetLaunchStorageInfoForDebug), "GetLaunchStorageInfoForDebug" },
            { 25, static_cast<InterfaceFunc>(&IApplicationFunctions::ExtendSaveData), "ExtendSaveData" },
            { 26, static_cast<InterfaceFunc>(&IApplicationFunctions::GetSaveDataSize), "GetSaveDataSize" },
            { 30, static_cast<InterfaceFunc>(&IApplicationFunctions::BeginBlockingHomeButtonShortAndLongPressed), "BeginBlockingHomeButtonShortAndLongPressed" },
            { 31, static_cast<InterfaceFunc>(&IApplicationFunctions::EndBlockingHomeButtonShortAndLongPressed), "EndBlockingHomeButtonShortAndLongPressed" },
            { 32, static_cast<InterfaceFunc>(&IApplicationFunctions::BeginBlockingHomeButton), "BeginBlockingHomeButton" },
            { 33, static_cast<InterfaceFunc>(&IApplicationFunctions::EndBlockingHomeButton), "EndBlockingHomeButton" },
            { 40, static_cast<InterfaceFunc>(&IApplicationFunctions::NotifyRunning), "NotifyRunning" },
            { 50, static_cast<InterfaceFunc>(&IApplicationFunctions::GetPseudoDeviceId), "GetPseudoDeviceId" },
            { 60, static_cast<InterfaceFunc>(&IApplicationFunctions::SetMediaPlaybackStateForApplication), "SetMediaPlaybackStateForApplication" },
            { 65, static_cast<InterfaceFunc>(&IApplicationFunctions::IsGamePlayRecordingSupported), "IsGamePlayRecordingSupported" },
            { 66, static_cast<InterfaceFunc>(&IApplicationFunctions::InitializeGamePlayRecording), "InitializeGamePlayRecording" },
            { 67, static_cast<InterfaceFunc>(&IApplicationFunctions::SetGamePlayRecordingState), "SetGamePlayRecordingState" },
            { 70, static_cast<InterfaceFunc>(&IApplicationFunctions::RequestToShutdown), "RequestToShutdown" },
            { 71, static_cast<InterfaceFunc>(&IApplicationFunctions::RequestToReboot), "RequestToReboot" },
        });
    }

    std::string getName() {
        return "nn::am::service::IApplicationFunctions";
    }
};

} // namespace service

} // namespace am

} // namespace nn
