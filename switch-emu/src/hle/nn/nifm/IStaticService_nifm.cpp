#include "IStaticService.h"
#include "IGeneralService.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace nifm {

namespace detail {

uint32_t IStaticService::CreateGeneralServiceOld(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* generalService = new IGeneralService();
    reply->moveHandle(generalService->connect());

    return ERR_SUCCESS;
}

} // namespace detail

} // namespace nifm

} // namespace nn