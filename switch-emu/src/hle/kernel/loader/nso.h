#pragma once
#include <stdint.h>

#include "structutils.h"

struct NsoSegmentHeader {
    uint32_t dataOffset;
    uint32_t memoryOffset;
    uint32_t size;
    union {
        uint32_t alignment;
        uint32_t bssSize;
    };
};

struct DataExtends {
    uint32_t offset;
    uint32_t size;
};

struct NSO_header {
    enum SegmentType {
        Text,
        Rodata,
        Data,
        NUM_SEGMENTS
    };

    uint8_t magic[4];
    uint32_t version; // ?
    uint32_t reserved1; // ?
    uint32_t flags; // ? TextCompress = 1, RoCompress = 2, DataCompress = 4, TextHash = 8, RoHash = 0x10, DataHash = 0x20
    NsoSegmentHeader segmentHeaders[NUM_SEGMENTS];
    uint8_t gnuBuildId[0x20]; // varible length?
    uint32_t segmentSizes[NUM_SEGMENTS];
    PADDING(0x24);
    DataExtends rodataDynstr;
    DataExtends rodataDynsym;
    uint8_t segmentSha256s[0x20][NUM_SEGMENTS];
};
CHECK_OFFSET(NSO_header, magic, 0x0);
CHECK_OFFSET(NSO_header, version, 0x4);
CHECK_OFFSET(NSO_header, reserved1, 0x8);
CHECK_OFFSET(NSO_header, flags, 0xC);
CHECK_OFFSET(NSO_header, segmentHeaders, 0x10);
CHECK_OFFSET(NSO_header, gnuBuildId, 0x40);
CHECK_OFFSET(NSO_header, segmentSizes, 0x60);
CHECK_OFFSET(NSO_header, rodataDynstr, 0x90);
CHECK_OFFSET(NSO_header, rodataDynsym, 0x98);
CHECK_OFFSET(NSO_header, segmentSha256s, 0xA0);
CHECK_SIZE(NSO_header, 0x100);
