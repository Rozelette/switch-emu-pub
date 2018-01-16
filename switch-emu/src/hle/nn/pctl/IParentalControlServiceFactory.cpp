#include "IParentalControlServiceFactory.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace pctl {

namespace detail {

namespace ipc {

uint32_t IParentalControlServiceFactory::CreateService(::ipc::IpcInfo* info, ::ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint64_t unk;
    } args;

    ::ipc::readArgs(info, &args);

    logger::info("unk: 0x%016llX", args.unk);

    return ERR_SUCCESS;
}

} // namespace ipc

} // namespace detail

} // namespace pctl

} // namespace nn