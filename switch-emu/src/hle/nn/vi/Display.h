#pragma once
#include <string>
#include <vector>
#include <optional>

#include "structutils.h"
#include "hle\kernel\types\KEvent.h"

namespace nn {

namespace vi {

namespace emu {

// flattened IGraphicBufferProducer
struct IGBPBuffer {
    uint32_t magic;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;
    uint32_t usage;
    UNKNOWN(0x4);
    uint32_t index;
    UNKNOWN(0xC);
    uint32_t gpuBufferId;
    UNKNOWN(0x44);
    uint32_t nvmapHandle;
    uint32_t offset;
    UNKNOWN(0xF0);
};
// TODO offsets
CHECK_SIZE(IGBPBuffer, 0x16C);

class BufferQueue {
public:
    BufferQueue(uint64_t id_) : id(id_) {}

    struct GraphicBuffer {
        enum class Status {
            FREE,
            DEQUEUED,
            QUEUED
        };

        Status status;
        uint32_t slot;
        IGBPBuffer buffer;
    };

    void setPreallocatedBuffer(uint32_t slot, IGBPBuffer& buffer);
    IGBPBuffer& requestBuffer(uint32_t slot);
    uint32_t dequeueBuffer();
    void queueBuffer(uint32_t slot);
    void freeBuffer(uint32_t slot);

    std::optional<GraphicBuffer> getQueuedBuffer();

    uint64_t getId() const { return id; }

private:
    uint64_t id;

    std::vector<GraphicBuffer> buffers;
};

class Layer {
public:
    Layer(uint64_t id_, uint64_t bufferId_) : id(id_), bufferId(bufferId_) {}

    uint64_t getId() const { return id; }
    uint64_t getBufferId() const { return bufferId; }

private:
    uint64_t id;
    uint64_t bufferId;
};

class Display {
public:
    Display() = default;
    Display(uint64_t id_, std::string name_);

    uint64_t getId() const { return id; }
    std::string getName() const { return name; }

    void addLayer(uint64_t layerId);
    std::vector<uint64_t>& getLayers() { return layers; }

    // TODO getVSync
    void triggerVSync();

private:
    uint64_t id;
    std::string name;
    std::vector<uint64_t> layers;
    // vsync
};

class DisplayManager {
public:
    DisplayManager();

    uint64_t openDisplay(std::string name);
    Display& getDisplay(uint64_t id);

    uint64_t createLayer(uint64_t displayId);
    Layer& getLayer(uint64_t id);

    uint64_t createBufferQueue();
    BufferQueue& getBufferQueue(uint64_t id);

    void draw();
private:
    uint64_t nextLayer = 100;
    uint64_t nextBuffer = 1000;
    std::vector<Display> displays;
    std::vector<Layer> layers;
    std::vector<BufferQueue> buffers;
};

} // namespace emu

} // namespace vi

} // namespace nn