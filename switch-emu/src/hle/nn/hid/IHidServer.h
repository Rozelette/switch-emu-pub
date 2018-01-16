#pragma once
#include "hle\service.h"

// HID Services
namespace nn {

namespace hid {

class IHidServer : public service::Interface {

    uint32_t CreateAppletResource(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(ActivateDebugPad);
    uint32_t ActivateTouchScreen(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(ActivateMouse);
    SERVICE_FUNC_STUB(ActivateKeyboard);
    SERVICE_FUNC_STUB(AcquireXpadIdEventHandle);
    SERVICE_FUNC_STUB(ReleaseXpadIdEventHandle);
    SERVICE_FUNC_STUB(ActivateXpad);
    SERVICE_FUNC_STUB(GetXpadIds);
    SERVICE_FUNC_STUB(ActivateJoyXpad);
    SERVICE_FUNC_STUB(GetJoyXpadLifoHandle);
    SERVICE_FUNC_STUB(GetJoyXpadIds);
    SERVICE_FUNC_STUB(ActivateSixAxisSensor);
    SERVICE_FUNC_STUB(DeactivateSixAxisSensor);
    SERVICE_FUNC_STUB(GetSixAxisSensorLifoHandle);
    SERVICE_FUNC_STUB(ActivateJoySixAxisSensor);
    SERVICE_FUNC_STUB(DeactivateJoySixAxisSensor);
    SERVICE_FUNC_STUB(GetJoySixAxisSensorLifoHandle);
    SERVICE_FUNC_STUB(StartSixAxisSensor);
    SERVICE_FUNC_STUB(StopSixAxisSensor);
    SERVICE_FUNC_STUB(IsSixAxisSensorFusionEnabled);
    SERVICE_FUNC_STUB(EnableSixAxisSensorFusion);
    SERVICE_FUNC_STUB(SetSixAxisSensorFusionParameters);
    SERVICE_FUNC_STUB(GetSixAxisSensorFusionParameters);
    SERVICE_FUNC_STUB(ResetSixAxisSensorFusionParameters);
    SERVICE_FUNC_STUB(SetAccelerometerParameters);
    SERVICE_FUNC_STUB(GetAccelerometerParameters);
    SERVICE_FUNC_STUB(ResetAccelerometerParameters);
    SERVICE_FUNC_STUB(SetAccelerometerPlayMode);
    SERVICE_FUNC_STUB(GetAccelerometerPlayMode);
    SERVICE_FUNC_STUB(ResetAccelerometerPlayMode);
    SERVICE_FUNC_STUB(SetGyroscopeZeroDriftMode);
    SERVICE_FUNC_STUB(GetGyroscopeZeroDriftMode);
    SERVICE_FUNC_STUB(ResetGyroscopeZeroDriftMode);
    SERVICE_FUNC_STUB(IsSixAxisSensorAtRest);
    SERVICE_FUNC_STUB(ActivateGesture);
    uint32_t SetSupportedNpadStyleSet(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetSupportedNpadStyleSet);
    uint32_t SetSupportedNpadIdType(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t ActivateNpad(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(DeactivateNpad);
    SERVICE_FUNC_STUB(AcquireNpadStyleSetUpdateEventHandle);
    SERVICE_FUNC_STUB(DisconnectNpad);
    SERVICE_FUNC_STUB(GetPlayerLedPattern);
    uint32_t SetNpadJoyHoldType(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetNpadJoyHoldType);
    SERVICE_FUNC_STUB(SetNpadJoyAssignmentModeSingleByDefault);
    SERVICE_FUNC_STUB(SetNpadJoyAssignmentModeSingle);
    SERVICE_FUNC_STUB(SetNpadJoyAssignmentModeDual);
    SERVICE_FUNC_STUB(MergeSingleJoyAsDualJoy);
    SERVICE_FUNC_STUB(StartLrAssignmentMode);
    SERVICE_FUNC_STUB(StopLrAssignmentMode);
    SERVICE_FUNC_STUB(SetNpadHandheldActivationMode);
    SERVICE_FUNC_STUB(GetNpadHandheldActivationMode);
    SERVICE_FUNC_STUB(SwapNpadAssignment);
    SERVICE_FUNC_STUB(IsUnintendedHomeButtonInputProtectionEnabled);
    SERVICE_FUNC_STUB(EnableUnintendedHomeButtonInputProtection);
    SERVICE_FUNC_STUB(GetVibrationDeviceInfo);
    SERVICE_FUNC_STUB(SendVibrationValue);
    SERVICE_FUNC_STUB(GetActualVibrationValue);
    SERVICE_FUNC_STUB(CreateActiveVibrationDeviceList);
    SERVICE_FUNC_STUB(PermitVibration);
    SERVICE_FUNC_STUB(IsVibrationPermitted);
    SERVICE_FUNC_STUB(SendVibrationValues);
    SERVICE_FUNC_STUB(ActivateConsoleSixAxisSensor);
    SERVICE_FUNC_STUB(StartConsoleSixAxisSensor);
    SERVICE_FUNC_STUB(StopConsoleSixAxisSensor);
    SERVICE_FUNC_STUB(IsUsbFullKeyControllerEnabled);
    SERVICE_FUNC_STUB(EnableUsbFullKeyController);
    SERVICE_FUNC_STUB(IsUsbFullKeyControllerConnected);
    SERVICE_FUNC_STUB(SetNpadCommunicationMode);
    SERVICE_FUNC_STUB(GetNpadCommunicationMode);

public:
    IHidServer() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IHidServer::CreateAppletResource), "CreateAppletResource" },
            { 1, static_cast<InterfaceFunc>(&IHidServer::ActivateDebugPad), "ActivateDebugPad" },
            { 11, static_cast<InterfaceFunc>(&IHidServer::ActivateTouchScreen), "ActivateTouchScreen" },
            { 21, static_cast<InterfaceFunc>(&IHidServer::ActivateMouse), "ActivateMouse" },
            { 31, static_cast<InterfaceFunc>(&IHidServer::ActivateKeyboard), "ActivateKeyboard" },
            { 40, static_cast<InterfaceFunc>(&IHidServer::AcquireXpadIdEventHandle), "AcquireXpadIdEventHandle" },
            { 41, static_cast<InterfaceFunc>(&IHidServer::ReleaseXpadIdEventHandle), "ReleaseXpadIdEventHandle" },
            { 51, static_cast<InterfaceFunc>(&IHidServer::ActivateXpad), "ActivateXpad" },
            { 55, static_cast<InterfaceFunc>(&IHidServer::GetXpadIds), "GetXpadIds" },
            { 56, static_cast<InterfaceFunc>(&IHidServer::ActivateJoyXpad), "ActivateJoyXpad" },
            { 58, static_cast<InterfaceFunc>(&IHidServer::GetJoyXpadLifoHandle), "GetJoyXpadLifoHandle" },
            { 59, static_cast<InterfaceFunc>(&IHidServer::GetJoyXpadIds), "GetJoyXpadIds" },
            { 60, static_cast<InterfaceFunc>(&IHidServer::ActivateSixAxisSensor), "ActivateSixAxisSensor" },
            { 61, static_cast<InterfaceFunc>(&IHidServer::DeactivateSixAxisSensor), "DeactivateSixAxisSensor" },
            { 62, static_cast<InterfaceFunc>(&IHidServer::GetSixAxisSensorLifoHandle), "GetSixAxisSensorLifoHandle" },
            { 63, static_cast<InterfaceFunc>(&IHidServer::ActivateJoySixAxisSensor), "ActivateJoySixAxisSensor" },
            { 64, static_cast<InterfaceFunc>(&IHidServer::DeactivateJoySixAxisSensor), "DeactivateJoySixAxisSensor" },
            { 65, static_cast<InterfaceFunc>(&IHidServer::GetJoySixAxisSensorLifoHandle), "GetJoySixAxisSensorLifoHandle" },
            { 66, static_cast<InterfaceFunc>(&IHidServer::StartSixAxisSensor), "StartSixAxisSensor" },
            { 67, static_cast<InterfaceFunc>(&IHidServer::StopSixAxisSensor), "StopSixAxisSensor" },
            { 68, static_cast<InterfaceFunc>(&IHidServer::IsSixAxisSensorFusionEnabled), "IsSixAxisSensorFusionEnabled" },
            { 69, static_cast<InterfaceFunc>(&IHidServer::EnableSixAxisSensorFusion), "EnableSixAxisSensorFusion" },
            { 70, static_cast<InterfaceFunc>(&IHidServer::SetSixAxisSensorFusionParameters), "SetSixAxisSensorFusionParameters" },
            { 71, static_cast<InterfaceFunc>(&IHidServer::GetSixAxisSensorFusionParameters), "GetSixAxisSensorFusionParameters" },
            { 72, static_cast<InterfaceFunc>(&IHidServer::ResetSixAxisSensorFusionParameters), "ResetSixAxisSensorFusionParameters" },
            { 73, static_cast<InterfaceFunc>(&IHidServer::SetAccelerometerParameters), "SetAccelerometerParameters" },
            { 74, static_cast<InterfaceFunc>(&IHidServer::GetAccelerometerParameters), "GetAccelerometerParameters" },
            { 75, static_cast<InterfaceFunc>(&IHidServer::ResetAccelerometerParameters), "ResetAccelerometerParameters" },
            { 76, static_cast<InterfaceFunc>(&IHidServer::SetAccelerometerPlayMode), "SetAccelerometerPlayMode" },
            { 77, static_cast<InterfaceFunc>(&IHidServer::GetAccelerometerPlayMode), "GetAccelerometerPlayMode" },
            { 78, static_cast<InterfaceFunc>(&IHidServer::ResetAccelerometerPlayMode), "ResetAccelerometerPlayMode" },
            { 79, static_cast<InterfaceFunc>(&IHidServer::SetGyroscopeZeroDriftMode), "SetGyroscopeZeroDriftMode" },
            { 80, static_cast<InterfaceFunc>(&IHidServer::GetGyroscopeZeroDriftMode), "GetGyroscopeZeroDriftMode" },
            { 81, static_cast<InterfaceFunc>(&IHidServer::ResetGyroscopeZeroDriftMode), "ResetGyroscopeZeroDriftMode" },
            { 82, static_cast<InterfaceFunc>(&IHidServer::IsSixAxisSensorAtRest), "IsSixAxisSensorAtRest" },
            { 91, static_cast<InterfaceFunc>(&IHidServer::ActivateGesture), "ActivateGesture" },
            { 100, static_cast<InterfaceFunc>(&IHidServer::SetSupportedNpadStyleSet), "SetSupportedNpadStyleSet" },
            { 101, static_cast<InterfaceFunc>(&IHidServer::GetSupportedNpadStyleSet), "GetSupportedNpadStyleSet" },
            { 102, static_cast<InterfaceFunc>(&IHidServer::SetSupportedNpadIdType), "SetSupportedNpadIdType" },
            { 103, static_cast<InterfaceFunc>(&IHidServer::ActivateNpad), "ActivateNpad" },
            { 104, static_cast<InterfaceFunc>(&IHidServer::DeactivateNpad), "DeactivateNpad" },
            { 106, static_cast<InterfaceFunc>(&IHidServer::AcquireNpadStyleSetUpdateEventHandle), "AcquireNpadStyleSetUpdateEventHandle" },
            { 107, static_cast<InterfaceFunc>(&IHidServer::DisconnectNpad), "DisconnectNpad" },
            { 108, static_cast<InterfaceFunc>(&IHidServer::GetPlayerLedPattern), "GetPlayerLedPattern" },
            { 120, static_cast<InterfaceFunc>(&IHidServer::SetNpadJoyHoldType), "SetNpadJoyHoldType" },
            { 121, static_cast<InterfaceFunc>(&IHidServer::GetNpadJoyHoldType), "GetNpadJoyHoldType" },
            { 122, static_cast<InterfaceFunc>(&IHidServer::SetNpadJoyAssignmentModeSingleByDefault), "SetNpadJoyAssignmentModeSingleByDefault" },
            { 123, static_cast<InterfaceFunc>(&IHidServer::SetNpadJoyAssignmentModeSingle), "SetNpadJoyAssignmentModeSingle" },
            { 124, static_cast<InterfaceFunc>(&IHidServer::SetNpadJoyAssignmentModeDual), "SetNpadJoyAssignmentModeDual" },
            { 125, static_cast<InterfaceFunc>(&IHidServer::MergeSingleJoyAsDualJoy), "MergeSingleJoyAsDualJoy" },
            { 126, static_cast<InterfaceFunc>(&IHidServer::StartLrAssignmentMode), "StartLrAssignmentMode" },
            { 127, static_cast<InterfaceFunc>(&IHidServer::StopLrAssignmentMode), "StopLrAssignmentMode" },
            { 128, static_cast<InterfaceFunc>(&IHidServer::SetNpadHandheldActivationMode), "SetNpadHandheldActivationMode" },
            { 129, static_cast<InterfaceFunc>(&IHidServer::GetNpadHandheldActivationMode), "GetNpadHandheldActivationMode" },
            { 130, static_cast<InterfaceFunc>(&IHidServer::SwapNpadAssignment), "SwapNpadAssignment" },
            { 131, static_cast<InterfaceFunc>(&IHidServer::IsUnintendedHomeButtonInputProtectionEnabled), "IsUnintendedHomeButtonInputProtectionEnabled" },
            { 132, static_cast<InterfaceFunc>(&IHidServer::EnableUnintendedHomeButtonInputProtection), "EnableUnintendedHomeButtonInputProtection" },
            { 200, static_cast<InterfaceFunc>(&IHidServer::GetVibrationDeviceInfo), "GetVibrationDeviceInfo" },
            { 201, static_cast<InterfaceFunc>(&IHidServer::SendVibrationValue), "SendVibrationValue" },
            { 202, static_cast<InterfaceFunc>(&IHidServer::GetActualVibrationValue), "GetActualVibrationValue" },
            { 203, static_cast<InterfaceFunc>(&IHidServer::CreateActiveVibrationDeviceList), "CreateActiveVibrationDeviceList" },
            { 204, static_cast<InterfaceFunc>(&IHidServer::PermitVibration), "PermitVibration" },
            { 205, static_cast<InterfaceFunc>(&IHidServer::IsVibrationPermitted), "IsVibrationPermitted" },
            { 206, static_cast<InterfaceFunc>(&IHidServer::SendVibrationValues), "SendVibrationValues" },
            { 300, static_cast<InterfaceFunc>(&IHidServer::ActivateConsoleSixAxisSensor), "ActivateConsoleSixAxisSensor" },
            { 301, static_cast<InterfaceFunc>(&IHidServer::StartConsoleSixAxisSensor), "StartConsoleSixAxisSensor" },
            { 302, static_cast<InterfaceFunc>(&IHidServer::StopConsoleSixAxisSensor), "StopConsoleSixAxisSensor" },
            { 400, static_cast<InterfaceFunc>(&IHidServer::IsUsbFullKeyControllerEnabled), "IsUsbFullKeyControllerEnabled" },
            { 401, static_cast<InterfaceFunc>(&IHidServer::EnableUsbFullKeyController), "EnableUsbFullKeyController" },
            { 402, static_cast<InterfaceFunc>(&IHidServer::IsUsbFullKeyControllerConnected), "IsUsbFullKeyControllerConnected" },
            { 1000, static_cast<InterfaceFunc>(&IHidServer::SetNpadCommunicationMode), "SetNpadCommunicationMode" },
            { 1001, static_cast<InterfaceFunc>(&IHidServer::GetNpadCommunicationMode), "GetNpadCommunicationMode" },
        });

        addService(this, "hid");
    }

    std::string getName() {
        return "nn::hid::IHidServer";
    }
};

} // namespace hid

} // namespace nn
