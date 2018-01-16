#include "ISystemClock.h"
#include "hle\nn\time\time.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace timesrv {

namespace detail {

namespace service {

uint32_t ISystemClock::GetCurrentTime(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::time::PosixTime posixTime;
    } out;

    out.posixTime = 0x12345678; // TODO actual time

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

} // namespace service

} // namespace detail

} // namespace timesrv

} // namespace nn