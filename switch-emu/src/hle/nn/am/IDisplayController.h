#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class IDisplayController : public ::service::Interface {

    SERVICE_FUNC_STUB(GetLastForegroundCaptureImage);
    SERVICE_FUNC_STUB(UpdateLastForegroundCaptureImage);
    SERVICE_FUNC_STUB(GetLastApplicationCaptureImage);
    SERVICE_FUNC_STUB(GetCallerAppletCaptureImage);
    SERVICE_FUNC_STUB(UpdateCallerAppletCaptureImage);
    SERVICE_FUNC_STUB(GetLastForegroundCaptureImageEx);
    SERVICE_FUNC_STUB(GetLastApplicationCaptureImageEx);
    SERVICE_FUNC_STUB(GetCallerAppletCaptureImageEx);
    SERVICE_FUNC_STUB(TakeScreenShotOfOwnLayer);
    SERVICE_FUNC_STUB(AcquireLastApplicationCaptureBuffer);
    SERVICE_FUNC_STUB(ReleaseLastApplicationCaptureBuffer);
    SERVICE_FUNC_STUB(AcquireLastForegroundCaptureBuffer);
    SERVICE_FUNC_STUB(ReleaseLastForegroundCaptureBuffer);
    SERVICE_FUNC_STUB(AcquireCallerAppletCaptureBuffer);
    SERVICE_FUNC_STUB(ReleaseCallerAppletCaptureBuffer);
    SERVICE_FUNC_STUB(AcquireLastApplicationCaptureBufferEx);
    SERVICE_FUNC_STUB(AcquireLastForegroundCaptureBufferEx);
    SERVICE_FUNC_STUB(AcquireCallerAppletCaptureBufferEx);
    SERVICE_FUNC_STUB(ClearCaptureBuffer);
    SERVICE_FUNC_STUB(ClearAppletTransitionBuffer);

public:
    IDisplayController() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IDisplayController::GetLastForegroundCaptureImage), "GetLastForegroundCaptureImage" },
            { 1, static_cast<InterfaceFunc>(&IDisplayController::UpdateLastForegroundCaptureImage), "UpdateLastForegroundCaptureImage" },
            { 2, static_cast<InterfaceFunc>(&IDisplayController::GetLastApplicationCaptureImage), "GetLastApplicationCaptureImage" },
            { 3, static_cast<InterfaceFunc>(&IDisplayController::GetCallerAppletCaptureImage), "GetCallerAppletCaptureImage" },
            { 4, static_cast<InterfaceFunc>(&IDisplayController::UpdateCallerAppletCaptureImage), "UpdateCallerAppletCaptureImage" },
            { 5, static_cast<InterfaceFunc>(&IDisplayController::GetLastForegroundCaptureImageEx), "GetLastForegroundCaptureImageEx" },
            { 6, static_cast<InterfaceFunc>(&IDisplayController::GetLastApplicationCaptureImageEx), "GetLastApplicationCaptureImageEx" },
            { 7, static_cast<InterfaceFunc>(&IDisplayController::GetCallerAppletCaptureImageEx), "GetCallerAppletCaptureImageEx" },
            { 8, static_cast<InterfaceFunc>(&IDisplayController::TakeScreenShotOfOwnLayer), "TakeScreenShotOfOwnLayer" },
            { 10, static_cast<InterfaceFunc>(&IDisplayController::AcquireLastApplicationCaptureBuffer), "AcquireLastApplicationCaptureBuffer" },
            { 11, static_cast<InterfaceFunc>(&IDisplayController::ReleaseLastApplicationCaptureBuffer), "ReleaseLastApplicationCaptureBuffer" },
            { 12, static_cast<InterfaceFunc>(&IDisplayController::AcquireLastForegroundCaptureBuffer), "AcquireLastForegroundCaptureBuffer" },
            { 13, static_cast<InterfaceFunc>(&IDisplayController::ReleaseLastForegroundCaptureBuffer), "ReleaseLastForegroundCaptureBuffer" },
            { 14, static_cast<InterfaceFunc>(&IDisplayController::AcquireCallerAppletCaptureBuffer), "AcquireCallerAppletCaptureBuffer" },
            { 15, static_cast<InterfaceFunc>(&IDisplayController::ReleaseCallerAppletCaptureBuffer), "ReleaseCallerAppletCaptureBuffer" },
            { 16, static_cast<InterfaceFunc>(&IDisplayController::AcquireLastApplicationCaptureBufferEx), "AcquireLastApplicationCaptureBufferEx" },
            { 17, static_cast<InterfaceFunc>(&IDisplayController::AcquireLastForegroundCaptureBufferEx), "AcquireLastForegroundCaptureBufferEx" },
            { 18, static_cast<InterfaceFunc>(&IDisplayController::AcquireCallerAppletCaptureBufferEx), "AcquireCallerAppletCaptureBufferEx" },
            { 20, static_cast<InterfaceFunc>(&IDisplayController::ClearCaptureBuffer), "ClearCaptureBuffer" },
            { 21, static_cast<InterfaceFunc>(&IDisplayController::ClearAppletTransitionBuffer), "ClearAppletTransitionBuffer" },
        });
    }

    std::string getName() {
        return "nn::am::service::IDisplayController";
    }
};

} // namespace service

} // namespace am

} // namespace nn
