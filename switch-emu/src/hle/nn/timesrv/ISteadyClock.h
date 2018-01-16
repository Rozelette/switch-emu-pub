#pragma once
#include "hle\service.h"

namespace nn {

namespace timesrv {

namespace detail {

namespace service {

class ISteadyClock : public ::service::Interface {

    SERVICE_FUNC_STUB(GetCurrentTimePoint);
    SERVICE_FUNC_STUB(GetTestOffset);
    SERVICE_FUNC_STUB(SetTestOffset);
    SERVICE_FUNC_STUB(GetRtcValue);
    SERVICE_FUNC_STUB(IsRtcResetDetected);
    SERVICE_FUNC_STUB(GetSetupResutltValue);
    SERVICE_FUNC_STUB(GetInternalOffset);
    SERVICE_FUNC_STUB(SetInternalOffset);

public:
    ISteadyClock() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ISteadyClock::GetCurrentTimePoint), "GetCurrentTimePoint" },
            { 2, static_cast<InterfaceFunc>(&ISteadyClock::GetTestOffset), "GetTestOffset" },
            { 3, static_cast<InterfaceFunc>(&ISteadyClock::SetTestOffset), "SetTestOffset" },
            { 100, static_cast<InterfaceFunc>(&ISteadyClock::GetRtcValue), "GetRtcValue" },
            { 101, static_cast<InterfaceFunc>(&ISteadyClock::IsRtcResetDetected), "IsRtcResetDetected" },
            { 102, static_cast<InterfaceFunc>(&ISteadyClock::GetSetupResutltValue), "GetSetupResutltValue" },
            { 200, static_cast<InterfaceFunc>(&ISteadyClock::GetInternalOffset), "GetInternalOffset" },
            { 201, static_cast<InterfaceFunc>(&ISteadyClock::SetInternalOffset), "SetInternalOffset" },
        });
    }

    std::string getName() {
        return "nn::timesrv::detail::service::ISteadyClock";
    }
};

} // namespace service

} // namespace detail

} // namespace timesrv

} // namespace nn