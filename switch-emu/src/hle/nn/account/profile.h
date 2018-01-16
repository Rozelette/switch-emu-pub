#pragma once
#include "structutils.h"

namespace nn {

namespace account {

namespace profile {

struct ProfileBase {
    UNKNOWN(0x38);
};
CHECK_SIZE(ProfileBase, 0x38);

} // namespace profile

} // namespace account

} // namespace nn
