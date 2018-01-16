#pragma once
#include "hle\service.h"

// Time Services
namespace nn {

namespace timesrv {

namespace detail {

namespace service {

class IStaticService: public ::service::Interface {

    uint32_t GetStandardUserSystemClock(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetStandardNetworkSystemClock(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetStandardSteadyClock(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetTimeZoneService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetStandardLocalSystemClock);
    SERVICE_FUNC_STUB(IsStandardUserSystemClockAutomaticCorrectionEnabled);
    SERVICE_FUNC_STUB(SetStandardUserSystemClockAutomaticCorrectionEnabled);
    SERVICE_FUNC_STUB(IsStandardNetworkSystemClockAccuracySufficient);

public:
    IStaticService() {
        registerFunctions({
        { 0, static_cast<InterfaceFunc>(&IStaticService::GetStandardUserSystemClock), "GetStandardUserSystemClock" },
        { 1, static_cast<InterfaceFunc>(&IStaticService::GetStandardNetworkSystemClock), "GetStandardNetworkSystemClock" },
        { 2, static_cast<InterfaceFunc>(&IStaticService::GetStandardSteadyClock), "GetStandardSteadyClock" },
        { 3, static_cast<InterfaceFunc>(&IStaticService::GetTimeZoneService), "GetTimeZoneService" },
        { 4, static_cast<InterfaceFunc>(&IStaticService::GetStandardLocalSystemClock), "GetStandardLocalSystemClock" },
        { 100, static_cast<InterfaceFunc>(&IStaticService::IsStandardUserSystemClockAutomaticCorrectionEnabled), "IsStandardUserSystemClockAutomaticCorrectionEnabled" },
        { 101, static_cast<InterfaceFunc>(&IStaticService::SetStandardUserSystemClockAutomaticCorrectionEnabled), "SetStandardUserSystemClockAutomaticCorrectionEnabled" },
        { 200, static_cast<InterfaceFunc>(&IStaticService::IsStandardNetworkSystemClockAccuracySufficient), "IsStandardNetworkSystemClockAccuracySufficient" },
        });

        addService(this, "time:u");
    }

    std::string getName() {
        return "nn::timesrv::detail::service::IStaticService";
    }
};

} // namespace service

} // namespace detail

} // namespace timesrv

} // namespace nn