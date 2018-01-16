#include <fstream>

#include "loader.h"
#include "mem.h"
#include "bitutils.h"
#include "log.h"
#include "nso.h"
#include "lz4\lib\lz4.h"
#include "hle\kernel\memoryMapper.h"

namespace kernel {

namespace loader {

class FileCursor {
private:
    size_t mReadOffset = 0;

public:
    template<typename T>
    void read(const std::vector<uint8_t>& buffer, T& var) {
        var = *reinterpret_cast<const T*>(&buffer[mReadOffset]);
        mReadOffset += sizeof(T);
    }

    void seek(size_t i) {
        mReadOffset = i;
    }
};

FileCursor gFileReader;

void printElfHeader(ElfHeader& eh) {
    logger::info("e_magic:      %08X", *reinterpret_cast<uint32_t*>(eh.e_magic));
    logger::info("e_class:      %08X", eh.e_class);
    logger::info("e_data:       %08X", eh.e_data);
    logger::info("e_elfversion: %08X", eh.e_elfversion);
    logger::info("e_osabi:      %08X", eh.e_osabi);
    logger::info("e_abiversion: %08X", eh.e_abiversion);
    logger::info("-------------");
    logger::info("e_type:       %08X", eh.e_type);
    logger::info("e_machine:    %08X", eh.e_machine);
    logger::info("e_version:    %08X", eh.e_version);
    logger::info("e_entry:      %016llX", eh.e_entry);
    logger::info("e_phoff:      %016llX", eh.e_phoff);
    logger::info("e_shoff:      %016llX", eh.e_shoff);
    logger::info("e_flags:      %08X", eh.e_flags);
    logger::info("e_ehsize:     %08X", eh.e_ehsize);
    logger::info("e_phentsize:  %08X", eh.e_phentsize);
    logger::info("e_phnum:      %08X", eh.e_phnum);
    logger::info("e_shentsize:  %08X", eh.e_shentsize);
    logger::info("e_shnum:      %08X", eh.e_shnum);
    logger::info("e_shstrndx:   %08X", eh.e_shstrndx);
    logger::info("");
}

void printSectionHeader(ElfSectionHeader& sh) {
    logger::info("sh_name:        %08X", sh.sh_name);
    logger::info("sh_type:        %08X", sh.sh_type);
    logger::info("sh_flags:       %016llX", sh.sh_flags);
    logger::info("sh_addr:        %016llX", sh.sh_addr);
    logger::info("sh_offset:      %016llX", sh.sh_offset);
    logger::info("sh_size:        %016llX", sh.sh_size);
    logger::info("sh_link:        %08X", sh.sh_link);
    logger::info("sh_info:        %08X", sh.sh_info);
    logger::info("sh_addralign:   %016llX", sh.sh_addralign);
    logger::info("sh_entsize:     %016llX", sh.sh_entsize);
    logger::info("");
}

void printSection(Section& section) {
    logger::info("name: %s", section.name.c_str());
    logger::info("header:");
    printSectionHeader(section.sh);
}

void printSymbolHeader(ElfSymbolHeader& symh) {
    logger::info("st_name:  %08X", symh.st_name);
    logger::info("st_info:  %02X", symh.st_info);
    logger::info("st_other: %02X", symh.st_other);
    logger::info("st_shndx: %04X", symh.st_shndx);
    logger::info("st_value: %016llX", symh.st_value);
    logger::info("st_size:  %016llX", symh.st_size);
    logger::info("");
}

void printSymbol(Symbol& sym) {
    logger::info("name: %s", sym.name.c_str());
    logger::info("header:");
    printSymbolHeader(sym.sh);
}

void printProgramHeader(ElfProgramHeader& ph) {
    logger::info("p_type:    %08X", ph.p_type);
    logger::info("p_flags:   %08X", ph.p_flags);
    logger::info("p_offset:  %016llX", ph.p_offset);
    logger::info("p_vaddr:   %016llX", ph.p_vaddr);
    logger::info("p_paddr:   %016llX", ph.p_paddr);
    logger::info("p_filesz:  %016llX", ph.p_filesz);
    logger::info("p_memsz:   %016llX", ph.p_memsz);
    logger::info("p_align:   %016llX", ph.p_align);
    logger::info("");
}

bool readElfHeader(const std::vector<uint8_t>& buffer, ElfHeader& eh) {
    for (auto i = 0u; i < 4; i++) {
        gFileReader.read(buffer, eh.e_magic[i]);
    }
    gFileReader.read(buffer, eh.e_class);
    gFileReader.read(buffer, eh.e_data);
    gFileReader.read(buffer, eh.e_elfversion);
    gFileReader.read(buffer, eh.e_osabi);
    gFileReader.read(buffer, eh.e_abiversion);
    for (auto i = 0u; i < 7; i++) {
        gFileReader.read(buffer, eh.e_pad[i]);
    }

    gFileReader.read(buffer, eh.e_type);
    gFileReader.read(buffer, eh.e_machine);
    gFileReader.read(buffer, eh.e_version);
    gFileReader.read(buffer, eh.e_entry);
    gFileReader.read(buffer, eh.e_phoff);
    gFileReader.read(buffer, eh.e_shoff);
    gFileReader.read(buffer, eh.e_flags);
    gFileReader.read(buffer, eh.e_ehsize);
    gFileReader.read(buffer, eh.e_phentsize);
    gFileReader.read(buffer, eh.e_phnum);
    gFileReader.read(buffer, eh.e_shentsize);
    gFileReader.read(buffer, eh.e_shnum);
    gFileReader.read(buffer, eh.e_shstrndx);

    //printElfHeader(eh);

    return true;
}

bool readSectionHeader(const std::vector<uint8_t>& buffer, ElfSectionHeader& sh) {
    gFileReader.read(buffer, sh.sh_name);
    gFileReader.read(buffer, sh.sh_type);
    gFileReader.read(buffer, sh.sh_flags);
    gFileReader.read(buffer, sh.sh_addr);
    gFileReader.read(buffer, sh.sh_offset);
    gFileReader.read(buffer, sh.sh_size);
    gFileReader.read(buffer, sh.sh_link);
    gFileReader.read(buffer, sh.sh_info);
    gFileReader.read(buffer, sh.sh_addralign);
    gFileReader.read(buffer, sh.sh_entsize);
    return true;
}

bool readSymbolHeader(const std::vector<uint8_t>& buffer, ElfSymbolHeader& symh) {
    gFileReader.read(buffer, symh.st_name);
    gFileReader.read(buffer, symh.st_info);
    gFileReader.read(buffer, symh.st_other);
    gFileReader.read(buffer, symh.st_shndx);
    gFileReader.read(buffer, symh.st_value);
    gFileReader.read(buffer, symh.st_size);
    return true;
}

bool readSections(const std::vector<uint8_t>& buffer, Program& prog) {
    prog.sections.resize(prog.eh.e_shnum);

    for (auto i = 0u; i < prog.eh.e_shnum; i++) {
        auto& section = prog.sections[i];

        gFileReader.seek(prog.eh.e_shoff + i * prog.eh.e_shentsize);
        readSectionHeader(buffer, section.sh);

        //printSectionHeader(section.sh);

        if (!section.sh.sh_size) {
            continue;
        }

        section.data.resize(section.sh.sh_size);
        gFileReader.seek(section.sh.sh_offset);
        if (section.sh.sh_type == SHT_NOBITS) {
            std::fill(section.data.begin(), section.data.end(), 0xCC);
        } else {
            for (auto off = 0u; off < section.sh.sh_size; off++) {
                gFileReader.read(buffer, section.data.data()[off]);
            }
        }
    }

    return true;
}

bool loadSections(const std::vector<uint8_t>& buffer, Program& prog) {
    auto& strSection = prog.sections[prog.eh.e_shstrndx];

    for (auto& section : prog.sections) {
        section.name = strSection.data.data() + section.sh.sh_name;

        logger::info("Found section: %s", section.name.c_str());
        //printSection(section);
    }

    return true;
}

bool loadSymbols(const std::vector<uint8_t>& buffer, Program& prog) {
    auto symbolSection = prog.findSection(".symtab");

    if (symbolSection == nullptr) {
        return false;
    }

    auto& stringSection = prog.sections[symbolSection->sh.sh_link];

    gFileReader.seek(symbolSection->sh.sh_offset);

    auto numSymbols = symbolSection->data.size() / sizeof(ElfSymbolHeader);
    prog.symbols.resize(numSymbols);

    for (auto i = 0u; i < numSymbols; i++) {
        auto& symbol = prog.symbols[i];

        readSymbolHeader(buffer, symbol.sh);
        symbol.name = stringSection.data.data() + symbol.sh.st_name;

        //printSymbol(symbol);
    }

    return true;
}

bool loadDynSymbols(const std::vector<uint8_t>& buffer, Program& prog) {
    auto symbolSection = prog.findSection(".dynsym");

    if (symbolSection == nullptr) {
        return false;
    }

    auto& stringSection = prog.sections[symbolSection->sh.sh_link];

    gFileReader.seek(symbolSection->sh.sh_offset);

    auto numSymbols = symbolSection->data.size() / sizeof(ElfSymbolHeader);
    prog.dynSymbols.resize(numSymbols);

    for (auto i = 0u; i < numSymbols; i++) {
        auto& symbol = prog.dynSymbols[i];

        readSymbolHeader(buffer, symbol.sh);
        symbol.name = stringSection.data.data() + symbol.sh.st_name;

        //printSymbol(symbol);
    }

    return true;
}

bool loadRelocations(const std::vector<uint8_t>& buffer, Program& prog) {
    for (auto& section : prog.sections) {
        if (section.sh.sh_type == SHT_REL) {
            logger::warn("TODO REL");
        }
        if (section.sh.sh_type == SHT_RELA) {
            auto numRela = section.data.size() / sizeof(ElfRela);
            auto symbolSection = prog.sections[section.sh.sh_link];

            for (auto i = 0u; i < numRela; i++) {
                ElfRela rela;

                gFileReader.seek(section.sh.sh_offset + i * sizeof(ElfRela));
                gFileReader.read(buffer, rela.r_offset);
                gFileReader.read(buffer, rela.r_info);
                gFileReader.read(buffer, rela.r_addend);

                auto symType = bitRange<0, 31>(rela.r_info);
                auto symIndex = bitRange<32, 63>(rela.r_info);

                auto& symbol = prog.dynSymbols[symIndex];

                logger::info("%s", symbol.name.c_str());
                logger::info("r_offset: %016llX", rela.r_offset);
                logger::info("r_info:   %016llX", rela.r_info);
                logger::info("r_addend: %016llX", rela.r_addend);
                logger::info("");

                switch (symType) {
                case 1025:
                {
                    logger::info("R_AARCH64_GLOB_DAT Rela");
                    auto symToReloc = prog.findSymbol(symbol.name);

                    if (symToReloc == nullptr) {
                        logger::error("Could not find symbol");
                        continue;
                    }

                    break;
                }
                case 1026:
                {
                    logger::info("R_AARCH64_JUMP_SLOT Rela");
                    auto symToReloc = prog.findSymbol(symbol.name);

                    if (symToReloc == nullptr) {
                        logger::warn("Could not find symbol %s for relocation", symbol.name.c_str());
                        continue;
                    }

                    logger::warn("Unimplemented function %s", symbol.name.c_str());
                    mem::write(rela.r_offset, 0x20000000 + 8 * symIndex);

                    break;
                }
                default:
                    logger::info("TODO: unhandled relocation type");
                }
                logger::info("");
            }
        }
    }
    return true;
}

bool readProgramHeaders(const std::vector<uint8_t>& buffer, Program& prog) {
    gFileReader.seek(prog.eh.e_phoff);

    for (auto i = 0u; i < prog.eh.e_phnum; i++) {
        gFileReader.seek(prog.eh.e_phoff + i * prog.eh.e_phentsize);

        ElfProgramHeader ph;
        gFileReader.read(buffer, ph.p_type);
        gFileReader.read(buffer, ph.p_flags);
        gFileReader.read(buffer, ph.p_offset);
        gFileReader.read(buffer, ph.p_vaddr);
        gFileReader.read(buffer, ph.p_paddr);
        gFileReader.read(buffer, ph.p_filesz);
        gFileReader.read(buffer, ph.p_memsz);
        gFileReader.read(buffer, ph.p_align);

        //printProgramHeader(ph);

        if (ph.p_type == PT_LOAD) {
            mem::writeRegion(ph.p_vaddr, &buffer[0] + ph.p_offset, ph.p_filesz);
        } else if (ph.p_type == PT_DYNAMIC) {
            auto dynamicSection = prog.findSection(".dynamic");

            if (dynamicSection == nullptr) {
                logger::error("Could not load dynamic secion\n");
                return false;
            }

            auto numDynamic = dynamicSection->data.size() / sizeof(ElfDyn);

            //logger::info("%d", numDynamic);

            for (auto dynIndex = 0u; dynIndex < numDynamic; dynIndex++) {
                //ElfDyn dyn;

                //seek(dynamicSection->sh.sh_offset + dynIndex * sizeof(ElfDyn));
                //read(buffer, dyn.d_tag);
                //read(buffer, dyn.d_un);
            }
        }
    }

    return true;
}

bool Loader::loadElf(const std::vector<uint8_t>& buffer) {
    logger::info("Loading ELF");

    readElfHeader(buffer, prog.eh);

    if ((prog.eh.e_magic[0] != 0x7F) || (prog.eh.e_magic[1] != 'E') || (prog.eh.e_magic[2] != 'L') || (prog.eh.e_magic[3] != 'F')) {
        logger::error("Incorrect magic in ELF header");
        return false;
    }

    if (prog.eh.e_class != 2) {
        logger::error("Incorrect class in ELF header");
        return false;
    }

    if (prog.eh.e_data != 1) {
        logger::error("Incorrect data in ELF header");
        return false;
    }

    if (prog.eh.e_version != 1) {
        logger::error("Incorrect version in ELF header");
        return false;
    }

    if (prog.eh.e_machine != 0xB7) {
        logger::error("Incorrect machine in ELF header");
        return false;
    }

    if (prog.eh.e_flags != 0) {
        logger::error("Incorrect flags in ELF header");
        return false;
    }

    if (prog.eh.e_ehsize != sizeof(ElfHeader)) {
        logger::error("Incorrect ehsize in ELF header");
        return false;
    }

    if (prog.eh.e_shentsize != sizeof(ElfSectionHeader)) {
        logger::error("Incorrect shentsize in ELF header");
        return false;
    }

    readSections(buffer, prog);

    loadSections(buffer, prog);

    readProgramHeaders(buffer, prog);

    loadSymbols(buffer, prog);
    loadDynSymbols(buffer, prog);

    loadRelocations(buffer, prog);

    return true;
}

bool Loader::loadNso(const std::vector<uint8_t>& buffer) {
    logger::info("Loading NSO");

    gFileReader.seek(0);
    NSO_header header;
    gFileReader.read(buffer, header);

    /*
    logger::info("magic:               %08X", *reinterpret_cast<uint32_t*>(header.magic));
    logger::info("version:             %08X", header.version);
    logger::info("flags:               %08X", header.flags);
    for (auto i = 0u; i < header.NUM_SEGMENTS; i++) {
        switch (i) {
        case 0:
            logger::info(".text:");
            break;
        case 1:
            logger::info(".rodata:");
            break;
        case 2:
            logger::info(".data:");
            break;
        }
        logger::info("dataOffset:          %08X", header.segmentHeaders[i].dataOffset);
        logger::info("memoryOffset:        %08X", header.segmentHeaders[i].memoryOffset);
        logger::info("size:                %08X", header.segmentHeaders[i].size);
        if (i != 2) {
            logger::info("alignment:           %08X", header.segmentHeaders[i].alignment);
        } else {
            logger::info("bssSize:             %08X", header.segmentHeaders[i].bssSize);
        }
    }
    logger::info("------------");
    // buildid
    for (auto i = 0u; i < 3; i++) {
        logger::info("segmentSize[%d]:      %08X", i, header.segmentSizes[i]);
    }
    logger::info("rodataDynstr.offset: %08llX", header.rodataDynstr.offset);
    logger::info("rodataDynstr.size:   %08llX", header.rodataDynstr.size);
    logger::info("rodataDynsym.offset: %08llX", header.rodataDynsym.offset);
    logger::info("rodataDynsym.size:   %08llX", header.rodataDynsym.size);
    //SHA
    */

    std::vector<uint8_t> imageBuffer;
    imageBuffer.resize(header.segmentHeaders[2].memoryOffset + header.segmentHeaders[2].size + header.segmentHeaders[2].bssSize);

    for (auto i = 0u; i < header.NUM_SEGMENTS; i++) {
        if (!decompress(reinterpret_cast<const char*>(buffer.data() + header.segmentHeaders[i].dataOffset), header.segmentSizes[i],
            reinterpret_cast<char*>(imageBuffer.data() + header.segmentHeaders[i].memoryOffset), header.segmentHeaders[i].size)) {

            return false;
        }
    }

    std::fstream out;
    out.open("out.bin", std::ios::out | std::ios::binary);
    out.write((char*)imageBuffer.data(), imageBuffer.size());

    ModHeader modHeader;
    modHeader = *reinterpret_cast<ModHeader*>(imageBuffer.data() + reinterpret_cast<uint32_t*>(imageBuffer.data())[1]);

    /*
    logger::info("MOD header:");
    logger::info("magic:        %08X", modHeader.magic);
    logger::info("dynamic:      %08X", modHeader.dynamic);
    logger::info("bss_start:    %08X", modHeader.bss_start);
    logger::info("bss_end:      %08X", modHeader.bss_end);
    logger::info("unwind_start: %08X", modHeader.unwind_start);
    logger::info("unwind_end:   %08X", modHeader.unwind_end);
    logger::info("mod_object:   %08X", modHeader.mod_object);
    */

    ModObject modObj;
    modObj.module_base = nextFreeAddress;

    loadedModules.push_back(modObj);

    uint64_t size = nextFreeAddress;
    nextFreeAddress += imageBuffer.size(); // ?
    nextFreeAddress += 0x8000000 - (nextFreeAddress % 0x8000000); // align to 0x8000000, all modules are?
    size = nextFreeAddress - size;

    loadedModulesSizes.push_back(size);

    logger::info("Putting module at %016llX", modObj.module_base);

    // mostly guess
    // TODO alignUp function
    kernel::memmap::addMapping(header.segmentHeaders[0].memoryOffset + modObj.module_base,
        header.segmentHeaders[0].size + (0x1000 - (header.segmentHeaders[0].size % 0x1000)),
        kernel::memmap::MemoryType::CODE_STATIC,
        mem::Permission::READ_EXECUTE);

    kernel::memmap::addMapping(header.segmentHeaders[1].memoryOffset + modObj.module_base,
        header.segmentHeaders[1].size + (0x1000 - (header.segmentHeaders[1].size % 0x1000)),
        kernel::memmap::MemoryType::PROCESS_MEM, // ???
        mem::Permission::READ_ONLY);

    uint64_t bssSize = header.segmentHeaders[2].size + header.segmentHeaders[2].bssSize;
    kernel::memmap::addMapping(header.segmentHeaders[2].memoryOffset + modObj.module_base,
        bssSize + (0x1000 - (bssSize % 0x1000)),
        kernel::memmap::MemoryType::PROCESS_MEM, // ???
        mem::Permission::READ_WRITE);

    mem::writeRegion(modObj.module_base, imageBuffer.data(), imageBuffer.size());

    // TODO add MOD to getFileType
    if (modHeader.magic == 0x30444F4D) {
        ElfDyn* dynOffset = reinterpret_cast<ElfDyn*>(&imageBuffer[modHeader.dynamic + reinterpret_cast<uint32_t*>(imageBuffer.data())[1]]);
        std::vector<uint64_t> neededLibraries;

        for (; dynOffset->d_tag != DT_NULL; dynOffset++) {
            switch (dynOffset->d_tag) {
            case DT_NEEDED:
                neededLibraries.push_back(dynOffset->d_val);
                break;
            case DT_JMPREL:
                modObj.relplt = dynOffset->d_val;
                break;
            case DT_RELA:
                modObj.reldyn = dynOffset->d_val;
                break;
            case DT_RELASZ:
                modObj.reladynSize = dynOffset->d_val;
                break;
            case DT_PLTREL:
                if (dynOffset->d_val == 7) {
                    modObj.isRela = 1;
                } else if (dynOffset->d_val == 17) {
                    modObj.isRela = 0;
                } else {
                    logger::warn("Unkown DT_PLTREL value %d", dynOffset->d_val);
                }
                break;
            case DT_PLTRELSZ:
                modObj.relpltSize = dynOffset->d_val;
                break;
            case DT_INIT:
                modObj.init = dynOffset->d_val;
                break;
            case DT_FINI:
                modObj.fini = dynOffset->d_val;
                break;
            case DT_STRTAB:
                modObj.strtab = dynOffset->d_val;
                break;
            case DT_SYMTAB:
                modObj.symtab = dynOffset->d_val;
                break;
            case DT_STRSZ:
                modObj.strtabSize = dynOffset->d_val;
                break;
            case DT_PLTGOT:
                modObj.got = dynOffset->d_val;
                break;
            case 0x000000006ffffff9: // RELACOUNT
                modObj.relaCount = dynOffset->d_val;
                break;
            case DT_RELAENT:
                if (dynOffset->d_val != 24) {
                    logger::warn("Unkown DT_RELAENT size %d", dynOffset->d_val);
                }
                break;
            case DT_SYMENT:
                if (dynOffset->d_val != 24) {
                    logger::warn("Unkown DT_SYMENT size %d", dynOffset->d_val);
                }
                break;
            case DT_DEBUG:
                break;
            default:
                logger::warn("Unkown d_tag %016llX with val : %016llX", dynOffset->d_tag, dynOffset->d_val);
                break;
            }
        }

        // SYMBOLS
        if (modObj.symtab != 0) {
            if (imageBuffer.size() >= (modObj.symtab + sizeof(ElfSymbolHeader))) {
                ElfSymbolHeader* symHeader = reinterpret_cast<ElfSymbolHeader*>(&imageBuffer[modObj.symtab]);
                uint64_t numSymbols = (modObj.strtab - modObj.symtab) / 24; // TODO better way that does not depend on the section ordering

                std::vector<Symbol> dynSymbols;
                dynSymbols.resize(numSymbols);

                uint64_t entry = 0;
                loadedSymbols.reserve(loadedSymbols.size() + numSymbols);
                for (int i = 0; i < numSymbols; i++, symHeader++) { // TODO bounds checking
                    auto& symbol = dynSymbols[i];
                    symbol.sh = *symHeader;
                    symbol.name = reinterpret_cast<char*>(imageBuffer.data() + modObj.strtab + symbol.sh.st_name); // TODO unsafe if no strtab
                    //if (symbol.name == "nnMain") {
                    if (symbol.name == "_init") {
                        entry = symbol.sh.st_value;
                        logger::info("Found entry at %016llX", entry);
                    }

                    if ((symbol.sh.st_bind == STB_GLOBAL) || (symbol.sh.st_bind == STB_WEAK)) {
                        if (symbol.sh.st_shndx != SHN_UNDEF) {
                            Symbol newGlobal = symbol;
                            newGlobal.sh.st_value += modObj.module_base;
                            // TODO how to handle multiple definitions?
                            loadedSymbols.emplace_back(newGlobal);
                            symbolLookupMap[newGlobal.sh.st_value] = newGlobal.name;
                        }
                    }
                }
                loadedSymbols.shrink_to_fit();
            } else {
                logger::warn("MOD symbol table at invalid location. MOD size: 0x%08X, symtab: 0x%08X", imageBuffer.size(), modObj.symtab);
            }
        }

        // LIBRARIES

        for (uint64_t i = 0u; i < neededLibraries.size(); i++) {
            uint64_t name = neededLibraries[i];
            logger::info("Loading library %s", reinterpret_cast<char*>(imageBuffer.data() + modObj.strtab + name)); // TODO unsafe if no strtab
            std::string fileName = "";
            // TODO How are this actually determined?
            if (i == neededLibraries.size() - 1) {
                // loading sdk
                fileName = "sdk";
            } else {
                // loading subsdk{i}
                char buff[8];
                snprintf(buff, 8, "subsdk%lld", i);
                fileName = buff;
            }
            logger::info("Using file %s for it", fileName.c_str());

            loadExe(workingPath + fileName);
        }
    }

    return true;
}

bool Loader::loadExe(const std::string& filePath) {
    std::vector<uint8_t> buffer;
    
    if (!readFile(buffer, filePath)) {
        return false;
    }

    if (buffer.size() == 0) {
        logger::error("Attempted to load empty or nonexistent file");
        return false;
    }

    switch (getFileType(buffer)) {
    case 0:
        return loadElf(buffer);
    case 1:
        return loadNso(buffer);
    default:
        logger::error("Unknown file type");
        return false;
    }
}

bool Loader::loadGame() {
    // TODO clean up loading
    if (!loadExe(workingPath + "rtld")) {
        //return false;
    }
    if (!loadExe(workingPath + "main")) {
        return false;
    }
    prog.eh.e_entry = 0x8000000;
    return true;
}

bool Loader::decompress(const char* src, uint32_t srcLength, char* dst, uint32_t dstLength) {
    int length = LZ4_decompress_safe(src, dst, srcLength, dstLength);
    if (length < 0) {
        logger::error("Error during decompression: %d", length);
        return false;
    }
    if (static_cast<uint32_t>(length) != dstLength) {
        logger::error("Mismatched decompression sizes");
        return false;
    }
    return length > 0;
}

// TODO enums
int Loader::getFileType(const std::vector<uint8_t>& buffer) {
    gFileReader.seek(0);
    uint8_t magic[4];
    for (auto i = 0u; i < 4; i++) {
        gFileReader.read(buffer, magic[i]);
    }
    gFileReader.seek(0);

    if ((magic[0] == 0x7F) || (magic[1] == 'E') || (magic[2] == 'L') || (magic[3] == 'F')) {
        return 0;
    }

    if ((magic[0] == 'N') || (magic[1] == 'S') || (magic[2] == 'O') || (magic[3] == '0')) {
        return 1;
    }

    return -1;
}

uint64_t Loader::getProgramEntry() {
    return prog.eh.e_entry;
}

std::string* Loader::findSymbol(uint64_t addr) {
    auto iter = symbolLookupMap.find(addr);

    if (iter != symbolLookupMap.end()) {
        return &(iter->second);
    } else {
        return nullptr;
    }
}

std::string Loader::findNearestSymbol(uint64_t addr) {
    auto iter = symbolLookupMap.lower_bound(addr);

    if (iter != symbolLookupMap.end()) {
        uint64_t error = addr - iter->first;
        if (error == 0) {
            return iter->second;
        } else {
            return (iter->second) + " + " + std::to_string(error);
        }
    } else {
        return "No Symbol Found";
    }
}

Program& Loader::getProgram() {
    return prog;
}

bool Loader::readFile(std::vector<uint8_t>& buffer, const std::string& filePath) {
    logger::info("Loading file %s", filePath.c_str());

    std::ifstream fileStream(filePath, std::ifstream::binary);
    if (!fileStream.is_open()) {
        logger::error("Couldn't open file %s", filePath.c_str());
        return false;
    }

    fileStream.seekg(0, fileStream.end);
    auto size = static_cast<unsigned int>(fileStream.tellg());
    fileStream.seekg(0, fileStream.beg);

    buffer.resize(size);

    fileStream.read(reinterpret_cast<char*>(buffer.data()), size);
    logger::info("Size: %d", size);

    return true;
}

} // namespace loader

} // namespace kernel