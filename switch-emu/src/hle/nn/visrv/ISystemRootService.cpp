#include "ISystemRootService.h"
#include "IApplicationDisplayService.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace visrv {

namespace sf {

uint32_t ISystemRootService::GetDisplayService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint32_t unk;
    } args;

    ipc::readArgs(info, &args);

    logger::info("unk: %08X", args.unk);

    Interface* displayService = new IApplicationDisplayService();
    reply->moveHandle(displayService->connect());

    return ERR_SUCCESS;
}

} // namespace sf

} // namespace visrv

} // namespace nn