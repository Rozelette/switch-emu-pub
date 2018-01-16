#include "IHOSBinderDriver.h"
#include "debug\memDump.h"
#include "mem.h"
#include "hle/nn/vi/Display.h"

// TODO avoid somehow?
extern nn::vi::emu::DisplayManager gDisplayManager;

namespace nns {

namespace hosbinder {

uint32_t IHOSBinderDriver::TransactParcel(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO what are the differences?
    return TransactParcelAuto(info, reply);
}

uint32_t IHOSBinderDriver::AdjustRefcount(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    struct {
        int32_t handle;
        int32_t addval;
        int32_t type;
    } args;

    ipc::readArgs(info, &args);

    logger::info("handle: %08d, addval: %08d, type: %08d", args.handle, args.addval, args.type);
    return ERR_SUCCESS;
}

uint32_t IHOSBinderDriver::GetNativeHandle(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    struct {
        int32_t unk1;
        uint32_t unk2;
    } args;

    ipc::readArgs(info, &args);

    logger::info("unk1: %08d, unk2: 0x%08X", args.unk1, args.unk2);

    reply->copyHandle(0x4321); // TODO actual handle
    return ERR_SUCCESS;
}

uint32_t IHOSBinderDriver::TransactParcelAuto(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    struct {
        int32_t bufferId;
        Transaction transaction;
        uint32_t flags;
    } args;

    ipc::readArgs(info, &args);
    
    // TODO look at incoming parcel
    //uint64_t addr = info->staticInDescriptors[0].address;
    //uint64_t size = info->staticInDescriptors[0].size;
    uint64_t addrIn = info->sendDescriptors[0].address;
    uint64_t sizeIn = info->sendDescriptors[0].size;
    uint64_t addrOut = info->recvDescriptors[0].address;
    uint64_t sizeOut = info->recvDescriptors[0].size;

    logger::info("bufferId: %08d, transaction: %d, flags: 0x%08X", args.bufferId, args.transaction, args.flags);
    logger::info("Parcel in: 0x%016llX", addrIn);
    debug::dumpMem(addrIn, sizeIn);

    auto& bufferQueue = gDisplayManager.getBufferQueue(args.bufferId);

    switch (args.transaction) {
    case Transaction::REQUEST_BUFFER:
    {
        uint32_t slot = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x0);

        logger::info("slot: 0x%08X", slot);

        // TODO write out GraphicBuffer
        bufferQueue.requestBuffer(slot);

        // TODO better interface to write to buffers
        uint64_t offset = 0;
        mem::write<uint32_t>(addrOut + offset, 0x4 + 0x4 + 0x4 + 0x16C + 0x4); // ParcelDataSize
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x10); // ParcelDataOffset
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x0); // ParcelObjectsSize
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x10 + 0x4 + 0x4 + 0x4 + 0x16C + 0x4); // ParcelObjectsOffset
        offset += 4;

        // ParcelData

        mem::write<uint32_t>(addrOut + offset, 1); // nonNull
        offset += 4;

        mem::write<uint32_t>(addrOut + offset, 0x16c); // flattened GraphicBuffer length (0x16c)
        offset += 4;

        mem::write<uint32_t>(addrOut + offset, 0); // unknown
        offset += 4;

        for (uint64_t i = 0; i < (0x16C / 4); i++) {
            mem::write<uint32_t>(addrOut + offset, 0); // flattened GraphicBuffer
            offset += 4;
        }

        mem::write<uint32_t>(addrOut + offset, 0); // status
        offset += 4;
    }
    break;
    case Transaction::DEQUEUE_BUFFER:
    {
        uint32_t pixelFormat = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x0);
        uint32_t width = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x4);
        uint32_t height = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x8);
        uint32_t getFrameTimestamps = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0xC);
        uint32_t usage = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10);

        logger::info("pixelFormat: 0x%08X", pixelFormat);
        logger::info("width: 0x%08X", width);
        logger::info("height: 0x%08X", height);
        logger::info("getFrameTimestamps: 0x%08X", getFrameTimestamps);
        logger::info("usage: 0x%08X", usage);

        uint32_t slot = bufferQueue.dequeueBuffer();

        // TODO better interface to write to buffers
        uint64_t offset = 0;
        mem::write<uint32_t>(addrOut + offset, 0x4 + 0x2C + 0x4); // ParcelDataSize
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x10); // ParcelDataOffset
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x0); // ParcelObjectsSize
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x10 + 0x4 + 0x2C + 0x4); // ParcelObjectsOffset
        offset += 4;

        // ParcelData

        mem::write<uint32_t>(addrOut + offset, slot); // slot
        offset += 4;

        for (uint32_t i = 0; i < 11; i++) { // fence
            mem::write<uint32_t>(addrOut + offset, 0);
            offset += 4;
        }

        mem::write<uint32_t>(addrOut + offset, 0); // status
        offset += 4;
    }
    break;
    case Transaction::QUEUE_BUFFER:
    {
        uint32_t slot = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x0);

        logger::info("slot: 0x%08X", slot);

        // queue_buffer_input
        uint64_t timeStamp = mem::read<uint64_t>(addrIn + 0x10 + 0x50 + 0x4 + 0x0);
        bool isAutoTimestamp = mem::read<bool>(addrIn + 0x10 + 0x50 + 0x4 + 0x8);
        // rect
        uint32_t left = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x4 + 0xC);
        uint32_t top = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x4 + 0x10);
        uint32_t right = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x4 + 0x14);
        uint32_t bottom = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x4 + 0x18);
        
        int32_t scalingMode = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x4 + 0x1C);
        uint32_t transform = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x4 + 0x20);
        // fence (0x2C bytes)
        // more?

        logger::info("queue_buffer_input:");
        logger::info("timeStamp: 0x%016llX", timeStamp);
        logger::info("isAutoTimestamp: 0x%08X", isAutoTimestamp);
        logger::info("left: 0x%08X", left);
        logger::info("top: 0x%08X", top);
        logger::info("right: 0x%08X", right);
        logger::info("bottom: 0x%08X", bottom);
        logger::info("scalingMode: 0x%08X", scalingMode);
        logger::info("transform: 0x%08X", transform);

        bufferQueue.queueBuffer(slot);

        // TODO better interface to write to buffers
        uint64_t offset = 0;
        mem::write<uint32_t>(addrOut + offset, 0x16C + 0x4); // ParcelDataSize
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x10); // ParcelDataOffset
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x0); // ParcelObjectsSize
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x10 + 0x4 + 0x4 + 0x4 + 0x16C + 0x4); // ParcelObjectsOffset
        offset += 4;

        // ParcelData
        // TODO this is some other object actually

        for (uint64_t i = 0; i < (0x16C / 4); i++) {
            mem::write<uint32_t>(addrOut + offset, 0); // flattened GraphicBuffer
            offset += 4;
        }

        mem::write<uint32_t>(addrOut + offset, 0); // status
        offset += 4;
    }
    break;
    case Transaction::CONNECT:
    {
        // TODO better interface to write to buffers
        uint64_t offset = 0;
        mem::write<uint32_t>(addrOut + offset, 0x10 + 0x4); // ParcelDataSize
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x10); // ParcelDataOffset
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x0); // ParcelObjectsSize
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0x10 + 0x10 + 0x4); // ParcelObjectsOffset
        offset += 4;

        // ParcelData

        // bufferProducerQueueBufferOutput

        mem::write<uint32_t>(addrOut + offset, 1280); // width
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 720); // height
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0); // transformHint
        offset += 4;
        mem::write<uint32_t>(addrOut + offset, 0); // numPendingBuffers
        offset += 4;

        mem::write<uint32_t>(addrOut + offset, 0); // status
        offset += 4;
    }
    break;
    case Transaction::SET_PREALLOCATED_BUFFER:
    {
        uint32_t slot = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x0);
        uint32_t unk1 = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x4);
        uint32_t len = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x8); // flattened GraphicBuffer length (0x16c)
        uint32_t unk2 = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0xC);

        logger::info("SET_PREALLOCATED_BUFFER: slot: 0x%08X, unk1: 0x%08X, len: 0x%08X, unk2: 0x%08X", slot, unk1, len, unk2);

        // TODO read entire graphicBuffer
        nn::vi::emu::IGBPBuffer buffer;
        buffer.magic = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0x0);
        buffer.width = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0x4);
        buffer.height = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0x8);
        buffer.stride = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0xC);
        buffer.format = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0x10);
        buffer.usage = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0x14);
        // 0x18 - ?
        buffer.index = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0x1C);
        // 0x20 - 0x28 - ?
        buffer.gpuBufferId = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0x2C);
        // 0x30 - 0x70 - ?
        buffer.nvmapHandle = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0x74);
        buffer.offset = mem::read<uint32_t>(addrIn + 0x10 + 0x50 + 0x10 + 0x78);
        // 0x7C - 0x168 - ?
        // size == 0x16c

        logger::info("graphicBuffer:");
        logger::info("width: 0x%08X", buffer.width);
        logger::info("height: 0x%08X", buffer.height);
        logger::info("stride: 0x%08X", buffer.stride);
        logger::info("format: 0x%08X", buffer.format);
        logger::info("usage: 0x%08X", buffer.usage);
        logger::info("index: 0x%08X", buffer.index);
        logger::info("gpuBufferId: 0x%08X", buffer.gpuBufferId);
        logger::info("nvmapHandle: 0x%08X", buffer.nvmapHandle);
        logger::info("offset: 0x%08X", buffer.offset);

        bufferQueue.setPreallocatedBuffer(slot, buffer);
    }
    break;
    default:
        logger::error("Unknown transaction in TransactParcelAuto: %d", args.transaction);
        return 1;
        break;
    }

    return ERR_SUCCESS;
}

} // namespace hosbinder

} // namespace nns