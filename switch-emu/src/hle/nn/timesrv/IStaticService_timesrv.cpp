#include "IStaticService.h"
#include "ISystemClock.h"
#include "ISteadyClock.h"
#include "ITimeZoneService.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace timesrv {

namespace detail {

namespace service {

uint32_t IStaticService::GetStandardUserSystemClock(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* clock = new ISystemClock();
    reply->moveHandle(clock->connect());

    return ERR_SUCCESS;
}

uint32_t IStaticService::GetStandardNetworkSystemClock(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* clock = new ISystemClock();
    reply->moveHandle(clock->connect());

    return ERR_SUCCESS;
}

uint32_t IStaticService::GetStandardSteadyClock(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* clock = new ISteadyClock();
    reply->moveHandle(clock->connect());

    return ERR_SUCCESS;
}

uint32_t IStaticService::GetTimeZoneService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* timeZone = new ITimeZoneService();
    reply->moveHandle(timeZone->connect());

    return ERR_SUCCESS;
}

} // namespace service

} // namespace detail

} // namespace timesrv

} // namespace nn