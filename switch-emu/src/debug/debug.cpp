#include <atomic>
#include <condition_variable>
#include <mutex>

#include "debug.h"
#include "debugUI.h"
#include "analysis.h"
#include "cpu.h"
#include "stackTrace.h"

namespace debug {

std::atomic<uint64_t> gPausingCores;
std::atomic<uint64_t> gResumingCores;
std::atomic<bool> gIsPaused;
uint64_t gBreakInitiator;
std::condition_variable gResumeIndicator;
std::mutex gDebugBreakMutex;
bool gSingleStepping;

enum CorePausedState {
    NO_CORES = 0,
    ALL_CORES = 0xF
};

void init() {
    analysis::init();
    // TODO should this be done in the graphics thread?
    ui::init();
}

void debugBreakHandler() {
    std::unique_lock<std::mutex> lock(gDebugBreakMutex);

    uint64_t coreID = cpu::this_core::getCoreID();
    uint64_t coreBit = 1llU << coreID;

    if ((coreID == 0) && !gSingleStepping) {
        printStackTrace();
    }
    gSingleStepping = false;

    uint64_t alreadyPausing = gPausingCores.fetch_or(coreBit);

    if (alreadyPausing == CorePausedState::NO_CORES) {
        // we are the initiating core, so let's break the others
        for (auto i = 0; i < cpu::NUM_CORES; i++) {
            if (i == coreID) {
                continue;
            }
            cpu::sendInterrupts(i, cpu::DEBUG_BREAK_INTERRUPT);
        }
        gBreakInitiator = coreID;
        gResumingCores.store(0);
    }

    if ((alreadyPausing | coreBit) == CorePausedState::ALL_CORES) {
        gIsPaused.store(true);
        // Wait until all cores have stopped before disabling single step mode
        // so a core doesn't execute more than one instruction
        cpu::setSingleStepMode(false);
        gPausingCores.store(0);
    }

    while (gIsPaused.load()) {
        gResumeIndicator.wait(lock);
    }

    // If we hit another breakpoint while debug, we want to clear it so we
    // don't immediate break again
    cpu::this_core::clearInterrupts(cpu::DEBUG_BREAK_INTERRUPT);

    // Wait until all cores are ready to proceed so we don't have one core
    // breaking while another is still resuming
    if ((gResumingCores.fetch_or(coreBit) | coreBit) == CorePausedState::ALL_CORES) {
        // we are the last core to resume
        gResumeIndicator.notify_all();
    } else {
        while ((gResumingCores.load() | coreBit) != CorePausedState::ALL_CORES) {
            gResumeIndicator.wait(lock);
        }
    }
}

void resumeFromDebugBreak() {
    bool wasPaused = gIsPaused.exchange(false);

    if (wasPaused) {
        gResumeIndicator.notify_all();
    }
}

void resumeFromDebugBreakForSingleStep() {
    gSingleStepping = true;
    resumeFromDebugBreak();
}

} // namespace debug