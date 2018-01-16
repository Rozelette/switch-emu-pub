#pragma once
#include "hle\service.h"

// Nvidia Driver
namespace nns {

namespace nvdrv {

class INvDrvServices : public service::Interface {

    uint32_t Open(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t Ioctl(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t Close(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t Initialize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t QueryEvent(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(MapSharedMem);
    SERVICE_FUNC_STUB(GetStatus);
    SERVICE_FUNC_STUB(ForceSetClientPID);
    uint32_t SetClientPID(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(DumpGraphicsMemoryInfo);
    SERVICE_FUNC_STUB(unk10);
    SERVICE_FUNC_STUB(Ioctl2);
    SERVICE_FUNC_STUB(Ioctl3);
    SERVICE_FUNC_STUB(unk13);

public:
    INvDrvServices() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&INvDrvServices::Open), "Open" },
            { 1, static_cast<InterfaceFunc>(&INvDrvServices::Ioctl), "Ioctl" },
            { 2, static_cast<InterfaceFunc>(&INvDrvServices::Close), "Close" },
            { 3, static_cast<InterfaceFunc>(&INvDrvServices::Initialize), "Initialize" },
            { 4, static_cast<InterfaceFunc>(&INvDrvServices::QueryEvent), "QueryEvent" },
            { 5, static_cast<InterfaceFunc>(&INvDrvServices::MapSharedMem), "MapSharedMem" },
            { 6, static_cast<InterfaceFunc>(&INvDrvServices::GetStatus), "GetStatus" },
            { 7, static_cast<InterfaceFunc>(&INvDrvServices::ForceSetClientPID), "ForceSetClientPID" },
            { 8, static_cast<InterfaceFunc>(&INvDrvServices::SetClientPID), "SetClientPID" },
            { 9, static_cast<InterfaceFunc>(&INvDrvServices::DumpGraphicsMemoryInfo), "DumpGraphicsMemoryInfo" },
            { 10, static_cast<InterfaceFunc>(&INvDrvServices::unk10), "SetClientPID" },
            { 11, static_cast<InterfaceFunc>(&INvDrvServices::Ioctl2), "Ioctl2" },
            { 12, static_cast<InterfaceFunc>(&INvDrvServices::Ioctl3), "Ioctl3" },
            { 13, static_cast<InterfaceFunc>(&INvDrvServices::unk13), "SetClientPID" },
        });

        // TODO what are the differences?
        addService(this, "nvdrv");
        addService(this, "nvdrv:a");
        addService(this, "nvdrv:s");
        addService(this, "nvdrv:t");
    }

    std::string getName() {
        return "nns::nvdrv::INvDrvServices";
    }
};

} // namespace nvdrv

} // namespace nns