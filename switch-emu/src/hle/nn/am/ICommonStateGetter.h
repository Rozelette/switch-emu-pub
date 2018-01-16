#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class ICommonStateGetter : public ::service::Interface {

    uint32_t GetEventHandle(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t ReceiveMessage(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetThisAppletKind);
    SERVICE_FUNC_STUB(AllowToEnterSleep);
    SERVICE_FUNC_STUB(DisallowToEnterSleep);
    uint32_t GetOperationMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetPerformanceMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetCradleStatus);
    SERVICE_FUNC_STUB(GetBootMode);
    uint32_t GetCurrentFocusState(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(RequestToAcquireSleepLock);
    SERVICE_FUNC_STUB(ReleaseSleepLock);
    SERVICE_FUNC_STUB(ReleaseSleepLockTransiently);
    SERVICE_FUNC_STUB(GetAcquiredSleepLockEvent);
    SERVICE_FUNC_STUB(PushToGeneralChannel);
    SERVICE_FUNC_STUB(GetHomeButtonReaderLockAccessor);
    SERVICE_FUNC_STUB(GetReaderLockAccessorEx);
    SERVICE_FUNC_STUB(GetCradleFwVersion);
    SERVICE_FUNC_STUB(IsVrModeEnabled);
    SERVICE_FUNC_STUB(SetVrModeEnabled);
    SERVICE_FUNC_STUB(IsInControllerFirmwareUpdateSection);
    SERVICE_FUNC_STUB(GetDefaultDisplayResolution);
    SERVICE_FUNC_STUB(GetDefaultDisplayResolutionChangeEvent);

public:
    ICommonStateGetter() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ICommonStateGetter::GetEventHandle), "GetEventHandle" },
            { 1, static_cast<InterfaceFunc>(&ICommonStateGetter::ReceiveMessage), "ReceiveMessage" },
            { 2, static_cast<InterfaceFunc>(&ICommonStateGetter::GetThisAppletKind), "GetThisAppletKind" },
            { 3, static_cast<InterfaceFunc>(&ICommonStateGetter::AllowToEnterSleep), "AllowToEnterSleep" },
            { 4, static_cast<InterfaceFunc>(&ICommonStateGetter::DisallowToEnterSleep), "DisallowToEnterSleep" },
            { 5, static_cast<InterfaceFunc>(&ICommonStateGetter::GetOperationMode), "GetOperationMode" },
            { 6, static_cast<InterfaceFunc>(&ICommonStateGetter::GetPerformanceMode), "GetPerformanceMode" },
            { 7, static_cast<InterfaceFunc>(&ICommonStateGetter::GetCradleStatus), "GetCradleStatus" },
            { 8, static_cast<InterfaceFunc>(&ICommonStateGetter::GetBootMode), "GetBootMode" },
            { 9, static_cast<InterfaceFunc>(&ICommonStateGetter::GetCurrentFocusState), "GetCurrentFocusState" },
            { 10, static_cast<InterfaceFunc>(&ICommonStateGetter::RequestToAcquireSleepLock), "RequestToAcquireSleepLock" },
            { 11, static_cast<InterfaceFunc>(&ICommonStateGetter::ReleaseSleepLock), "ReleaseSleepLock" },
            { 12, static_cast<InterfaceFunc>(&ICommonStateGetter::ReleaseSleepLockTransiently), "ReleaseSleepLockTransiently" },
            { 13, static_cast<InterfaceFunc>(&ICommonStateGetter::GetAcquiredSleepLockEvent), "GetAcquiredSleepLockEvent" },
            { 20, static_cast<InterfaceFunc>(&ICommonStateGetter::PushToGeneralChannel), "PushToGeneralChannel" },
            { 30, static_cast<InterfaceFunc>(&ICommonStateGetter::GetHomeButtonReaderLockAccessor), "GetHomeButtonReaderLockAccessor" },
            { 31, static_cast<InterfaceFunc>(&ICommonStateGetter::GetReaderLockAccessorEx), "GetReaderLockAccessorEx" },
            { 40, static_cast<InterfaceFunc>(&ICommonStateGetter::GetCradleFwVersion), "GetCradleFwVersion" },
            { 50, static_cast<InterfaceFunc>(&ICommonStateGetter::IsVrModeEnabled), "IsVrModeEnabled" },
            { 51, static_cast<InterfaceFunc>(&ICommonStateGetter::SetVrModeEnabled), "SetVrModeEnabled" },
            { 55, static_cast<InterfaceFunc>(&ICommonStateGetter::IsInControllerFirmwareUpdateSection), "IsInControllerFirmwareUpdateSection" },
            { 60, static_cast<InterfaceFunc>(&ICommonStateGetter::GetDefaultDisplayResolution), "GetDefaultDisplayResolution" },
            { 61, static_cast<InterfaceFunc>(&ICommonStateGetter::GetDefaultDisplayResolutionChangeEvent), "GetDefaultDisplayResolutionChangeEvent" },
        });
    }

    std::string getName() {
        return "nn::am::service::ICommonStateGetter";
    }
};

} // namespace service

} // namespace am

} // namespace nn
