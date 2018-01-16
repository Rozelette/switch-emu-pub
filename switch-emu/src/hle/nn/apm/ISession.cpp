#include "ISession.h"
#include "hle\nn\apm\apm.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace apm {

uint32_t ISession::SetPerformanceConfiguration(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::apm::PerformanceMode mode;
        nn::apm::PerformanceConfiguration configuration;
    } args;

    ipc::readArgs(info, &args);

    logger::info("mode: %08llX", args.mode);
    logger::info("configuration: %08llX", args.configuration);

    return ERR_SUCCESS;
}

} // namespace apm

} // namespace nn