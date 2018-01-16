#pragma once
#include <stdint.h>

#include "common\structutils.h"

namespace kernel {

struct META {
    char magic[4];
    UNKNOWN(0x1B);
    char titleName[0x50];
    uint32_t aci0Offset;
    uint32_t aci0Size;
    uint32_t acidOffset;
    uint32_t acidSize;
};
CHECK_OFFSET(META, magic, 0x0);
CHECK_OFFSET(META, titleName, 0x20);
CHECK_OFFSET(META, aci0Offset, 0x70);
CHECK_OFFSET(META, aci0Size, 0x74);
CHECK_OFFSET(META, acidOffset, 0x78);
CHECK_OFFSET(META, acidSize, 0x7C);
CHECK_SIZE(META, 0x80);

struct ACID {
    char rsa2048Signature[0x100];
    char rsa2048PublicKey[0x100];
    char magic[4];
    uint32_t size; // ?
    PADDING(0x4); // zeroes?
    uint32_t retailFlag; // 0 = dev unit, 1 = retail
    uint64_t titleId;
    uint64_t titleIdCopy;
    uint32_t fsAccessControlOffset;
    uint32_t fsAccessControlSize;
    uint32_t serviceAccessControlOffset;
    uint32_t serviceAccessControlSize;
    uint32_t kernelAccessControlOffset;
    uint32_t kernelAccessControlSize;
    PADDING(0x8);
};
CHECK_OFFSET(ACID, rsa2048Signature, 0x0);
CHECK_OFFSET(ACID, rsa2048PublicKey, 0x100);
CHECK_OFFSET(ACID, magic, 0x200);
CHECK_OFFSET(ACID, size, 0x0204);
CHECK_OFFSET(ACID, retailFlag, 0x20C);
CHECK_OFFSET(ACID, titleId, 0x210);
CHECK_OFFSET(ACID, titleIdCopy, 0x218);
CHECK_OFFSET(ACID, fsAccessControlOffset, 0x220);
CHECK_OFFSET(ACID, fsAccessControlSize, 0x224);
CHECK_OFFSET(ACID, serviceAccessControlOffset, 0x228);
CHECK_OFFSET(ACID, serviceAccessControlSize, 0x22C);
CHECK_OFFSET(ACID, kernelAccessControlOffset, 0x230);
CHECK_OFFSET(ACID, kernelAccessControlSize, 0x234);
CHECK_SIZE(ACID, 0x240);

struct ACI0 {
    // same as ACID?
};

} // namespace kernel