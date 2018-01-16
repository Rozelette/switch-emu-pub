#include <list>
#include <thread>
#include <mutex>
#include <atomic>

#include "clock.h"
#include "platform_thread.h"

// TODO should cpu have a periodic interupt that does this function?
namespace kernel::clock {

std::list<ClockCallbackFunc> gCallbacks;
std::thread gClockThread;
std::mutex gClockLock;
std::atomic<bool> gShouldRun;

void init() {
    gCallbacks.clear();
}

void workLoop() {
    while (gShouldRun.load()) {
        // TODO can we be a bit more loose on holding this lock?
        gClockLock.lock();
        for (auto callback = gCallbacks.begin(); callback != gCallbacks.end();) {
            bool shouldContinue = (*callback)();
            if (shouldContinue) {
                ++callback;
            } else {
                callback = gCallbacks.erase(callback);
            }
        }
        gClockLock.unlock();
        // TODO is this a good rate (1kHz)? For scheduling purposes, it seems a bit low
        // TODO do a dynamic delay based on execution time of the callbacks so the rate is constant
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void start() {
    gShouldRun.store(true);
    gClockThread = std::thread(workLoop);

    platform::setThreadName(&gClockThread, "Clock Thread");
}

void stop() {
    gShouldRun.store(false);
}

void join() {
    stop();
    if (gClockThread.joinable()) {
        gClockThread.join();
    }
}

void addCallback(ClockCallbackFunc func) {
    std::lock_guard<std::mutex> lock(gClockLock);
    gCallbacks.push_back(func);
}

} // kernel::clock