#pragma once
#include <stdint.h>

#include "structutils.h"

struct ModHeader {
    uint32_t magic;
    uint32_t dynamic;
    uint32_t bss_start;
    uint32_t bss_end;
    uint32_t unwind_start;
    uint32_t unwind_end;
    uint32_t mod_object;
};

CHECK_SIZE(ModHeader, 0x1C);

struct ModObject {
    uint64_t next = 0;
    uint64_t prev = 0;
    uint64_t relplt = 0;
    uint64_t reldyn = 0;
    uint64_t module_base = 0;
    uint64_t dynamic = 0;
    uint64_t isRela = 0;
    uint64_t relpltSize = 0;
    uint64_t init = 0;
    uint64_t fini = 0;
    uint64_t bucket = 0;
    uint64_t chain = 0;
    uint64_t strtab = 0;
    uint64_t symtab = 0;
    uint64_t strtabSize = 0;
    uint64_t got = 0;
    uint64_t reladynSize = 0;
    uint64_t reldynSize = 0;
    uint64_t relCount = 0;
    uint64_t relaCount = 0;
    uint64_t nchain = 0;
    uint64_t nbucket = 0;
    uint64_t gotValue = 0;
};

CHECK_OFFSET(ModObject, next, 0x0);
CHECK_OFFSET(ModObject, prev, 0x8);
CHECK_OFFSET(ModObject, relplt, 0x10);
CHECK_OFFSET(ModObject, reldyn, 0x18);
CHECK_OFFSET(ModObject, module_base, 0x20);
CHECK_OFFSET(ModObject, dynamic, 0x28);
CHECK_OFFSET(ModObject, isRela, 0x30);
CHECK_OFFSET(ModObject, relpltSize, 0x38);
CHECK_OFFSET(ModObject, init, 0x40);
CHECK_OFFSET(ModObject, fini, 0x48);
CHECK_OFFSET(ModObject, bucket, 0x50);
CHECK_OFFSET(ModObject, chain, 0x58);
CHECK_OFFSET(ModObject, strtab, 0x60);
CHECK_OFFSET(ModObject, symtab, 0x68);
CHECK_OFFSET(ModObject, strtabSize, 0x70);
CHECK_OFFSET(ModObject, got, 0x78);
CHECK_OFFSET(ModObject, reladynSize, 0x80);
CHECK_OFFSET(ModObject, reldynSize, 0x88);
CHECK_OFFSET(ModObject, relCount, 0x90);
CHECK_OFFSET(ModObject, relaCount, 0x98);
CHECK_OFFSET(ModObject, nchain, 0xA0);
CHECK_OFFSET(ModObject, nbucket, 0xA8);
CHECK_OFFSET(ModObject, gotValue, 0xB0);
CHECK_SIZE(ModObject, 0xB8);
