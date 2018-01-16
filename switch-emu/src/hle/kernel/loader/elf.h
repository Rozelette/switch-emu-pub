#pragma once
#include <stdint.h>

struct ElfHeader {
    uint8_t e_magic[4];
    uint8_t e_class;
    uint8_t e_data;
    uint8_t e_elfversion;
    uint8_t e_osabi;
    uint8_t e_abiversion;
    uint8_t e_pad[7];

    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

enum : uint32_t {
    SHT_NULL = 0,
    SHT_PROGBITS = 1,
    SHT_SYMTAB = 2,
    SHT_STRTAB = 3,
    SHT_RELA = 4,
    SHT_HASH = 5,
    SHT_DYNAMIC = 6,
    SHT_NOTE = 7,
    SHT_NOBITS = 8,
    SHT_REL = 9,
    SHT_SHLIB = 10,
    SHT_DYNSYM = 11,
    SHT_LOOS = 0x60000000,
    SHT_HIOS = 0x6FFFFFFF,
    SHT_LOPROC = 0x70000000,
    SHT_HIPROC = 0x7FFFFFFF
};

enum : uint64_t {
    SHF_WRITE = 0x1,
    SHF_ALLOC = 0x2,
    SHF_EXECINSTR = 0x4,
    SHF_MASKOS = 0x0F000000,
    SHF_MASKPROC = 0xF0000000
};

struct ElfSectionHeader {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
};

enum : uint16_t {
    SHN_UNDEF = 0,
    SHN_LOPROC = 0xFF00,
    SHN_HIPROC = 0xFF1F,
    SHN_LOOS = 0xFF20,
    SHN_HIOS = 0xFF3F,
    SHN_ABS = 0xFFF1,
    SHN_COMMON = 0xFFF2
};

struct ElfSymbolHeader {
    uint32_t st_name;
    union {
        uint8_t st_info;
        struct {
            uint8_t st_type : 4;
            uint8_t st_bind : 4;
        };
    };
    uint8_t st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
};

enum : uint32_t {
    STT_NOTYPE = 0,
    STT_OBJECT = 1,
    STT_FUNC = 2,
    STT_SECTION = 3,
    STT_FILE = 4,
    STT_LOPROC = 13,
    STT_HIPROC = 15
};

enum : uint32_t {
    STB_LOCAL = 0,
    STB_GLOBAL = 1,
    STB_WEAK = 2,
    STB_LOPROC = 13,
    STB_HIPROC = 15
};

struct ElfProgramHeader {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
};

enum : uint32_t {
    PT_NULL = 0,
    PT_LOAD = 1,
    PT_DYNAMIC = 2,
    PT_INTERP = 3,
    PT_NOTE = 4,
    PT_SHLIB = 5,
    PT_PHDR = 6,
    PT_LOOS = 0x60000000,
    PT_HIOS = 0x6FFFFFFF,
    PT_LOPROC = 0x70000000,
    PT_HIPROC = 0x7FFFFFFF
};

struct ElfDyn {
    uint64_t d_tag;
    union {
        uint64_t d_val;
        uint64_t d_ptr;
    };
};

enum : uint64_t {
    DT_NULL = 0,
    DT_NEEDED = 1,
    DT_PLTRELSZ = 2,
    DT_PLTGOT = 3,
    DT_HASH = 4,
    DT_STRTAB = 5,
    DT_SYMTAB = 6,
    DT_RELA = 7,
    DT_RELASZ = 8,
    DT_RELAENT = 9,
    DT_STRSZ = 10,
    DT_SYMENT = 11,
    DT_INIT = 12,
    DT_FINI = 13,
    DT_SONAME = 14,
    DT_RPATH = 15,
    DT_SYMBOLIC = 16,
    DT_REL = 17,
    DT_RELSZ = 18,
    DT_RELENT = 19,
    DT_PLTREL = 20,
    DT_DEBUG = 21,
    DT_TEXTREL = 22,
    DT_JMPREL = 23,
    DT_BIND_NOW = 24,
    DT_INIT_ARRAY = 25,
    DT_FINI_ARRAY = 26,
    DT_INIT_ARRAYSZ = 27,
    DT_FINI_ARRAY_SZ = 28,
    DT_LOOS = 0x60000000,
    DT_HIOS = 0x6FFFFFFF,
    DT_LOPROC = 0x70000000,
    DT_HIPROC = 0x7FFFFFFF
};

struct ElfRel {
    uint64_t r_offset;
    uint64_t r_info;
};

struct ElfRela {
    uint64_t r_offset;
    uint64_t r_info;
    uint64_t r_addend;
};

enum {
    R_AARCH64_NONE = 0,
    R_AARCH64_ABS64 = 257,
    R_AARCH64_GLOB_DAT = 1025,
    R_AARCH64_JUMP_SLOT = 1026,
    R_AARCH64_RELATIVE = 1027
};