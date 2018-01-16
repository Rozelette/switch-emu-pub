#include "deviceHandleManager.h"
#include "deviceInterface.h"

#include "nvhostCtrlInterface.h"
#include "nvhostCtrlGpuInterface.h"
#include "nvmapInterface.h"
#include "nvhostAsGpuInterface.h"
#include "nvhostGpuInterface.h"

#include "log.h"

namespace gpu::handleman {

handle_t connectToDevice(std::string name) {
    if (name == "/dev/nvhost-ctrl") {
        return kernel::handleman::createHandle(ObjectPtr<DeviceInterface>(new NvhostCtrlInterface()));
    } else if (name == "/dev/nvhost-ctrl-gpu") {
        return kernel::handleman::createHandle(ObjectPtr<DeviceInterface>(new NvhostCtrlGpuInterface()));
    } else if (name == "/dev/nvmap") {
        return kernel::handleman::createHandle(ObjectPtr<DeviceInterface>(new NvmapInterface()));
    } else if (name == "/dev/nvhost-as-gpu") {
        return kernel::handleman::createHandle(ObjectPtr<DeviceInterface>(new NvhostAsGpuInterface()));
    } else if (name == "/dev/nvhost-gpu") {
        return kernel::handleman::createHandle(ObjectPtr<DeviceInterface>(new NvhostGpuInterface()));
    } else {
        logger::error("Connected to unknown device: %s", name.c_str());
    }

    return 0;
}

uint32_t sendIoctl(handle_t handle, uint32_t cmd, const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO type check
    ObjectPtr<DeviceInterface> inter = kernel::object_pointer_cast<DeviceInterface>(kernel::handleman::lookupObject(handle));

    return inter->handleIoctl(cmd, in, out);
}

void closeHandle(handle_t handle) {
    // TODO
}

} // namespace gpu