#pragma once
#include <string>
#include <stdint.h>
#include <vector>

#include "hle\kernel\handleManager.h"

namespace gpu::handleman {

handle_t connectToDevice(std::string name);
uint32_t sendIoctl(handle_t handle, uint32_t cmd, const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
void closeHandle(handle_t handle);

} // namespace gpu::handleman