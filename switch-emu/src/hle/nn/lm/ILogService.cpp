#include "ILogService.h"
#include "ILogger.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace lm {

uint32_t ILogService::Initialize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint64_t unk;
    } args;

    ipc::readArgs(info, &args);

    logger::info("unk: %016llX", args.unk);

    Interface* logger = new ILogger();
    reply->moveHandle(logger->connect());

    return ERR_SUCCESS;
}

} // namespace lm

} // namespace nn