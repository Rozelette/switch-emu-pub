#pragma once
#include <stdint.h>

#include "common\structutils.h"

struct NRO_header {
    UNKNOWN(0x04); // TODO is this padding?
    uint32_t module_off;
    UNKNOWN(0x08); // TODO is this padding?
    uint8_t magic[4];
    UNKNOWN(0x04);
    uint32_t size;
    UNKNOWN(0x04);
    uint32_t text_off;
    uint32_t text_size;
    uint32_t ro_off;
    uint32_t ro_size;
    uint32_t data_off;
    uint32_t data_size;
    uint32_t bss_size;
    UNKNOWN(0x44);
};

CHECK_OFFSET(NRO_header, module_off, 0x04);
CHECK_OFFSET(NRO_header, magic, 0x10);
CHECK_OFFSET(NRO_header, size, 0x018);
CHECK_OFFSET(NRO_header, text_off, 0x20);
CHECK_OFFSET(NRO_header, text_size, 0x24);
CHECK_OFFSET(NRO_header, ro_off, 0x28);
CHECK_OFFSET(NRO_header, ro_size, 0x2C);
CHECK_OFFSET(NRO_header, data_off, 0x30);
CHECK_OFFSET(NRO_header, data_size, 0x34);
CHECK_OFFSET(NRO_header, bss_size, 0x38);
CHECK_SIZE(NRO_header, 0x80);
