#include "ITimeZoneService.h"
#include "hle\nn\time\time.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace timesrv {

namespace detail {

namespace service {

uint32_t ITimeZoneService::ToCalendarTimeWithMyRule(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    // nn::time::CalendarTime
    // notes: from function _ZN2nn4time14ToCalendarTimeEPNS0_12CalendarTimeEPNS0_22CalendarAdditionalInfoERKNS0_9PosixTimeE
    // code loads, in order, a half word, a byte, a byte, a byte, a byte, a byte
    /*
    nn::time::CalendarTime = {
        int16_t year; ?
        int8_t month; ? (1 - 12)
        int8_t day; ? (1 - 31)
        int8_t hour; ? (0 - 23?)
        int8_t minute; ?
        int8_t second; ?
        int8_t padding; ?
    }
    */
    // (month - 1) is used to index into and 12?-length array of [0, 31, 59, 90, ...] (values after 12th are 31)
    // Days since year start, by month?
    // This value is then added to (day - 1) and day, creating 2 values
    // Then, year is tested for being a leap year, and then selects one of those 2 values
    // daysElasped = (isLeapYear(year)) ? days + daysByMonth[month] : days + daysByMonth[month] - 1;
    // The checks if year > 1970
    // Then is does other stuff but I got bored and the program stopped crashing

    struct {
        nn::time::PosixTime posixTime;
    } args;
    ipc::readArgs(info, &args);

    logger::info("posixTime: 0x%016llX", args.posixTime);

    struct {
        nn::time::CalendarTime calendarTime;
        nn::time::sf::CalendarAdditionalInfo additionalInfo;
    } out;

    out.calendarTime.year = 2017;
    out.calendarTime.month = 11;
    out.calendarTime.day = 29;
    out.calendarTime.hour = 15;
    out.calendarTime.minute = 26;
    out.calendarTime.second = 50;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

} // namespace service

} // namespace detail

} // namespace timesrv

} // namespace nn