#include <string>
#include <map>

#include "kernel.h"
#include "thread.h"
#include "hle\kernel\loader\loader.h"
#include "log.h"
#include "types.h"
#include "cpu.h"
#include "mem.h"
#include "hle\service.h"
#include "scheduler.h"
#include "memoryMapper.h"
#include "debug\debug.h"
#include "debug\stackTrace.h"
#include "platform_thread.h"
#include "hle\svc.h"
#include "clock.h"
#include "alarm.h"
#include "handleManager.h"

namespace kernel {

loader::Loader gLoader;
std::string gBasePath;
handle_t gCurrentProcess = INVALID_HANDLE;

void interruptHandler(uint64_t interrupts) {
    //logger::info("Interrupt of %016llX on core %016llX", interrupts, cpu::this_core::getCoreID());

    if (interrupts & cpu::RESET_INTERRUPT) {
        platform::exitThread(0);
    }

    if (interrupts & cpu::DEBUG_BREAK_INTERRUPT) {
        debug::debugBreakHandler();
    }

    if (interrupts != cpu::DEBUG_BREAK_INTERRUPT) {
        // Don't reschedule if there was only a debug break so we can debug the same thread
        kernel::scheduler::lockScheduler();
        kernel::scheduler::rescheduleNoLock();
        kernel::scheduler::unlockScheduler();
    }
}

void segFaultHandler(uint64_t addr) {
    uint64_t pc = cpu::this_core::getCore()->PC;
    logger::error("Segfault at %016llX accessing %016llX", pc, addr);

    debug::debugBreakHandler();
}

void illInstHandler() {
    uint64_t pc = cpu::this_core::getCore()->PC;
    logger::error("Illegal instruction at %016llX", pc);
    cpu::sendInterrupts(cpu::this_core::getCore()->coreNum, cpu::DEBUG_BREAK_INTERRUPT);
}

bool init() {
    cpu::setEntryPoint(cpuEntry);
    cpu::setInterruptHandler(interruptHandler);
    cpu::setSegFaultHandler(segFaultHandler);
    cpu::setIllInstHandler(illInstHandler);
    cpu::setSVCHandler(SVC::callSVC);

    service::init();
    clock::init();
    clock::start();
    alarm::init();

    return true;
}

void exit() {
    debug::resumeFromDebugBreak();
}

uint32_t* getCommandBuffer() {
    return reinterpret_cast<uint32_t*>(mem::translate(scheduler::getCurrentThread()->getTLS()));
}

void setGameCartPath(const std::string& path) {
    gBasePath = path;
}
std::string getGameCartPath() {
    return gBasePath;
}

bool launchGameCart() {
    if (gBasePath == "") {
        return false;
    }

    // TODO mappings for a bunch of regions of memory
    memmap::addMapping(0, 1llU << 36, memmap::MemoryType::UNMAPPED, mem::Permission::NONE);
    // TODO what is the actual size created?
    memmap::addMapping(0x700000000, 0x200 * 32, memmap::MemoryType::THEAD_LOCAL_STORAGE, mem::Permission::READ_WRITE);
    // TODO what is the actual stack size + location + type for the main stack?
    memmap::addMapping(0xE0000000, 0x10000000, memmap::MemoryType::NORMAL, mem::Permission::READ_WRITE);

    gLoader.setRootPath(gBasePath);

    if (!gLoader.loadGame()) {
        logger::error("Couldn't load file");
        return false;
    }

    memmap::printMappings();

    CreateProcessInfo info;
    gCurrentProcess = handleman::createHandle(ObjectPtr<KProcess>(new KProcess(info)));

    return true;
}

void stopGame() {
    // TODO
    // TODO should be able to forcibly remove all copies of the handle
    handleman::closeHandle(gCurrentProcess);
    gCurrentProcess = INVALID_HANDLE;
}

bool isGameRunning() {
    return gCurrentProcess != INVALID_HANDLE;
}

ObjectPtr<KProcess> getCurrentProcess() {
    return object_pointer_cast<KProcess>(handleman::lookupObject(gCurrentProcess));
}

handle_t getCurrentProcessHandle() {
    return gCurrentProcess;
}

void cpuEntry() {
    if (cpu::this_core::getCoreID() == 0) {
        logger::info("Starting main thread");
        handle_t coreThread = scheduler::createThread(gLoader.getProgramEntry(), 0, 0x00000000F0000000, 0, CoreAffinity::CORE0);
        scheduler::getThread(coreThread)->context.GPRegs[0] = 0; // some exception pointer, should be 0
        scheduler::getThread(coreThread)->context.GPRegs[1] = coreThread;
        scheduler::startThread(coreThread);

        cpu::sendInterrupts(0, cpu::GENERIC_INTERRUPT);
    }

    while (true) {
        //logger::info("Core %d idling", cpu::this_core::getCoreID());
        cpu::this_core::waitForInterrupt();
        //logger::info("Core %d resuming", cpu::this_core::getCoreID());
        cpu::this_core::resume();
    }
}

std::string getBasePath() {
    return gBasePath;
}

kernel::loader::Loader& getLoader() {
    return gLoader;
}

GraphicsDriver* gGraphicsDriver;

void setGraphicsDriver(GraphicsDriver* driver) {
    gGraphicsDriver = driver;
}

GraphicsDriver* getGraphicsDriver() {
    return gGraphicsDriver;
}

} // namespace kernel