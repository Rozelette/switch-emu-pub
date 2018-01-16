#include <chrono>

#include "alarm.h"
#include "clock.h"
#include "time.h"
#include "cpu.h"

namespace kernel::alarm {

std::vector<Alarm> gAlarms;

bool Alarm::update(uint64_t time) {
    std::lock_guard<std::mutex> lock(mutex);
    if ((time >= triggerTime) && !hasTriggered) {
        hasTriggered = true;
        for (auto& handle : waitingThreads) {
            scheduler::queueThreadNoLock(handle);
        }
        waitingThreads.clear();
        cpu::sendInterrupts(0, cpu::GENERIC_INTERRUPT);
    }

    return hasTriggered;
}

void Alarm::wait(handle_t handle) {
    std::lock_guard<std::mutex> lock(mutex);
    if (hasTriggered) {
        scheduler::queueThreadNoLock(handle);
    } else {
        waitingThreads.push_back(handle);
        scheduler::getThread(handle)->wait();
    }
    cpu::sendInterrupts(0, cpu::GENERIC_INTERRUPT);
}

Alarm::~Alarm() {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& handle : waitingThreads) {
        scheduler::queueThreadNoLock(handle);
    }
}

bool updateAlarms() {
    // TODO thread safe
    uint64_t now = kernel::time::getTimeInNanoseconds();
    for (Alarm& alarm : gAlarms) {
        alarm.update(now);
    }

    return true;
}

void init() {
    clock::addCallback(updateAlarms);
}

Alarm& createAlarm(uint64_t time) {
    // TODO thread safe
    // TODO Alarms are never destroyed. Once we start using actual handles, this should be fixed
    gAlarms.push_back(std::move(Alarm(time)));
    return gAlarms.back();
}

} // kernel::alarm

