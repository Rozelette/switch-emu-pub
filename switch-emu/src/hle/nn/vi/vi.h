#pragma once
#include "structutils.h"

namespace nn {

namespace vi {

struct DisplayName {
    char name[0x40];
};
CHECK_SIZE(DisplayName, 0x40);

} // namespace vi

} // namespace nn
