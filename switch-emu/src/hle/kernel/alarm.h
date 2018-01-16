#pragma once
#include <stdint.h>
#include <vector>
#include <mutex>

#include "hle\kernel\types\KAutoObject.h"
#include "hle\kernel\thread.h"
#include "scheduler.h"

// TODO replace all of this with actual K* types eventually
namespace kernel::alarm {

class Alarm {
public:
    Alarm(uint64_t triggerTime_) : triggerTime(triggerTime_) {}
    Alarm(const Alarm&& other) : triggerTime(other.triggerTime) {}

    Alarm::~Alarm();

    bool update(uint64_t time);

    void wait(handle_t handle);

private:
    uint64_t triggerTime = 0;
    bool hasTriggered = false;
    std::vector<handle_t> waitingThreads;
    std::mutex mutex;
};

void init();

Alarm& createAlarm(uint64_t time);

} // kernel::alarm

