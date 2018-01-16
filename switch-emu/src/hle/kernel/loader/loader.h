#pragma once
#include <vector>
#include <string>
#include <tuple>
#include <map>
#include <functional>

#include "elf.h"
#include "mod.h"

namespace kernel {

struct Section {
    ElfSectionHeader sh;
    std::vector<char> data;
    std::string name;
};

struct Symbol {
    ElfSymbolHeader sh;
    std::string name;
};

struct Program {
    ElfHeader eh;
    std::vector<Section> sections;
    std::vector<Symbol> symbols;
    std::vector<Symbol> dynSymbols;

    Section* findSection(const std::string& s) {
        for (auto& section : sections) {
            if (section.name.compare(s) == 0) {
                return &section;
            }
        }

        return nullptr;
    }

    Symbol* findSymbol(const std::string& s) {
        for (auto& symbol : symbols) {
            if (symbol.name.compare(s) == 0) {
                return &symbol;
            }
        }

        return nullptr;
    }
};

namespace loader {

class Loader {
public:
    Loader(){};

    void setRootPath(const std::string& path) {
        workingPath = path;
    }

    bool loadGame();
    bool loadExe(const std::string& path);

    Program& getProgram();
    uint64_t getProgramEntry();

    std::string* findSymbol(uint64_t addr);
    std::string findNearestSymbol(uint64_t addr);

private:
    // TODO isLoaded
    std::string workingPath;
    uint64_t nextFreeAddress = 0x8000000;
    std::vector<Symbol> loadedSymbols;
    Program prog;
    std::map<uint64_t, std::string, std::greater<uint64_t>> symbolLookupMap;
    std::vector<ModObject> loadedModules;
    std::vector<uint64_t> loadedModulesSizes;

    bool loadElf(const std::vector<uint8_t>& buffer);
    bool loadNso(const std::vector<uint8_t>& buffer);

    bool Loader::readFile(std::vector<uint8_t>& buffer, const std::string& path);
    bool decompress(const char* src, uint32_t srcLength, char* dst, uint32_t dstLength);
    int getFileType(const std::vector<uint8_t>& buffer);

};

} // namespace loader

} // namespace kernel