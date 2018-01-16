#pragma once
#include <stdint.h>

#include "structutils.h"

namespace nn {

namespace time {

using PosixTime = uint64_t;

struct CalendarTime {
    int16_t year;
    int8_t month; // (1 - 12)
    int8_t day; // (1 - 31)
    int8_t hour;
    int8_t minute;
    int8_t second;
    PADDING(1);
};
CHECK_OFFSET(CalendarTime, year, 0x0);
CHECK_OFFSET(CalendarTime, month, 0x2);
CHECK_OFFSET(CalendarTime, day, 0x3);
CHECK_OFFSET(CalendarTime, hour, 0x4);
CHECK_OFFSET(CalendarTime, minute, 0x5);
CHECK_OFFSET(CalendarTime, second, 0x6);
CHECK_SIZE(CalendarTime, 0x8);

namespace sf {

struct CalendarAdditionalInfo {
    UNKNOWN(0x18);
};
CHECK_SIZE(CalendarAdditionalInfo, 0x18);

} // namespace sf

} // namespace time

} // namespace nn
