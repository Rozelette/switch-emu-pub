#include "ipc.h"
#include "log.h"
#include "mem.h"
#include "hle\service.h"
#include "bitutils.h"
#include "debug\memDump.h"
#include "kernel\kernel.h"
#include "kernel\scheduler.h"

#include "cpu.h"

namespace ipc {

uint64_t doIpcCall(handle_t handle, uint64_t buffer, uint64_t size) {
    // TODO errors
    IpcInfo info;
    // TODO safe?
    memset(&info, 0, sizeof(IpcInfo));

    info.buffer = buffer;

    uint32_t word0 = mem::read<uint32_t>(buffer + 0);
    uint32_t word1 = mem::read<uint32_t>(buffer + 4);
    uint64_t type = bitRange<0, 15>(word0);
    uint64_t numStaticIn = bitRange<16, 19>(word0);
    uint64_t numSend = bitRange<20, 23>(word0);
    uint64_t numRecv = bitRange<24, 27>(word0);
    uint64_t numTrans = bitRange<28, 31>(word0);
    uint64_t wordCount = bitRange<0, 9>(word1); // count in u32s, including padding and C descriptor sizes
    uint64_t cFlags = bitRange<10, 13>(word1);
    bool enableHandleDescriptor = testBit<31>(word1);

    uint64_t offset = 8;

    uint64_t numHandlesToCopy = 0;
    uint64_t numHandlesToMove = 0;

    bool sendPid = false;

    if (enableHandleDescriptor) {
        uint32_t word2 = mem::read<uint32_t>(buffer + 8);
        sendPid = testBit<0>(word2);
        numHandlesToCopy = bitRange<1, 4>(word2);
        numHandlesToMove = bitRange<5, 8>(word2);

        offset += 4;
    }

    if (sendPid) {
        info.pid = mem::read<uint64_t>(buffer + 12);

        offset += 8;
    }

    for (uint64_t i = 0; i < numHandlesToCopy; i++) {
        handle_t copiedHandle = static_cast<handle_t>(mem::read<uint32_t>(buffer + offset));
        if (copiedHandle == CURRENT_PROCESS) {
            copiedHandle = kernel::getCurrentProcessHandle();
        } else if (copiedHandle == CURRENT_THREAD) {
            copiedHandle = kernel::scheduler::getCurrentHandle();
        }
        info.copiedHandles.push_back(copiedHandle);
        logger::info("Copied handle %08X", copiedHandle);
        offset += 4;
    }
    for (uint64_t i = 0; i < numHandlesToMove; i++) {
        handle_t movedHandle = static_cast<handle_t>(mem::read<uint32_t>(buffer + offset));
        info.movedHandles.push_back(movedHandle);
        logger::info("Moved handle %08X", movedHandle);
        offset += 4;
    }

    for (uint64_t i = 0; i < numStaticIn; i++) {
        // TODO
        uint64_t descWord0 = mem::read<uint32_t>(buffer + offset);
        uint64_t descWord1 = mem::read<uint32_t>(buffer + offset + 4);
        StaticInDescriptor desc;
        desc.counter = 0; // TODO
        desc.address = descWord1 | (bitRange<12, 15>(descWord0) << 32) | (bitRange<6, 8>(descWord0) << 36);
        desc.size = bitRange<16, 31>(descWord0);

        logger::info("X: counter:%u, addr:%016llX, size:%016llX", desc.counter, desc.address, desc.size);

        info.staticInDescriptors.push_back(desc);

        offset += 8;
    }

    for (uint64_t i = 0; i < numSend; i++) {
        // TODO
        uint64_t descWord0 = mem::read<uint32_t>(buffer + offset);
        uint64_t descWord1 = mem::read<uint32_t>(buffer + offset + 4);
        uint64_t descWord2 = mem::read<uint32_t>(buffer + offset + 8);

        SendDescriptor desc;
        desc.flags = bitRange<0, 1>(descWord2);
        desc.address = descWord1 | (bitRange<28, 31>(descWord2) << 32) | (bitRange<2, 4>(descWord2) << 36);
        desc.size = descWord0 | (bitRange<24, 27>(descWord2) << 32);

        logger::info("A: flags:%u, addr:%016llX, size:%016llX", desc.flags, desc.address, desc.size);

        info.sendDescriptors.push_back(desc);

        offset += 12;
    }

    for (uint64_t i = 0; i < numRecv; i++) {
        // TODO
        uint64_t descWord0 = mem::read<uint32_t>(buffer + offset);
        uint64_t descWord1 = mem::read<uint32_t>(buffer + offset + 4);
        uint64_t descWord2 = mem::read<uint32_t>(buffer + offset + 8);

        RecvDescriptor desc;
        desc.flags = bitRange<0, 1>(descWord2);
        desc.address = descWord1 | (bitRange<28, 31>(descWord2) << 32) | (bitRange<2, 4>(descWord2) << 36);
        desc.size = descWord0 | (bitRange<24, 27>(descWord2) << 32);

        logger::info("B: flags:%u, addr:%016llX, size:%016llX", desc.flags, desc.address, desc.size);

        info.recvDescriptors.push_back(desc);

        offset += 12;
    }

    for (uint64_t i = 0; i < numTrans; i++) {
        // TODO
        uint64_t descWord0 = mem::read<uint32_t>(buffer + offset);
        uint64_t descWord1 = mem::read<uint32_t>(buffer + offset + 4);
        uint64_t descWord2 = mem::read<uint32_t>(buffer + offset + 8);

        TransDescriptor desc;
        desc.flags = bitRange<0, 1>(descWord2);
        desc.address = descWord1 | (bitRange<28, 31>(descWord2) << 32) | (bitRange<2, 4>(descWord2) << 36);
        desc.size = descWord0 | (bitRange<24, 27>(descWord2) << 32);

        logger::info("W: flags:%u, addr:%016llX, size:%016llX", desc.flags, desc.address, desc.size);

        info.transDescriptors.push_back(desc);

        offset += 12;
    }

    uint64_t rawDataStart = buffer + offset;

    if ((offset % 0x10) != 0) {
        offset += 0x10 - (offset % 0x10); // TODO alignUp helper function
    }

    if (cFlags != 0) {
        if (cFlags == 1) {
            // TODO inlined C buffer?
            logger::error("cFlags == 1");
        } else {
            // TODO how does it differ when cFlags > 2?
            uint64_t numCBuffs = (cFlags == 2) ? 1 : cFlags - 2;
            uint64_t sizeOfCBufferSizes = (numCBuffs + 1) / 2;
            uint64_t sizeStart = rawDataStart + (wordCount - sizeOfCBufferSizes) * 4;
            std::vector<uint64_t> cBufferSizes;
            for (auto i = 0u; i < numCBuffs; i++) {
                // TODO why is size stored in two different places? Do they mean different things?
                // Is it meant to expose the size to the handler? (by putting it in the raw data section)
                cBufferSizes.push_back(mem::read<uint16_t>(sizeStart + i * 2));

                uint64_t descWord0 = mem::read<uint32_t>(rawDataStart + wordCount * 4 + 8 * i);
                uint64_t descWord1 = mem::read<uint32_t>(rawDataStart + wordCount * 4 + 8 * i + 4);

                StaticOutDescriptor desc;
                desc.address = descWord0 | (bitRange<0, 15>(descWord1));
                desc.size = bitRange<16, 31>(descWord1);

                if (desc.size != cBufferSizes[i]) {
                    logger::error("Size mismatch in C descriptor");
                }

                logger::info("C: addr:%016llX, size:%016llX, size2:%016llX", desc.address, desc.size, cBufferSizes[i]);

                info.staticOutDescriptors.push_back(desc);
            }
        }
    }

    ObjectPtr<service::InterfaceHandle> object = kernel::object_pointer_cast<service::InterfaceHandle>(kernel::handleman::lookupObject(handle));
    if (!object) {
        logger::error("Game tried to use to invalid handle: %08X", handle);
        return 0;
    }

    if (object->isDomain && (type != 5)) {
        info.domainCmd = mem::read<uint8_t>(buffer + offset);
        info.domainSize = mem::read<uint16_t>(buffer + offset + 2);
        info.domainObjId = mem::read<uint32_t>(buffer + offset + 4);

        logger::info("Domain message: cmd = %02X, size = %04X, ObjectID = %08X", info.domainCmd, info.domainSize, info.domainObjId);

        offset += 0x10;
    }

    info.rawData = buffer + offset;
    info.isDomain = object->isDomain;

    info.cmdId = mem::read<uint32_t>(info.rawData + 8);

    // TODO how exactly does wordCount correlate to the args in? It seems inconsistant, and generally larger than needed
    info.argData.resize(wordCount * sizeof(uint32_t));
    for (auto i = 0; i < info.argData.size(); i++) {
        info.argData[i] = mem::read<uint8_t>(info.rawData + i + 16);
    }

    ///*
    logger::info("IPC: handle = %08X, id = %u, type = %u, buffer = %016llX", handle, info.cmdId, type, buffer);
    logger::info("Descriptors: X:%u, A:%u, B:%u, W:%u", numStaticIn, numSend, numRecv, numTrans);
    logger::info("wordCount: %u", wordCount);
    logger::info("cFlags: %u", cFlags);
    logger::info("enableHandleDescriptor: %u", enableHandleDescriptor);
    logger::info("sendPid: %u", sendPid);
    logger::info("Handle: Copy:%u, Move:%u", numHandlesToCopy, numHandlesToMove);
    //*/

    logger::info("msg:");
    debug::dumpMem(buffer, 0x80);

    if (!(info.isDomain && (info.domainCmd == 2)) && (type != 2) && (mem::read<uint32_t>(info.rawData) != 0x49434653)) {
        logger::error("Incorrect magic in IPC %08X", mem::read<uint32_t>(info.rawData));
        return 0;
    }

    if (info.isDomain && (type != 5)) {
        if (info.domainCmd == 1) {
            // TODO type always is 4?
            ObjectPtr<service::InterfaceHandle> inter = kernel::object_pointer_cast<service::InterfaceHandle>(kernel::handleman::lookupObject(info.domainObjId));
            if (!inter) {
                logger::error("Game tried to use to invalid handle: %08X", handle);
                return 0;
            }

            inter->interface->handleSyncRequest(&info);
        } else if (info.domainCmd == 2) {
            // close handle
            // TODO
        } else {
            logger::error("Unhandled domain command %u", info.domainCmd);
        }
    } else {
        if (type == 4) {
            ObjectPtr<service::InterfaceHandle> inter = kernel::object_pointer_cast<service::InterfaceHandle>(kernel::handleman::lookupObject(handle));
            if (!inter) {
                logger::error("Game tried to use to invalid handle: %08X", handle);
                return 0;
            }

            inter->interface->handleSyncRequest(&info);
        } else if (type == 5) {
            switch (info.cmdId) {
            case 0:
            {
                // ConvertSessionToDomain
                // TODO what even is a domain
                // currently we change the existing handle. Should we create a new handle instead?
                logger::info("ConvertSessionToDomain");

                ipc::IpcMessageCons reply(true, 0, &info);
                reply.writeError(0);
                uint32_t data = static_cast<uint32_t>(handle);
                reply.writeArgs(&data);
                reply.construct();

                ObjectPtr<service::InterfaceHandle> inter = kernel::object_pointer_cast<service::InterfaceHandle>(kernel::handleman::lookupObject(handle));
                if (!inter) {
                    logger::error("Game tried to use to invalid handle: %08X", handle);
                    return 0;
                }

                inter->isDomain = true;
                break;
            }
            case 2:
            {
                // DuplicateSession
                // TODO any side effects?
                logger::info("DuplicateSession");

                ipc::IpcMessageCons reply(true, 0, &info);
                reply.writeError(0);
                reply.moveHandle(handle);
                reply.construct();

                break;
            }
            case 3:
            {
                // QueryPointerBufferSize
                // TODO what does this do?
                logger::info("QueryPointerBufferSize");

                ipc::IpcMessageCons reply(true, 0, &info);
                reply.writeError(0);
                reply.writeData(0x500); // TODO how do we actually calculate this?
                reply.construct();

                break;
            }
            case 4:
            {
                // DuplicateSessionEx
                // TODO any side effects?
                // TODO how is this different from DuplicateSession?
                logger::info("DuplicateSessionEx");

                ipc::IpcMessageCons reply(true, 0, &info);
                reply.writeError(0);
                reply.moveHandle(handle);
                reply.construct();

                break;
            }
            default:
                logger::error("Unhandled ipc type 5 command: %d", info.cmdId);
            }
        } else if (type == 2) {
            // CloseHandle
            // TODO actually do it
            // how is this different from the svc?
            logger::info("Closing handle %08X", handle);

            ipc::IpcMessageCons reply(true, 0, &info);
            reply.writeError(0);
            reply.construct();

            logger::info("reply:");
            debug::dumpMem(buffer, 0x80);

            return 0xf601; // debug sdk asserts for this? (nn::sf::hipc::client::CloseClientSessionHandleSafely checks for 0x25A0B, returned when this returns 0xf601)
                           // perhaps returns this (0xf601 = Port remote dead) when you close the last handle of something?
        }
    }

    logger::info("reply:");
    debug::dumpMem(buffer, 0x80);

    return 0;
}

void IpcMessageCons::construct() {
    for (uint64_t i = 0; i < 0x100; i++) {
        mem::write<uint8_t>(memOffset + i, 0);
    }

    uint64_t size = 0;

    // TODO how does domain actually work? convert moved handles into data?
    uint32_t numObjects = 0;
    if (isDomain) {
        numObjects = movedHandles.size();
        for (uint64_t i = 0; i < movedHandles.size(); i++) {
            writeArgs(&movedHandles[i]); // TODO where are the object ids written to?
        }
        movedHandles.clear();
    }

    // TODO descriptors
    // TODO how to actually calcualte size?
    bool handleDesc = copiedHandles.size() > 0 || movedHandles.size() > 0;
    uint32_t word0 = type;
    uint32_t word1 = (8 + (dataBuf.size() + 3) / 4 + (isDomain ? 4 : 0)) | // size
        (handleDesc ? 0x80000000 : 0); // enable handle desciptor

    mem::write<uint32_t>(memOffset, word0);
    mem::write<uint32_t>(memOffset + 4, word1);

    size += 8;

    // TODO sendPID
    if (handleDesc) {
        uint32_t word = static_cast<uint32_t>((copiedHandles.size() << 1) | (movedHandles.size() << 5));
        mem::write(memOffset + 8, word);

        uint64_t handleOffset = 0;
        // TODO size of handles? assuming 4...
        for (uint64_t i = 0; i < copiedHandles.size(); i++) {
            mem::write<uint32_t>(memOffset + 12 + handleOffset, copiedHandles[i]);
            handleOffset += 4;
        }
        for (uint64_t i = 0; i < movedHandles.size(); i++) {
            mem::write<uint32_t>(memOffset + 12 + handleOffset, movedHandles[i]);
            handleOffset += 4;
        }

        size += 4 + copiedHandles.size() * 4 + movedHandles.size() * 4;
    }

    if ((size % 0x10) != 0) {
        size = size + (0x10 - (size % 0x10)); // align to 0x10
    }

    if (isDomain) {
        mem::write<uint32_t>(memOffset + size + 0x0, numObjects);
        mem::write<uint32_t>(memOffset + size + 0x4, 0); // miss-aligned u64?
        mem::write<uint32_t>(memOffset + size + 0x8, 0); // miss-aligned u64?
        mem::write<uint32_t>(memOffset + size + 0xC, 0); // unk
        size += 0x10;
    }

    mem::write(memOffset + size, 'S');
    mem::write(memOffset + size + 1, 'F');
    mem::write(memOffset + size + 2, 'C');
    mem::write(memOffset + size + 3, (isReply)? 'O' : 'I');

    mem::write<uint64_t>(memOffset + size + 8, (isReply) ? error : command);

    for (uint64_t i = 0; i < dataBuf.size(); i++) {
        // TODO bounds checking
        mem::write<uint8_t>(memOffset + size + 16 + i, dataBuf[i]);
    }
}

} // namespace ipc