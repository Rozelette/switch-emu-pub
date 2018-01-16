#pragma once
#include "hle\service.h"

namespace nn {

namespace timesrv {

namespace detail {

namespace service {

class ISystemClock : public ::service::Interface {

    uint32_t GetCurrentTime(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(SetCurrentTime);
    SERVICE_FUNC_STUB(GetSystemClockContext);
    SERVICE_FUNC_STUB(SetSystemClockContext);

public:
    ISystemClock() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ISystemClock::GetCurrentTime), "GetCurrentTime" },
            { 1, static_cast<InterfaceFunc>(&ISystemClock::SetCurrentTime), "SetCurrentTime" },
            { 2, static_cast<InterfaceFunc>(&ISystemClock::GetSystemClockContext), "GetSystemClockContext" },
            { 3, static_cast<InterfaceFunc>(&ISystemClock::SetSystemClockContext), "SetSystemClockContext" },
        });
    }

    std::string getName() {
        return "nn::timesrv::detail::service::ISystemClock";
    }
};

} // namespace service

} // namespace detail

} // namespace timesrv

} // namespace nn