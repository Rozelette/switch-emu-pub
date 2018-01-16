#pragma once
#include <stdint.h>
#include "structutils.h"

namespace nn {

namespace am {

using AppletMessage = uint32_t; // TODO enum?

namespace service {

struct AppletIdentityInfo {
    uint8_t unk[16]; // TODO uint128_t?
};
CHECK_SIZE(AppletIdentityInfo, 0x10);

using AppletKind = uint64_t; // TODO enum?

} // namespace service

} // namespace am

} // namespace nn