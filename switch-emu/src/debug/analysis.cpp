#include <map>

#include "analysis.h"
#include "bitutils.h"
#include "hle\kernel\kernel.h"

namespace debug::analysis {

static std::map<uint64_t, FuncInfo> sFunctionTable;

void init() {
    // TODO make sure it's loaded
    auto prog = kernel::getLoader().getProgram();

    for (auto& sym : prog.symbols) {
        if (sym.sh.st_type == STT_FUNC) {
            printf("Found function %s\n", sym.name.c_str());
            FuncInfo info;
            info.name = sym.name;
            sFunctionTable.emplace(sym.sh.st_value, info);
        }
    }
}

AddrInfo get(uint64_t addr) {
    AddrInfo info;

    auto funcSearch = sFunctionTable.find(addr);
    if (funcSearch != sFunctionTable.end()) {
        info.func = &funcSearch->second;
    } else {
        info.func = nullptr;
    }

    return info;
}

// TODO really need a branch common func for this, disassembler, and interpreter
BranchInfo getBranchInfo(cpu::DecodedInstruction& dInst, uint64_t addr, cpu::Core* state) {
    BranchInfo ret;
    ret.target = 0xFFFFFFFFFFFFFFFF;
    ret.isVariable = false;
    switch (dInst.id) {
    case cpu::instructionID::B_cond:
    {
        uint64_t off = signExtend<19>(static_cast<uint64_t>(dInst.inst._5_23)) << 2;
        ret.target = addr + off;
        break;
    }
    case cpu::instructionID::BR:
    {
        ret.target = state->GPRegs[dInst.inst.Rn];
        ret.isVariable = true;
        break;
    }
    case cpu::instructionID::BLR:
    {
        ret.target = state->GPRegs[dInst.inst.Rn];
        ret.isVariable = true;
        break;
    }
    case cpu::instructionID::RET:
    {
        ret.target = state->GPRegs[dInst.inst.Rn];
        ret.isVariable = true;
        break;
    }
    case cpu::instructionID::ERET:
    {
        // TODO
        break;
    }
    case cpu::instructionID::DRPS:
    {
        // TODO
        break;
    }
    case cpu::instructionID::B:
    {
        uint64_t off = signExtend<26>(static_cast<uint64_t>(dInst.inst._0_25)) << 2;
        ret.target = addr + off;
        break;
    }
    case cpu::instructionID::BL:
    {
        uint64_t off = signExtend<26>(static_cast<uint64_t>(dInst.inst._0_25)) << 2;
        ret.target = addr + off;
        break;
    }
    case cpu::instructionID::CBZ_32:
    {
        uint64_t off = signExtend<19>(static_cast<uint64_t>(dInst.inst._5_23)) << 2;
        ret.target = addr + off;
        break;
    }
    case cpu::instructionID::CBNZ_32:
    {
        uint64_t off = signExtend<19>(static_cast<uint64_t>(dInst.inst._5_23)) << 2;
        ret.target = addr + off;
        break;
    }
    case cpu::instructionID::CBZ_64:
    {
        uint64_t off = signExtend<19>(static_cast<uint64_t>(dInst.inst._5_23)) << 2;
        ret.target = addr + off;
        break;
    }
    case cpu::instructionID::CBNZ_64:
    {
        uint64_t off = signExtend<19>(static_cast<uint64_t>(dInst.inst._5_23)) << 2;
        ret.target = addr + off;
        break;
    }
    case cpu::instructionID::TBZ:
    {
        uint64_t off = signExtend<14>(static_cast<uint64_t>(dInst.inst._5_18)) << 2;
        ret.target = addr + off;
        break;
    }
    case cpu::instructionID::TBNZ:
    {
        uint64_t off = signExtend<14>(static_cast<uint64_t>(dInst.inst._5_18)) << 2;
        ret.target = addr + off;
        break;
    }
    default:
        // TODO error
        break;
    }

    return ret;
}

} // namespace debug::analysis