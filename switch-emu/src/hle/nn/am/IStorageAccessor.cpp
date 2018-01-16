#include "IStorageAccessor.h"
#include "hle\ipc.h"
#include "log.h"
#include "mem.h"

namespace nn {

namespace am {

namespace service {

uint32_t IStorageAccessor::GetSize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    reply->writeData(0x100); // TODO what is a good size? 0 does not seem to work
    // for PopLaunchParams, nn::account::detail::TryPopPreselectedUser seems to abort with < 0x87
    // the C buffer that is later passed to Read is 0x88 long, so that's probably the right size

    return ERR_SUCCESS;
}

uint32_t IStorageAccessor::Read(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO

    uint64_t addr = info->staticOutDescriptors[0].address;
    uint64_t size = info->staticOutDescriptors[0].size;

    // for nn::account::detail::TryPopPreselectedUser, it expects addr + 0 to be 0xQQQQQQ01C79497CA
    // and this passes addr + 8 and addr + 0x10 to nn::account::OpenPreselectedUser
    // nn::account::OpenPreselectedUser expects at least to be non-zero (ors them, then aborts if the result is zero)
    mem::write<uint32_t>(addr, 0xC79497CA);
    mem::write<uint8_t>(addr + 4, 0x1);
    mem::write<uint64_t>(addr + 8, 0xF);
    mem::write<uint64_t>(addr + 16, 0xF0);

    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn