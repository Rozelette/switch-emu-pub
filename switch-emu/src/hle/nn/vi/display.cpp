#include <algorithm>

#include "Display.h"
#include "log.h"
#include "hle/kernel/kernel.h"
#include "gpu\nvmapInterface.h"

nn::vi::emu::DisplayManager gDisplayManager;

namespace nn {

namespace vi {

namespace emu {

void BufferQueue::setPreallocatedBuffer(uint32_t slot, IGBPBuffer& buffer) {
    GraphicBuffer newBuffer;
    newBuffer.status = GraphicBuffer::Status::FREE;
    newBuffer.slot = slot;
    newBuffer.buffer = buffer;
    buffers.emplace_back(newBuffer);
}

IGBPBuffer& BufferQueue::requestBuffer(uint32_t slot) {
    auto itr = std::find_if(buffers.begin(), buffers.end(), [&](const GraphicBuffer& buffer) { return buffer.slot == slot; });

    if (itr == buffers.end()) {
        logger::error("Could not find buffer at slot %d", slot);
    }

    return itr->buffer;
}

uint32_t BufferQueue::dequeueBuffer() {
    // TODO DEQUEUE_BUFFER pases in values (pixelFormat, width, height). Should we make sure to return a buffer with matching parameters?
    auto itr = std::find_if(buffers.begin(), buffers.end(), [&](const GraphicBuffer& buffer) { return buffer.status == BufferQueue::GraphicBuffer::Status::FREE; });

    if (itr == buffers.end()) {
        logger::error("Could not find a free buffer");
    }

    itr->status = BufferQueue::GraphicBuffer::Status::DEQUEUED;

    return itr->slot;
}

void BufferQueue::queueBuffer(uint32_t slot) {
    auto itr = std::find_if(buffers.begin(), buffers.end(), [&](const GraphicBuffer& buffer) { return buffer.slot == slot; });

    if (itr == buffers.end()) {
        logger::error("Could not find buffer at slot %d", slot);
    }

    itr->status = BufferQueue::GraphicBuffer::Status::QUEUED;
}

void BufferQueue::freeBuffer(uint32_t slot) {
    auto itr = std::find_if(buffers.begin(), buffers.end(), [&](const GraphicBuffer& buffer) { return buffer.slot == slot; });

    if (itr == buffers.end()) {
        logger::error("Could not find buffer at slot %d", slot);
    }

    itr->status = BufferQueue::GraphicBuffer::Status::FREE;
}

std::optional<BufferQueue::GraphicBuffer> BufferQueue::getQueuedBuffer() {
    auto itr = std::find_if(buffers.begin(), buffers.end(), [&](const GraphicBuffer& buffer) { return buffer.status == BufferQueue::GraphicBuffer::Status::QUEUED; });

    if (itr == buffers.end()) {
        return {};
    }

    return *itr;
}

Display::Display(uint64_t id_, std::string name_) : id(id_), name(name_) {
    // TODO create vsync event
}

void Display::addLayer(uint64_t layerId) {
    layers.push_back(layerId);
}

void Display::triggerVSync() {
    // TODO
}

DisplayManager::DisplayManager() {
    // TODO more
    displays.emplace_back(Display(1, "Default"));
}

uint64_t DisplayManager::openDisplay(std::string name) {
    auto iter = std::find_if(displays.begin(), displays.end(), [&](const Display& display) {return display.getName() == name; });

    if (iter == displays.end()) {
        logger::error("Unknown display %s", name.c_str());
    }
    return (*iter).getId();
}

Display& DisplayManager::getDisplay(uint64_t id) {
    auto iter = std::find_if(displays.begin(), displays.end(), [&](const Display& display) {return display.getId() == id; });

    if (iter == displays.end()) {
        logger::error("Unknown display id %016llX", id);
    }
    return *iter;
}

uint64_t DisplayManager::createLayer(uint64_t displayId) {
    uint64_t layerId = nextLayer++;
    uint64_t bufferId = createBufferQueue();
    layers.emplace_back(Layer(layerId, bufferId));

    getDisplay(displayId).addLayer(layerId);

    logger::info("Adding layer %016llX", layerId);
    logger::info("Num layers %016llX", layers.size());

    return layerId;
}

Layer& DisplayManager::getLayer(uint64_t id) {
    auto iter = std::find_if(layers.begin(), layers.end(), [&](const Layer& layer) {return layer.getId() == id; });

    if (iter == layers.end()) {
        logger::error("Unknown layer id %016llX", id);
        logger::error("Unknown layer id %016llX", layers.size());
    }
    return *iter;
}

uint64_t DisplayManager::createBufferQueue() {
    // TODO
    uint64_t bufferId = nextBuffer++;
    buffers.emplace_back(BufferQueue(bufferId));

    return bufferId;
}

BufferQueue& DisplayManager::getBufferQueue(uint64_t id) {
    auto iter = std::find_if(buffers.begin(), buffers.end(), [&](const BufferQueue& buffer) {return buffer.getId() == id; });

    if (iter == buffers.end()) {
        logger::error("Unknown buffer id %016llX", id);
    }
    return *iter;
}

// TODO race conditions?
void DisplayManager::draw() {
    for (Display& display : displays) {
        if (display.getLayers().size() == 0) {
            continue;
        }

        for (uint64_t layerId : display.getLayers()) {
            Layer& layer = getLayer(layerId);
            BufferQueue& bufferQueue = getBufferQueue(layer.getBufferId());

            auto& queuedBuffer = bufferQueue.getQueuedBuffer();
            if (queuedBuffer.has_value()) {
                const BufferQueue::GraphicBuffer& buffer = queuedBuffer.value();
                kernel::getGraphicsDriver()->setBuffer(gpu::lookupNvmapObject(buffer.buffer.nvmapHandle).addr + buffer.buffer.offset);

                bufferQueue.freeBuffer(buffer.slot);
            }
        }
    }
}

} // namespace emu

} // namespace vi

} // namespace nn