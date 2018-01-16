#include "IFileSystemProxy.h"
#include "IStorage.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace fssrv {

namespace sf {

uint32_t IFileSystemProxy::Initialize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    return ERR_SUCCESS;
}

uint32_t IFileSystemProxy::MountSaveData(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* storage = new IStorage("saveData");
    reply->moveHandle(storage->connect());

    return ERR_SUCCESS;
}

uint32_t IFileSystemProxy::OpenDataStorageByCurrentProcess(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* storage = new IStorage("host");
    reply->moveHandle(storage->connect());

    return ERR_SUCCESS;

    // notes:
    // nn::fs::RomFsFileSystem::GetRequiredWorkingMemorySize with the handle from this, then reads 0x50 bytes with offset? 0
    // then adds qwords at file offsets 0x10, 0x20, 0x30, 0x40 together, and aligns up to 0x8
    // are those file offsets sizes of some kind? of various partitions?
    // looks like the header of a romFS. Does "host" refer to the switchOS?
}

uint32_t IFileSystemProxy::OpenRomStorage(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* storage = new IStorage("unk203");
    reply->moveHandle(storage->connect());

    return ERR_SUCCESS;

    // notes:
    // nn::fs::RomFsFileSystem::GetRequiredWorkingMemorySize with the handle from this, then reads 0x50 bytes with offset? 0
    // then adds qwords at file offsets 0x10, 0x20, 0x30, 0x40 together, and aligns up to 0x8
    // are those file offsets sizes of some kind? of various partitions?
    // looks like the header of a romFS. Who's romFS does this refer to?
}

// notes:
// nn::fs::QueryMountRomCacheSize calls OpenHost and unk203, then calls nn::fs::RomFsFileSystem::GetRequiredWorkingMemorySize for both, adding the results together

uint32_t IFileSystemProxy::GetGlobalAccessLogMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    reply->writeData(0); // TODO what are legal values?

    return ERR_SUCCESS;
}

} // namespace sf

} // namespace fssrv

} // namespace nn