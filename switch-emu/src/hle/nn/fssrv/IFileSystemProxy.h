#pragma once
#include "hle\service.h"

// FS(main) Services
namespace nn {

namespace fssrv {

namespace sf {

class IFileSystemProxy : public service::Interface {

    uint32_t Initialize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(OpenDataFileSystemByCurrentProcess);
    SERVICE_FUNC_STUB(MountContent7);
    SERVICE_FUNC_STUB(MountContent);
    SERVICE_FUNC_STUB(OpenDataFileSystemByApplicationId);
    SERVICE_FUNC_STUB(MountBis);
    SERVICE_FUNC_STUB(OpenBisPartition);
    SERVICE_FUNC_STUB(InvalidateBisCache);
    SERVICE_FUNC_STUB(OpenHostFileSystemImpl);
    SERVICE_FUNC_STUB(MountSdCard);
    SERVICE_FUNC_STUB(FormatSdCard);
    SERVICE_FUNC_STUB(DeleteSaveData);
    SERVICE_FUNC_STUB(CreateSaveData);
    SERVICE_FUNC_STUB(CreateSystemSaveData);
    SERVICE_FUNC_STUB(RegisterSaveDataAtomicDeletion);
    SERVICE_FUNC_STUB(DeleteSaveDataWithSpaceId);
    SERVICE_FUNC_STUB(FormatSdCardDryRun);
    SERVICE_FUNC_STUB(IsExFatSupported);
    SERVICE_FUNC_STUB(OpenGameCardPartition);
    SERVICE_FUNC_STUB(MountGameCardPartition);
    SERVICE_FUNC_STUB(ExtendSaveData);
    uint32_t MountSaveData(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(MountSystemSaveData);
    SERVICE_FUNC_STUB(MountSaveDataReadOnly);
    SERVICE_FUNC_STUB(ReadSaveDataFileSystemExtraDataWithSpaceId);
    SERVICE_FUNC_STUB(ReadSaveDataFileSystemExtraData);
    SERVICE_FUNC_STUB(WriteSaveDataFileSystemExtraData);
    SERVICE_FUNC_STUB(OpenSaveDataInfoReader);
    SERVICE_FUNC_STUB(OpenSaveDataIterator);
    SERVICE_FUNC_STUB(OpenSaveDataThumbnailFile);
    SERVICE_FUNC_STUB(MountImageDirectory);
    SERVICE_FUNC_STUB(MountContentStorage);
    uint32_t OpenDataStorageByCurrentProcess(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(OpenDataStorageByApplicationId);
    SERVICE_FUNC_STUB(OpenDataStorageByDataId);
    uint32_t OpenRomStorage(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(OpenDeviceOperator);
    SERVICE_FUNC_STUB(OpenSdCardDetectionEventNotifier);
    SERVICE_FUNC_STUB(OpenGameCardDetectionEventNotifier);
    SERVICE_FUNC_STUB(SetCurrentPosixTime);
    SERVICE_FUNC_STUB(QuerySaveDataTotalSize);
    SERVICE_FUNC_STUB(VerifySaveData);
    SERVICE_FUNC_STUB(CorruptSaveDataForDebug);
    SERVICE_FUNC_STUB(CreatePaddingFile);
    SERVICE_FUNC_STUB(DeleteAllPaddingFiles);
    SERVICE_FUNC_STUB(GetRightsId);
    SERVICE_FUNC_STUB(RegisterExternalKey);
    SERVICE_FUNC_STUB(UnregisterExternalKey);
    SERVICE_FUNC_STUB(GetRightsIdByPath);
    SERVICE_FUNC_STUB(GetRightsIdByPath2);
    SERVICE_FUNC_STUB(SetSdCardEncryptionSeed);
    SERVICE_FUNC_STUB(GetAndClearFileSystemProxyErrorInfo);
    SERVICE_FUNC_STUB(SetBisRootForHost);
    SERVICE_FUNC_STUB(SetSaveDataSize);
    SERVICE_FUNC_STUB(SetSaveDataRootPath);
    SERVICE_FUNC_STUB(DisableAutoSaveDataCreation);
    SERVICE_FUNC_STUB(SetGlobalAccessLogMode);
    uint32_t GetGlobalAccessLogMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(OutputAccessLogToSdCard);

public:
    IFileSystemProxy() {
        registerFunctions({
            { 1, static_cast<InterfaceFunc>(&IFileSystemProxy::Initialize), "Initialize" },
            { 2, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenDataFileSystemByCurrentProcess), "OpenDataFileSystemByCurrentProcess" },
            { 7, static_cast<InterfaceFunc>(&IFileSystemProxy::MountContent7), "MountContent7" },
            { 8, static_cast<InterfaceFunc>(&IFileSystemProxy::MountContent), "MountContent" },
            { 9, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenDataFileSystemByApplicationId), "OpenDataFileSystemByApplicationId" },
            { 11, static_cast<InterfaceFunc>(&IFileSystemProxy::MountBis), "MountBis" },
            { 12, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenBisPartition), "OpenBisPartition" },
            { 13, static_cast<InterfaceFunc>(&IFileSystemProxy::InvalidateBisCache), "InvalidateBisCache" },
            { 17, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenHostFileSystemImpl), "OpenHostFileSystemImpl" },
            { 18, static_cast<InterfaceFunc>(&IFileSystemProxy::MountSdCard), "MountSdCard" },
            { 19, static_cast<InterfaceFunc>(&IFileSystemProxy::FormatSdCard), "FormatSdCard" },
            { 21, static_cast<InterfaceFunc>(&IFileSystemProxy::DeleteSaveData), "DeleteSaveData" },
            { 22, static_cast<InterfaceFunc>(&IFileSystemProxy::CreateSaveData), "CreateSaveData" },
            { 23, static_cast<InterfaceFunc>(&IFileSystemProxy::CreateSystemSaveData), "CreateSystemSaveData" },
            { 24, static_cast<InterfaceFunc>(&IFileSystemProxy::RegisterSaveDataAtomicDeletion), "RegisterSaveDataAtomicDeletion" },
            { 25, static_cast<InterfaceFunc>(&IFileSystemProxy::DeleteSaveDataWithSpaceId), "DeleteSaveDataWithSpaceId" },
            { 26, static_cast<InterfaceFunc>(&IFileSystemProxy::FormatSdCardDryRun), "FormatSdCardDryRun" },
            { 27, static_cast<InterfaceFunc>(&IFileSystemProxy::IsExFatSupported), "IsExFatSupported" },
            { 30, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenGameCardPartition), "OpenGameCardPartition" },
            { 31, static_cast<InterfaceFunc>(&IFileSystemProxy::MountGameCardPartition), "MountGameCardPartition" },
            { 32, static_cast<InterfaceFunc>(&IFileSystemProxy::ExtendSaveData), "ExtendSaveData" },
            { 51, static_cast<InterfaceFunc>(&IFileSystemProxy::MountSaveData), "MountSaveData" },
            { 52, static_cast<InterfaceFunc>(&IFileSystemProxy::MountSystemSaveData), "MountSystemSaveData" },
            { 53, static_cast<InterfaceFunc>(&IFileSystemProxy::MountSaveDataReadOnly), "MountSaveDataReadOnly" },
            { 57, static_cast<InterfaceFunc>(&IFileSystemProxy::ReadSaveDataFileSystemExtraDataWithSpaceId), "ReadSaveDataFileSystemExtraDataWithSpaceId" },
            { 58, static_cast<InterfaceFunc>(&IFileSystemProxy::ReadSaveDataFileSystemExtraData), "ReadSaveDataFileSystemExtraData" },
            { 59, static_cast<InterfaceFunc>(&IFileSystemProxy::WriteSaveDataFileSystemExtraData), "WriteSaveDataFileSystemExtraData" },
            { 60, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenSaveDataInfoReader), "OpenSaveDataInfoReader" },
            { 61, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenSaveDataIterator), "OpenSaveDataIterator" },
            { 80, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenSaveDataThumbnailFile), "OpenSaveDataThumbnailFile" },
            { 100, static_cast<InterfaceFunc>(&IFileSystemProxy::MountImageDirectory), "MountImageDirectory" },
            { 110, static_cast<InterfaceFunc>(&IFileSystemProxy::MountContentStorage), "MountContentStorage" },
            { 200, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenDataStorageByCurrentProcess), "OpenDataStorageByCurrentProcess" },
            { 201, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenDataStorageByApplicationId), "OpenDataStorageByApplicationId" },
            { 202, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenDataStorageByDataId), "OpenDataStorageByDataId" },
            { 203, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenRomStorage), "OpenRomStorage" },
            { 400, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenDeviceOperator), "OpenDeviceOperator" },
            { 500, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenSdCardDetectionEventNotifier), "OpenSdCardDetectionEventNotifier" },
            { 501, static_cast<InterfaceFunc>(&IFileSystemProxy::OpenGameCardDetectionEventNotifier), "OpenGameCardDetectionEventNotifier" },
            { 600, static_cast<InterfaceFunc>(&IFileSystemProxy::SetCurrentPosixTime), "SetCurrentPosixTime" },
            { 601, static_cast<InterfaceFunc>(&IFileSystemProxy::QuerySaveDataTotalSize), "QuerySaveDataTotalSize" },
            { 602, static_cast<InterfaceFunc>(&IFileSystemProxy::VerifySaveData), "VerifySaveData" },
            { 603, static_cast<InterfaceFunc>(&IFileSystemProxy::CorruptSaveDataForDebug), "CorruptSaveDataForDebug" },
            { 604, static_cast<InterfaceFunc>(&IFileSystemProxy::CreatePaddingFile), "CreatePaddingFile" },
            { 605, static_cast<InterfaceFunc>(&IFileSystemProxy::DeleteAllPaddingFiles), "DeleteAllPaddingFiles" },
            { 606, static_cast<InterfaceFunc>(&IFileSystemProxy::GetRightsId), "GetRightsId" },
            { 607, static_cast<InterfaceFunc>(&IFileSystemProxy::RegisterExternalKey), "RegisterExternalKey" },
            { 607, static_cast<InterfaceFunc>(&IFileSystemProxy::UnregisterExternalKey), "UnregisterExternalKey" },
            { 609, static_cast<InterfaceFunc>(&IFileSystemProxy::GetRightsIdByPath), "GetRightsIdByPath" },
            { 610, static_cast<InterfaceFunc>(&IFileSystemProxy::GetRightsIdByPath2), "GetRightsIdByPath2" },
            { 620, static_cast<InterfaceFunc>(&IFileSystemProxy::SetSdCardEncryptionSeed), "SetSdCardEncryptionSeed" },
            { 800, static_cast<InterfaceFunc>(&IFileSystemProxy::GetAndClearFileSystemProxyErrorInfo), "GetAndClearFileSystemProxyErrorInfo" },
            { 1000, static_cast<InterfaceFunc>(&IFileSystemProxy::SetBisRootForHost), "SetBisRootForHost" },
            { 1001, static_cast<InterfaceFunc>(&IFileSystemProxy::SetSaveDataSize), "SetSaveDataSize" },
            { 1002, static_cast<InterfaceFunc>(&IFileSystemProxy::SetSaveDataRootPath), "SetSaveDataRootPath" },
            { 1003, static_cast<InterfaceFunc>(&IFileSystemProxy::DisableAutoSaveDataCreation), "DisableAutoSaveDataCreation" },
            { 1004, static_cast<InterfaceFunc>(&IFileSystemProxy::SetGlobalAccessLogMode), "SetGlobalAccessLogMode" },
            { 1005, static_cast<InterfaceFunc>(&IFileSystemProxy::GetGlobalAccessLogMode), "GetGlobalAccessLogMode" },
            { 1006, static_cast<InterfaceFunc>(&IFileSystemProxy::OutputAccessLogToSdCard), "OutputAccessLogToSdCard" },
        });

        addService(this, "fsp-srv");
    }

    std::string getName() {
        return "nn::fssrv::sf::IFileSystemProxy";
    }
};

} // namespace sf

} // namespace fssrv

} // namespace nn