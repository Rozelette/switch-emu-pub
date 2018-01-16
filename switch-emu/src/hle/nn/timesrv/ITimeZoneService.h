#pragma once
#include "hle\service.h"

namespace nn {

namespace timesrv {

namespace detail {

namespace service {

class ITimeZoneService : public ::service::Interface {

    SERVICE_FUNC_STUB(GetDeviceLocationName);
    SERVICE_FUNC_STUB(SetDeviceLocationName);
    SERVICE_FUNC_STUB(GetTotalLocationNameCount);
    SERVICE_FUNC_STUB(LoadLocationNameList);
    SERVICE_FUNC_STUB(LoadTimeZoneRule);
    SERVICE_FUNC_STUB(GetTimeZoneRuleVersion);
    SERVICE_FUNC_STUB(ToCalendarTime);
    uint32_t ToCalendarTimeWithMyRule(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(ToPosixTime);
    SERVICE_FUNC_STUB(ToPosixTimeWithMyRule);

public:
    ITimeZoneService() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ITimeZoneService::GetDeviceLocationName), "GetDeviceLocationName" },
            { 1, static_cast<InterfaceFunc>(&ITimeZoneService::SetDeviceLocationName), "SetDeviceLocationName" },
            { 2, static_cast<InterfaceFunc>(&ITimeZoneService::GetTotalLocationNameCount), "GetTotalLocationNameCount" },
            { 3, static_cast<InterfaceFunc>(&ITimeZoneService::LoadLocationNameList), "LoadLocationNameList" },
            { 4, static_cast<InterfaceFunc>(&ITimeZoneService::LoadTimeZoneRule), "LoadTimeZoneRule" },
            { 5, static_cast<InterfaceFunc>(&ITimeZoneService::GetTimeZoneRuleVersion), "GetTimeZoneRuleVersion" },
            { 100, static_cast<InterfaceFunc>(&ITimeZoneService::ToCalendarTime), "ToCalendarTime" },
            { 101, static_cast<InterfaceFunc>(&ITimeZoneService::ToCalendarTimeWithMyRule), "ToCalendarTimeWithMyRule" },
            { 201, static_cast<InterfaceFunc>(&ITimeZoneService::ToPosixTime), "ToPosixTime" },
            { 202, static_cast<InterfaceFunc>(&ITimeZoneService::ToPosixTimeWithMyRule), "ToPosixTimeWithMyRule" },
        });
    }

    std::string getName() {
        return "nn::timesrv::detail::service::ITimeZoneService";
    }
};

} // namespace service

} // namespace detail

} // namespace timesrv

} // namespace nn