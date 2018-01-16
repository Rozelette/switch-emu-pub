#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class IProcessWindingController : public ::service::Interface {

    SERVICE_FUNC_STUB(GetLaunchReason);
    SERVICE_FUNC_STUB(OpenCallingLibraryApplet);
    SERVICE_FUNC_STUB(PushContext);
    SERVICE_FUNC_STUB(PopContext);
    SERVICE_FUNC_STUB(CancelWindingReservation);
    SERVICE_FUNC_STUB(WindAndDoReserved);
    SERVICE_FUNC_STUB(ReserveToStartAndWaitAndUnwindThis);

public:
    IProcessWindingController() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IProcessWindingController::GetLaunchReason), "GetLaunchReason" },
            { 11, static_cast<InterfaceFunc>(&IProcessWindingController::OpenCallingLibraryApplet), "OpenCallingLibraryApplet" },
            { 21, static_cast<InterfaceFunc>(&IProcessWindingController::PushContext), "PushContext" },
            { 22, static_cast<InterfaceFunc>(&IProcessWindingController::PopContext), "PopContext" },
            { 23, static_cast<InterfaceFunc>(&IProcessWindingController::CancelWindingReservation), "CancelWindingReservation" },
            { 30, static_cast<InterfaceFunc>(&IProcessWindingController::WindAndDoReserved), "WindAndDoReserved" },
            { 40, static_cast<InterfaceFunc>(&IProcessWindingController::ReserveToStartAndWaitAndUnwindThis), "ReserveToStartAndWaitAndUnwindThis" },
        });
    }

    std::string getName() {
        return "nn::am::service::IProcessWindingController";
    }
};

} // namespace service

} // namespace am

} // namespace nn
