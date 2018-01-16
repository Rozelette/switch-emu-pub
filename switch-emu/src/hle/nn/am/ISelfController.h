#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class ISelfController : public ::service::Interface {

    SERVICE_FUNC_STUB(Exit);
    SERVICE_FUNC_STUB(LockExit);
    SERVICE_FUNC_STUB(UnlockExit);
    SERVICE_FUNC_STUB(EnterFatalSection);
    SERVICE_FUNC_STUB(LeaveFatalSection);
    SERVICE_FUNC_STUB(GetLibraryAppletLaunchableEvent);
    SERVICE_FUNC_STUB(SetScreenShotPermission);
    uint32_t SetOperationModeChangedNotification(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t SetPerformanceModeChangedNotification(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t SetFocusHandlingMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t SetRestartMessageEnabled(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t SetScreenShotAppletIdentityInfo(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t SetOutOfFocusSuspendingEnabled(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(SetControllerFirmwareUpdateSection);
    SERVICE_FUNC_STUB(SetRequiresCaptureButtonShortPressedMessage);
    SERVICE_FUNC_STUB(SetScreenShotImageOrientation);
    uint32_t CreateManagedDisplayLayer(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(SetHandlesRequestToDisplay);
    SERVICE_FUNC_STUB(ApproveToDisplay);
    SERVICE_FUNC_STUB(OverrideAutoSleepTimeAndDimmingTime);
    SERVICE_FUNC_STUB(SetMediaPlaybackState);
    SERVICE_FUNC_STUB(SetIdleTimeDetectionExtension);
    SERVICE_FUNC_STUB(GetIdleTimeDetectionExtension);
    SERVICE_FUNC_STUB(SetInputDetectionSourceSet);
    SERVICE_FUNC_STUB(ReportUserIsActive);
    SERVICE_FUNC_STUB(GetCurrentIlluminance);
    SERVICE_FUNC_STUB(IsIlluminanceAvailable);

public:
    ISelfController() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ISelfController::Exit), "Exit" },
            { 1, static_cast<InterfaceFunc>(&ISelfController::LockExit), "LockExit" },
            { 2, static_cast<InterfaceFunc>(&ISelfController::UnlockExit), "UnlockExit" },
            { 3, static_cast<InterfaceFunc>(&ISelfController::EnterFatalSection), "EnterFatalSection" },
            { 4, static_cast<InterfaceFunc>(&ISelfController::LeaveFatalSection), "LeaveFatalSection" },
            { 9, static_cast<InterfaceFunc>(&ISelfController::GetLibraryAppletLaunchableEvent), "GetLibraryAppletLaunchableEvent" },
            { 10, static_cast<InterfaceFunc>(&ISelfController::SetScreenShotPermission), "SetScreenShotPermission" },
            { 11, static_cast<InterfaceFunc>(&ISelfController::SetOperationModeChangedNotification), "SetOperationModeChangedNotification" },
            { 12, static_cast<InterfaceFunc>(&ISelfController::SetPerformanceModeChangedNotification), "SetPerformanceModeChangedNotification" },
            { 13, static_cast<InterfaceFunc>(&ISelfController::SetFocusHandlingMode), "SetFocusHandlingMode" },
            { 14, static_cast<InterfaceFunc>(&ISelfController::SetRestartMessageEnabled), "SetRestartMessageEnabled" },
            { 15, static_cast<InterfaceFunc>(&ISelfController::SetScreenShotAppletIdentityInfo), "SetScreenShotAppletIdentityInfo" },
            { 16, static_cast<InterfaceFunc>(&ISelfController::SetOutOfFocusSuspendingEnabled), "SetOutOfFocusSuspendingEnabled" },
            { 17, static_cast<InterfaceFunc>(&ISelfController::SetControllerFirmwareUpdateSection), "SetControllerFirmwareUpdateSection" },
            { 18, static_cast<InterfaceFunc>(&ISelfController::SetRequiresCaptureButtonShortPressedMessage), "SetRequiresCaptureButtonShortPressedMessage" },
            { 19, static_cast<InterfaceFunc>(&ISelfController::SetScreenShotImageOrientation), "SetScreenShotImageOrientation" },
            { 40, static_cast<InterfaceFunc>(&ISelfController::CreateManagedDisplayLayer), "CreateManagedDisplayLayer" },
            { 50, static_cast<InterfaceFunc>(&ISelfController::SetHandlesRequestToDisplay), "SetHandlesRequestToDisplay" },
            { 51, static_cast<InterfaceFunc>(&ISelfController::ApproveToDisplay), "ApproveToDisplay" },
            { 60, static_cast<InterfaceFunc>(&ISelfController::OverrideAutoSleepTimeAndDimmingTime), "OverrideAutoSleepTimeAndDimmingTime" },
            { 61, static_cast<InterfaceFunc>(&ISelfController::SetMediaPlaybackState), "SetMediaPlaybackState" },
            { 62, static_cast<InterfaceFunc>(&ISelfController::SetIdleTimeDetectionExtension), "SetIdleTimeDetectionExtension" },
            { 63, static_cast<InterfaceFunc>(&ISelfController::GetIdleTimeDetectionExtension), "GetIdleTimeDetectionExtension" },
            { 64, static_cast<InterfaceFunc>(&ISelfController::SetInputDetectionSourceSet), "SetInputDetectionSourceSet" },
            { 65, static_cast<InterfaceFunc>(&ISelfController::ReportUserIsActive), "ReportUserIsActive" },
            { 66, static_cast<InterfaceFunc>(&ISelfController::GetCurrentIlluminance), "GetCurrentIlluminance" },
            { 67, static_cast<InterfaceFunc>(&ISelfController::IsIlluminanceAvailable), "IsIlluminanceAvailable" },
        });
    }

    std::string getName() {
        return "nn::am::service::ISelfController";
    }
};

} // namespace service

} // namespace am

} // namespace nn
