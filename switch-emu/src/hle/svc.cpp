#include "svc.h"
#include "cpu.h"
#include "mem.h"
#include "log.h"
#include "hle\service.h"
#include "kernel\kernel.h"
#include "kernel\thread.h"
#include "kernel\scheduler.h"
#include "hle\ipc.h"
#include "kernel\memoryMapper.h"
#include "kernel\handleManager.h"
#include "hle\kernel\types\KProcess.h"
#include "hle\kernel\types\KTransferMemory.h"

namespace SVC {

// TODO helper template functions

void svcSetHeapSize(cpu::Core* core) {
    // TODO
    uint32_t size = static_cast<uint32_t>(core->GPRegs[1]);
    logger::info("svcSetHeapSize called with size %016llX", size);

    // TODO actual mapping
    // TODO shrink/expand current mapping if called >1 times
    kernel::memmap::addMapping(0xaa0000000, size, kernel::memmap::MemoryType::HEAP, mem::Permission::READ_WRITE);
    kernel::memmap::printMappings();

    core->GPRegs[0] = 0;
    // TODO what is a good value for the head addr?
    core->GPRegs[1] = 0xaa0000000;
}

void svcSetMemoryAttribute(cpu::Core* core) {
    // TODO
    uint64_t addr = core->GPRegs[0];
    uint64_t size = core->GPRegs[1];
    uint64_t state0 = core->GPRegs[2];
    uint64_t state1 = core->GPRegs[3];

    logger::info("svcSetMemoryAttribute for 0x%016llX with size: 0x%016llX, state0: 0x%08llX, state1: 0x%08llX", addr, size, state0, state1);

    core->GPRegs[0] = 0;
}

void svcMapMemory(cpu::Core* core) {
    // TODO
    uint64_t dstAddr = core->GPRegs[0];
    uint64_t srcAddr = core->GPRegs[1];
    uint64_t size = core->GPRegs[2];

    logger::info("svcMapMemory from %016llX to %016llX with size: %016llX", srcAddr, dstAddr, size);

    // TODO what is the actual mapping made?
    kernel::memmap::addMapping(dstAddr, size, kernel::memmap::MemoryType::MAPPED_MEM, mem::Permission::ALL);
    kernel::memmap::printMappings();

    for (uint64_t i = 0; i < size; i++) {
        mem::write<uint8_t>(dstAddr + i, mem::read<uint8_t>(srcAddr + i));
    }

    core->GPRegs[0] = 0;
}

void svcQueryMemory(cpu::Core* core) {
    // TODO keep track of more memory regions (stack, tls)
    uint64_t memInfo = core->GPRegs[0];
    uint64_t addr = core->GPRegs[2];

    logger::info("svcQueryMemory for addr: %016llX", addr);

    kernel::memmap::MemoryMapping mapping = kernel::memmap::findMapping(addr);
    logger::info("Found memory region at %016llX with size %016llX of type %08X with permissions %02X",
        mapping.base, mapping.size, mapping.state, mapping.permissions);

    mem::write<uint64_t>(memInfo, mapping.base);
    mem::write<uint64_t>(memInfo + 8, mapping.size);
    mem::write<uint64_t>(memInfo + 16, static_cast<uint64_t>(mapping.state.type)); // TODO just the type, or entire state? rtld checks the entire word, so prob just the type
    mem::write<uint64_t>(memInfo + 24, static_cast<uint64_t>(mapping.permissions)); // permissions? rtld checks this for 0x5
    // unknown if more

    core->GPRegs[0] = 0;
    core->GPRegs[1] = 0;
}

void svcExitProcess(cpu::Core* core) {
    // TODO actually do this once we get processes
    cpu::halt();
}

void svcCreateThread(cpu::Core* core) {
    // TODO errors
    uint64_t entry = core->GPRegs[1];
    uint64_t arg = core->GPRegs[2];
    uint64_t stack = core->GPRegs[3];
    uint32_t priority = static_cast<uint32_t>(core->GPRegs[4]);
    kernel::CoreAffinity cores = static_cast<kernel::CoreAffinity>(core->GPRegs[5]);

    handle_t thread = kernel::scheduler::createThread(entry, arg, stack, priority, cores);

    logger::info("svcCreateThread: entry = %016llX, arg = %016llX, stack = %016llX, priority = %08X, cores = %08X", entry, arg, stack, priority, cores);

    core->GPRegs[0] = 0;
    core->GPRegs[1] = thread;
}

void svcStartThread(cpu::Core* core) {
    // TODO errors
    uint32_t handle = static_cast<uint32_t>(core->GPRegs[0]);
    kernel::scheduler::startThread(handle);

    core->GPRegs[0] = 0;
}

void svcExitThread(cpu::Core* core) {
    kernel::scheduler::exitThread();
}

void svcSleepThread(cpu::Core* core) {
    uint64_t nano = core->GPRegs[0];
    if (nano == 0) {
        kernel::scheduler::yieldThread();
    } else {
        kernel::scheduler::sleepThread(nano);
    }
}

void svcGetThreadPriority(cpu::Core* core) {
    // TODO
    uint64_t addr = core->GPRegs[0];
    uint64_t handle = core->GPRegs[1];

    logger::info("svcGetThreadPriority for %016llX to %016llX", handle, addr);

    mem::write<uint64_t>(addr, 0x2C);

    core->GPRegs[0] = 0;
}

void svcSetThreadPriority(cpu::Core* core) {
    // TODO
    uint64_t handle = core->GPRegs[0];
    uint64_t prio = core->GPRegs[1];

    logger::info("svcSetThreadPriority for %016llX to prio %016llX", handle, prio);

    core->GPRegs[0] = 0;
}

void svcSetThreadCoreMask(cpu::Core* core) {
    // TODO
    uint64_t handle = core->GPRegs[0];
    uint64_t in1 = core->GPRegs[1];
    uint64_t in2 = core->GPRegs[1];

    logger::info("svcSetThreadCoreMask for %016llX with in1: %016llX, in2: %016llX", handle, in1, in2);

    core->GPRegs[0] = 0;
}

void svcGetCurrentProcessorNumber(cpu::Core* core) {
    core->GPRegs[0] = cpu::this_core::getCoreID();
}

void svcMapSharedMemory(cpu::Core* core) {
    // TODO
    uint64_t handle = core->GPRegs[0];
    uint64_t addr = core->GPRegs[1];
    uint64_t size = core->GPRegs[2];
    uint64_t perm = core->GPRegs[3];

    logger::info("svcMapSharedMemory with handle %016X to %016llX with size: %016llX and perm: %016llX", handle, addr, size, perm);

    // TODO what is the actual mapping made?
    kernel::memmap::addMapping(addr, size, kernel::memmap::MemoryType::SHARED_MEM_BLOCK, mem::Permission::ALL);
    kernel::memmap::printMappings();

    core->GPRegs[0] = 0;
}

void svcCreateTransferMemory(cpu::Core* core) {
    uint64_t addr = core->GPRegs[1];
    uint64_t size = core->GPRegs[2];
    uint64_t perm = core->GPRegs[3];

    logger::info("svcCreateTransferMemory for %016llX with size: %016llX and perm: %016llX", addr, size, perm);

    handle_t memHandle = kernel::handleman::createHandle(ObjectPtr<KTransferMemory>(new KTransferMemory(addr, size, perm, ObjectPtr<KProcess>(nullptr))));

    core->GPRegs[0] = 0;
    core->GPRegs[1] = memHandle; // TODO actual handle
}

void svcCloseHandle(cpu::Core* core) {
    // TODO actually close handles
    handle_t handle = static_cast<handle_t>(core->GPRegs[0]);
    kernel::handleman::closeHandle(handle);

    core->GPRegs[0] = 0;
}

void svcResetSignal(cpu::Core* core) {
    // TODO actually reset signal
    logger::info("svcResetSignal for 0x%08X", core->GPRegs[0]);

    core->GPRegs[0] = 0;
}

void svcWaitSynchronization(cpu::Core* core) {
    // TODO actually wait
    uint64_t x0 = core->GPRegs[0]; // TODO is this an actual arg?
    uint64_t handlePtr = core->GPRegs[1];
    uint64_t numHandles = core->GPRegs[2];
    uint64_t timeout = core->GPRegs[3];

    std::vector<handle_t> handles;
    for (uint64_t i = 0; i < numHandles; i++) {
        // TODO size of handle_t
        handles.push_back(static_cast<handle_t>(mem::read<uint32_t>(handlePtr + 4 * i)));
    }

    logger::info("svcWaitSynchronization, handles[]: %016llX, num: %016llX, timeout: %016llX (X0: %016llX); handles:", handlePtr, numHandles, timeout, x0);
    for (const handle_t& handle : handles) {
        logger::info("\t%08X", handle);
    }

    handle_t thisThread = kernel::scheduler::getCurrentHandle();
    kernel::scheduler::getThread(thisThread)->wakeupCallback = [](ObjectPtr<kernel::Thread> thread) {
        thread->cancelAllWaitObjects();
    };

    for (const handle_t& handle : handles) {
        // TODO remove (use it to avoid fake handles)
        if (handle > 0x100) {
            continue;
        }
        ObjectPtr<KSynchronizationObject> obj = kernel::object_pointer_cast<KSynchronizationObject>(kernel::handleman::lookupObject(handle));
        obj->sync(thisThread);
    }

    core->GPRegs[0] = 0;
    core->GPRegs[1] = 0; // TODO handle index?
}

void svcSignalProcessWideKey(cpu::Core* core) {
    uint64_t ptr = core->GPRegs[0];
    uint64_t value = core->GPRegs[1];

    logger::info("svcSignalProcessWideKey called with ptr: %016llX and value: %016llX", ptr, value);

    uint64_t val = mem::read<uint64_t>(ptr);
    mem::write<uint64_t>(ptr, val + 1);

    core->GPRegs[0] = 0;
}

void svcConnectToPort(cpu::Core* core) {
    // TODO errors
    char* name = reinterpret_cast<char*>(mem::translate(core->GPRegs[1]));

    auto iter = service::gNamedPorts.find(name);
    if (iter == service::gNamedPorts.end()) {
        logger::error("Program tried to connect to unknown port: %s", name);
        core->GPRegs[0] = 1; // TODO actual error
        return;
    }

    logger::info("svcConnectToPort to %s", name);

    core->GPRegs[0] = 0;
    core->GPRegs[1] = iter->second->connect();
}

void svcSendSyncRequest(cpu::Core* core) {
    core->GPRegs[0] = ipc::doIpcCall(static_cast<handle_t>(core->GPRegs[0]), kernel::scheduler::getCurrentThread()->getTLS(), 1024 * 1024); // TODO assuming 1Mb
}

void svcSendSyncRequestWithUserBuffer(cpu::Core* core) {
    core->GPRegs[0] = ipc::doIpcCall(static_cast<handle_t>(core->GPRegs[2]), core->GPRegs[0], core->GPRegs[1]);
}

void svcBreak(cpu::Core* core) {
    // TODO abort emulation
    logger::error("svcBreak");
    logger::error("X0:%016llX", core->GPRegs[0]);
    logger::error("X1:%016llX", core->GPRegs[1]);
    logger::error("X2:%016llX", core->GPRegs[2]);

    cpu::sendInterrupts(cpu::this_core::getCoreID(), cpu::DEBUG_BREAK_INTERRUPT);
}

void svcGetThreadId(cpu::Core* core) {
    uint64_t handle = core->GPRegs[1]; // wiki is wrong when it says X0

    logger::info("svcGetThreadId for %016llX", handle);

    core->GPRegs[0] = 0;
    core->GPRegs[1] = 1; // TODO actual id
}

void svcGetInfo(cpu::Core* core) {
    logger::info("svcGetInfo");
    logger::info("X0:%016llX", core->GPRegs[0]);
    logger::info("X1:%016llX", core->GPRegs[1]);
    logger::info("X2:%016llX", core->GPRegs[2]);
    logger::info("X2:%016llX", core->GPRegs[3]);

    uint64_t addr = core->GPRegs[0];
    uint64_t infoType = core->GPRegs[1];
    //uint64_t handle = core->GPRegs[2];
    uint64_t subType = core->GPRegs[3];

    if (infoType == 0 && subType == 0) {
        // AllowedCpuIdBitmask
        // TODO
        core->GPRegs[1] = 0xF;
    } else if (infoType == 2 && subType == 0) {
        // MapRegionBaseAddr
        // TODO
        core->GPRegs[1] = 0x21000000;
    } else if (infoType == 3 && subType == 0) {
        // MapRegionSize
        // TODO
        core->GPRegs[1] = 0x10000000;
    } else if (infoType == 4 && subType == 0) {
        // HeapRegionBaseAddr
        // TODO
        core->GPRegs[1] = 0xaa0000000;
    } else if (infoType == 5 && subType == 0) {
        // HeapRegionSize
        // TODO
        core->GPRegs[1] = 0;
    } else if (infoType == 6 && subType == 0) {
        // TotalMemoryUsage
        // TODO
        // A lot of games do setHeapSize(TotalMemoryUsage - TotalHeapUsage)
        // Is this Total usable memory?
        core->GPRegs[1] = 0x80000000;
    } else if (infoType == 7 && subType == 0) {
        // TotalHeapUsage
        // TODO
        core->GPRegs[1] = 0x100000;
    } else if (infoType == 11) {
        // random from TRNG
        if (subType == 0) {
            // core 0?
            core->GPRegs[1] = 0;
        } else if (subType == 1) {
            // core 1?
            core->GPRegs[1] = 1;
        } else if (subType == 2) {
            // core 2?
            core->GPRegs[1] = 2;
        } else if (subType == 3) {
            // core 3?
            core->GPRegs[1] = 3;
        } else {
            logger::warn("Unhandled svcGetInfo infoType: %016llX, subType: %016llX", infoType, subType);
        }
    } else if (infoType == 12 && subType == 0) {
        // AddressSpaceBaseAddr
        core->GPRegs[1] = 0x8000000;
    } else if (infoType == 13 && subType == 0) {
        // AddressSpaceSize
        core->GPRegs[1] = 0x7ff8000000;
    } else {
        logger::error("Unhandled svcGetInfo infoType: %016llX, subType: %016llX", infoType, subType);

        core->GPRegs[0] = 1;
        return;
    }

    core->GPRegs[0] = 0;
}

void svcOutputDebugString(cpu::Core* core) {
    // TODO errors
    char* str = reinterpret_cast<char*>(mem::translate(core->GPRegs[0]));
    uint64_t size = core->GPRegs[1];

    //logger::info("ptr:%016llX, size:%016llX", (uint64_t)str, size);

    if (str != nullptr) {
        uint64_t index = 0;
        while ((index <= size) && (str[index] != '\0')) {
            printf("%c", str[index++]);
        }
    }
    printf("\n");
}

const std::array<void(*)(cpu::Core*), 0x80> svcFunctionTable = {
    nullptr, // none
    svcSetHeapSize,
    nullptr, // svcSetMemoryPermission
    svcSetMemoryAttribute, 
    svcMapMemory,
    nullptr, // svcUnmapMemory
    svcQueryMemory,
    svcExitProcess,
    svcCreateThread,
    svcStartThread,
    svcExitThread,
    svcSleepThread,
    svcGetThreadPriority,
    svcSetThreadPriority,
    nullptr, // svcGetThreadCoreMask
    svcSetThreadCoreMask,
    svcGetCurrentProcessorNumber,
    nullptr, // svcSignalEvent
    nullptr, // svcClearEvent
    svcMapSharedMemory,
    nullptr, // svcUnmapSharedMemory
    svcCreateTransferMemory,
    svcCloseHandle,
    svcResetSignal,
    svcWaitSynchronization,
    nullptr, // svcCancelSynchronization
    nullptr, // svcArbitrateLock
    nullptr, // svcArbitrateUnlock
    nullptr, // svcWaitProcessWideKeyAtomic
    svcSignalProcessWideKey,
    nullptr, // svcGetSystemTick
    svcConnectToPort,
    nullptr, // svcSendSyncRequestLight
    svcSendSyncRequest,
    svcSendSyncRequestWithUserBuffer, // svcSendSyncRequestWithUserBuffer
    nullptr, // svcSendAsyncRequestWithUserBuffer
    nullptr, // svcGetProcessId
    svcGetThreadId,
    svcBreak,
    svcOutputDebugString,
    nullptr, // svcReturnFromException
    svcGetInfo, // svcGetInfo
    nullptr, // svcFlushEntireDataCache
    nullptr, // svcFlushDataCache
    nullptr, // svcMapPhysicalMemory
    nullptr, // svcUnmapPhysicalMemory
    nullptr, // ?
    nullptr, // svcGetLastThreadInfo
    nullptr, // svcGetResourceLimitLimitValue
    nullptr, // svcGetResourceLimitCurrentValue
    nullptr, // svcSetThreadActivity
    nullptr, // svcGetThreadContext3
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // svcDumpInfo
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // svcCreateSession
    nullptr, // svcAcceptSession
    nullptr, // svcReplyAndReceiveLight
    nullptr, // svcReplyAndReceive
    nullptr, // svcReplyAndReceiveWithUserBuffer
    nullptr, // svcCreateEvent
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // ?
    nullptr, // svcSleepSystem
    nullptr, // svcReadWriteRegister
    nullptr, // svcSetProcessActivity
    nullptr, // svcCreateSharedMemory
    nullptr, // svcMapTransferMemory
    nullptr, // svcUnmapTransferMemory
    nullptr, // svcCreateInterruptEvent
    nullptr, // svcQueryPhysicalAddress
    nullptr, // svcQueryIoMapping
    nullptr, // svcCreateDeviceAddressSpace
    nullptr, // svcAttachDeviceAddressSpace
    nullptr, // svcDetachDeviceAddressSpace
    nullptr, // svcMapDeviceAddressSpaceByForce
    nullptr, // svcMapDeviceAddressSpaceAligned
    nullptr, // svcMapDeviceAddressSpace
    nullptr, // svcUnmapDeviceAddressSpace
    nullptr, // svcInvalidateProcessDataCache
    nullptr, // svcStoreProcessDataCache
    nullptr, // svcFlushProcessDataCache
    nullptr, // svcDebugActiveProcess
    nullptr, // svcBreakDebugProcess
    nullptr, // svcTerminateDebugProcess
    nullptr, // svcGetDebugEvent
    nullptr, // svcContinueDebugEvent
    nullptr, // svcGetProcessList
    nullptr, // svcGetThreadList
    nullptr, // svcGetDebugThreadContext
    nullptr, // svcSetDebugThreadContext
    nullptr, // svcQueryDebugProcessMemory
    nullptr, // svcReadDebugProcessMemory
    nullptr, // svcWriteDebugProcessMemory
    nullptr, // svcSetHardwareBreakPoint
    nullptr, // svcGetDebugThreadParam
    nullptr, // ?
    nullptr, // ?
    nullptr, // svcCreatePort
    nullptr, // svcManageNamedPort
    nullptr, // svcConnectToPort
    nullptr, // svcSetProcessMemoryPermission
    nullptr, // svcMapProcessMemory
    nullptr, // svcUnmapProcessMemory
    nullptr, // svcQueryProcessMemory
    nullptr, // svcMapProcessCodeMemory
    nullptr, // svcUnmapProcessCodeMemory
    nullptr, // svcCreateProcess
    nullptr, // svcStartProcess
    nullptr, // svcTerminateProcess
    nullptr, // svcGetProcessInfo
    nullptr, // svcCreateResourceLimit
    nullptr, // svcSetResourceLimitLimitValue
    nullptr, // svcCallSecureMonitor
};

void callSVC(uint64_t id) {
    auto core = cpu::this_core::getCore();
    auto func = svcFunctionTable[id];
    logger::info("Game called SVC 0x%02X at 0x%016llX", id, core->PC);
    if (func) {
        func(core);
    } else {
        logger::error("Unrecognized syscall number: 0x%02X at 0x%016llX", id, core->PC);
    }
}

} // namespace svc