#include "ISettingsServer.h"
#include "hle\ipc.h"
#include "log.h"
#include "mem.h"

namespace nn {

namespace settings {

uint32_t ISettingsServer::GetAvailableLanguageCodes(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        int32_t unk;
    } out;

    out.unk = 0x4; // TODO find legal values

    reply->writeArgs(&out);

    uint64_t addr = info->staticOutDescriptors[0].address;

    mem::write<uint64_t>(addr, 0x0); // TODO find legal values
    mem::write<uint64_t>(addr + 0x8, 0x1);
    mem::write<uint64_t>(addr + 0x10, 0x2);
    mem::write<uint64_t>(addr + 0x18, 0x3);

    return ERR_SUCCESS;
}

} // namespace settings

} // namespace nn