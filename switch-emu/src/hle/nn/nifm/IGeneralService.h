#pragma once
#include "hle\service.h"

namespace nn {

namespace nifm {

namespace detail {

class IGeneralService : public service::Interface {

    SERVICE_FUNC_STUB(GetClientId);
    SERVICE_FUNC_STUB(CreateScanRequest);
    SERVICE_FUNC_STUB(CreateRequest);
    SERVICE_FUNC_STUB(GetCurrentNetworkProfile);
    SERVICE_FUNC_STUB(EnumerateNetworkInterfaces);
    SERVICE_FUNC_STUB(EnumerateNetworkProfiles);
    SERVICE_FUNC_STUB(GetNetworkProfile);
    SERVICE_FUNC_STUB(SetNetworkProfile);
    SERVICE_FUNC_STUB(RemoveNetworkProfile);
    SERVICE_FUNC_STUB(GetScanData);
    SERVICE_FUNC_STUB(GetCurrentIpAddress);
    SERVICE_FUNC_STUB(GetCurrentAccessPoint);
    SERVICE_FUNC_STUB(CreateTemporaryNetworkProfile);
    SERVICE_FUNC_STUB(GetCurrentIpConfigInfo);
    SERVICE_FUNC_STUB(SetWirelessCommunicationEnabled);
    SERVICE_FUNC_STUB(IsWirelessCommunicationEnabled);
    SERVICE_FUNC_STUB(GetInternetConnectionStatus);
    SERVICE_FUNC_STUB(SetEthernetCommunicationEnabled);
    SERVICE_FUNC_STUB(IsEthernetCommunicationEnabled);
    SERVICE_FUNC_STUB(IsAnyInternetRequestAccepted);
    SERVICE_FUNC_STUB(IsAnyForegroundRequestAccepted);
    SERVICE_FUNC_STUB(PutToSleep);
    SERVICE_FUNC_STUB(WakeUp);
    SERVICE_FUNC_STUB(GetSsidListVersion);
    SERVICE_FUNC_STUB(SetExclusiveClient);
    SERVICE_FUNC_STUB(GetDefaultIpSetting);
    SERVICE_FUNC_STUB(SetDefaultIpSetting);
    SERVICE_FUNC_STUB(SetWirelessCommunicationEnabledForTest);
    SERVICE_FUNC_STUB(SetEthernetCommunicationEnabledForTest);
    SERVICE_FUNC_STUB(GetTelemetorySystemEventReadableHandle);
    SERVICE_FUNC_STUB(GetTelemetryInfo);
    SERVICE_FUNC_STUB(ConfirmSystemAvailability);

public:
    IGeneralService() {
        registerFunctions({
            { 1, static_cast<InterfaceFunc>(&IGeneralService::GetClientId), "GetClientId" },
            { 2, static_cast<InterfaceFunc>(&IGeneralService::CreateScanRequest), "CreateScanRequest" },
            { 4, static_cast<InterfaceFunc>(&IGeneralService::CreateRequest), "CreateRequest" },
            { 5, static_cast<InterfaceFunc>(&IGeneralService::GetCurrentNetworkProfile), "GetCurrentNetworkProfile" },
            { 6, static_cast<InterfaceFunc>(&IGeneralService::EnumerateNetworkInterfaces), "EnumerateNetworkInterfaces" },
            { 7, static_cast<InterfaceFunc>(&IGeneralService::EnumerateNetworkProfiles), "EnumerateNetworkProfiles" },
            { 8, static_cast<InterfaceFunc>(&IGeneralService::GetNetworkProfile), "GetNetworkProfile" },
            { 9, static_cast<InterfaceFunc>(&IGeneralService::SetNetworkProfile), "SetNetworkProfile" },
            { 10, static_cast<InterfaceFunc>(&IGeneralService::RemoveNetworkProfile), "RemoveNetworkProfile" },
            { 11, static_cast<InterfaceFunc>(&IGeneralService::GetScanData), "GetScanData" },
            { 12, static_cast<InterfaceFunc>(&IGeneralService::GetCurrentIpAddress), "GetCurrentIpAddress" },
            { 13, static_cast<InterfaceFunc>(&IGeneralService::GetCurrentAccessPoint), "GetCurrentAccessPoint" },
            { 14, static_cast<InterfaceFunc>(&IGeneralService::CreateTemporaryNetworkProfile), "CreateTemporaryNetworkProfile" },
            { 15, static_cast<InterfaceFunc>(&IGeneralService::GetCurrentIpConfigInfo), "GetCurrentIpConfigInfo" },
            { 16, static_cast<InterfaceFunc>(&IGeneralService::SetWirelessCommunicationEnabled), "SetWirelessCommunicationEnabled" },
            { 17, static_cast<InterfaceFunc>(&IGeneralService::IsWirelessCommunicationEnabled), "IsWirelessCommunicationEnabled" },
            { 18, static_cast<InterfaceFunc>(&IGeneralService::GetInternetConnectionStatus), "GetInternetConnectionStatus" },
            { 19, static_cast<InterfaceFunc>(&IGeneralService::SetEthernetCommunicationEnabled), "SetEthernetCommunicationEnabled" },
            { 20, static_cast<InterfaceFunc>(&IGeneralService::IsEthernetCommunicationEnabled), "IsEthernetCommunicationEnabled" },
            { 21, static_cast<InterfaceFunc>(&IGeneralService::IsAnyInternetRequestAccepted), "IsAnyInternetRequestAccepted" },
            { 22, static_cast<InterfaceFunc>(&IGeneralService::IsAnyForegroundRequestAccepted), "IsAnyForegroundRequestAccepted" },
            { 23, static_cast<InterfaceFunc>(&IGeneralService::PutToSleep), "PutToSleep" },
            { 24, static_cast<InterfaceFunc>(&IGeneralService::WakeUp), "WakeUp" },
            { 25, static_cast<InterfaceFunc>(&IGeneralService::GetSsidListVersion), "GetSsidListVersion" },
            { 26, static_cast<InterfaceFunc>(&IGeneralService::SetExclusiveClient), "SetExclusiveClient" },
            { 27, static_cast<InterfaceFunc>(&IGeneralService::GetDefaultIpSetting), "GetDefaultIpSetting" },
            { 28, static_cast<InterfaceFunc>(&IGeneralService::SetDefaultIpSetting), "SetDefaultIpSetting" },
            { 29, static_cast<InterfaceFunc>(&IGeneralService::SetWirelessCommunicationEnabledForTest), "SetWirelessCommunicationEnabledForTest" },
            { 30, static_cast<InterfaceFunc>(&IGeneralService::SetEthernetCommunicationEnabledForTest), "SetEthernetCommunicationEnabledForTest" },
            { 31, static_cast<InterfaceFunc>(&IGeneralService::GetTelemetorySystemEventReadableHandle), "GetTelemetorySystemEventReadableHandle" },
            { 32, static_cast<InterfaceFunc>(&IGeneralService::GetTelemetryInfo), "GetTelemetryInfo" },
            { 33, static_cast<InterfaceFunc>(&IGeneralService::ConfirmSystemAvailability), "ConfirmSystemAvailability" },
        });
    }

    std::string getName() {
        return "nn::nifm::detail::IGeneralService";
    }
};

} // namespace detail

} // namespace nifm

} // namespace nn
