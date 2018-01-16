#include "INvDrvServices.h"
#include "hle\ipc.h"
#include "log.h"
#include "mem.h"
#include "gpu\deviceHandleManager.h"
#include "debug\memDump.h"

namespace nns {

namespace nvdrv {

uint32_t INvDrvServices::Open(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    uint64_t addr = info->sendDescriptors[0].address;
    uint64_t size = info->sendDescriptors[0].size;
    std::string devName = "";
    for (uint64_t i = 0; i < size; i++) {
        devName += static_cast<char>(mem::read<uint8_t>(addr + i));
    }

    handle_t devHandle = gpu::handleman::connectToDevice(devName);

    logger::info("nvdrv open device: %s handle: 0x%08X", devName.c_str(), devHandle);

    struct {
        uint32_t fd;
        uint32_t error;
    } out;

    out.fd = devHandle;
    out.error = 0;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t INvDrvServices::Ioctl(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    // TODO x desc.
    uint32_t handle = mem::read<uint32_t>(info->rawData + 0x10);
    uint32_t cmd = mem::read<uint32_t>(info->rawData + 0x14);

    logger::info("Ioctl to 0x%08X of command 0x%08X", handle, cmd);

    std::vector<uint8_t> in;
    std::vector<uint8_t> out;

    // TODO officials games use staticInDescriptors, but libtransitor uses sendDescriptors.
    // type 0x21? (A + X)
    // TODO handle properly
    uint64_t addr = 0;
    uint64_t size = 0;
    if (info->staticInDescriptors.size() != 0 && info->staticInDescriptors[0].address != 0) {
        addr = info->staticInDescriptors[0].address;
        size = info->staticInDescriptors[0].size;
    } else {
        addr = info->sendDescriptors[0].address;
        size = info->sendDescriptors[0].size;
    }

    for (uint64_t i = 0; i < size; i++) {
        in.push_back(mem::read<uint8_t>(addr + i));
    }

    //logger::info("Ioctl buffer:");
    //debug::dumpMem(addr, size);

    // TODO better buffer transfer?
    uint32_t ioctlRet = gpu::handleman::sendIoctl(handle, cmd, in, out);

    for (uint64_t i = 0; i < info->recvDescriptors[0].size; i++) {
        mem::write<uint8_t>(info->recvDescriptors[0].address + i, 0);
    }
    if (out.size() != 0) {
        // type 0x22? (B + C)
        // TODO handle properly
        uint64_t outAddr = 0;
        if (info->staticOutDescriptors.size() != 0 && info->staticOutDescriptors[0].address != 0) {
            outAddr = info->staticOutDescriptors[0].address;
        } else {
            outAddr = info->recvDescriptors[0].address;
        }
        for (uint64_t i = 0; i < out.size(); i++) {
            mem::write<uint8_t>(outAddr + i, out[i]);
        }
    }

    struct {
        uint32_t error;
    } ret;

    ret.error = ioctlRet;

    reply->writeArgs(&ret);

    return ERR_SUCCESS;
}

uint32_t INvDrvServices::Close(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    // TODO args in
    struct {
        uint32_t error;
    } out;

    out.error = 0;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t INvDrvServices::Initialize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    // TODO args in, handles in
    struct {
        uint32_t error;
    } out;

    out.error = 0;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t INvDrvServices::QueryEvent(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    // TODO args in
    struct {
        uint32_t error;
    } out;

    out.error = 0;

    reply->writeArgs(&out);
    reply->copyHandle(0x1357);

    return ERR_SUCCESS;
}

uint32_t INvDrvServices::SetClientPID(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    // TODO args in
    struct {
        uint32_t error;
    } out;

    out.error = 0;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

} // namespace nvdrv

} // namespace nns