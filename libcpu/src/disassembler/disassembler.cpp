#include <sstream>

#include "disassembler.h"
#include "types.h"
#include "bitutils.h"
#include "instutils.h"
#include "mem.h"
#include "decode.h"
#include "unreachable.h"

// TODO all instruction aliases
namespace cpu {

namespace disassembler {

std::vector<InstructionInfo> infoTable;
std::vector<InstructionInfo> aliasTable;

#define ARG_LIST(...) __VA_ARGS__

#define INST(x, name, regs, ...) \
    infoTable.push_back( { \
        instructionID::x, \
        name, \
        { ARG_LIST regs }, \
        0, \
        0, \
        #x \
    } );

// TODO different ID?
#define ALIAS(x, name, regs, mask, condition, ...) \
    aliasTable.push_back( { \
        instructionID::INVALID, \
        name, \
        { ARG_LIST regs }, \
        mask, \
        condition \
    } );

bool init() {
    infoTable.push_back({ instructionID::INVALID, "INVALID", {}, 0, 0, "INVALID" });
#include "instructions.inl"
    infoTable.push_back({ instructionID::NUM_INSTRUCTIONS, "NUM_INSTRUCTIONS", {}, 0, 0, "NUM_INSTRUCTIONS" });

#include "instruction_aliases.inl"

    return true;
}

#undef INST
#undef ALIAS
#undef ARG_LIST

std::string disassembleCondition(Condition cond) {
    switch (cond) {
    case Condition::EQ:
        return "EQ";
    case Condition::NE:
        return "NE";
    case Condition::CS:
        return "CS";
    case Condition::CC:
        return "CC";
    case Condition::MI:
        return "MI";
    case Condition::PL:
        return "PL";
    case Condition::VS:
        return "VS";
    case Condition::VC:
        return "VC";
    case Condition::HI:
        return "HI";
    case Condition::LS:
        return "LS";
    case Condition::GE:
        return "GE";
    case Condition::LT:
        return "LT";
    case Condition::GT:
        return "GT";
    case Condition::LE:
        return "LE";
    case Condition::AL:
    case Condition::A2:
        return "AL";
    default:
        UNREACHABLE();
    }
    return "";
}

std::string disassembleShiftType(uint64_t shift) {
    switch (shift) {
    case 0:
        return "LSL";
    case 1:
        return "LSR";
    case 2:
        return "ASR";
    case 3:
        return "ROR";
    default:
        UNREACHABLE();
    }
    return "";
}

std::string disassembleExtendType(uint64_t extend) {
    switch (extend) {
    case 0:
        return "UXTB";
    case 1:
        return "UXTH";
    case 2:
        return "UXTW"; // or LSL?
    case 3:
        return "UXTX";
    case 4:
        return "SXTB";
    case 5:
        return "SXTH";
    case 6:
        return "SXTW";
    case 7:
        return "SXTX";
    default:
        UNREACHABLE();
    }
    return "";
}

std::string disassemblePrefetchOp(uint64_t op) {
    // TODO better disassembly
    return "#" + std::to_string(op);
}

std::string disassembleBarrierType(uint64_t type) {
    switch (type) {
    case 0:
        return "#0";
    case 1:
        return "OSHLD";
    case 2:
        return "OSHST";
    case 3:
        return "OSH";
    case 4:
        return "#4";
    case 5:
        return "NSHLD";
    case 6:
        return "NSHST";
    case 7:
        return "NSH";
    case 8:
        return "#8";
    case 9:
        return "ISHLD";
    case 10:
        return "ISHST";
    case 11:
        return "ISH";
    case 12:
        return "#12";
    case 13:
        return "LD";
    case 14:
        return "ST";
    case 15:
        return "SY";
    default:
        UNREACHABLE();
    }
    return "";
}

std::string disassembleReg(RegType type, bool useSP, uint32_t regNum) {
    std::string ret = "";

    switch (type) {
    case RegType::X:
        if (regNum == 31) {
            if (useSP) {
                ret += "SP";
            } else {
                ret += "XZR";
            }
        } else {
            ret += "X";
            ret += std::to_string(regNum);
        }
        break;
    case RegType::W:
        if (regNum == 31) {
            if (useSP) {
                ret += "WSP";
            } else {
                ret += "WZR";
            }
        } else {
            ret += "W";
            ret += std::to_string(regNum);
        }
        break;
    case RegType::B:
        ret += "B";
        ret += std::to_string(regNum);
        break;
    case RegType::H:
        ret += "H";
        ret += std::to_string(regNum);
        break;
    case RegType::S:
        ret += "S";
        ret += std::to_string(regNum);
        break;
    case RegType::D:
        ret += "D";
        ret += std::to_string(regNum);
        break;
    case RegType::Q:
        ret += "Q";
        ret += std::to_string(regNum);
        break;
    default:
        break;
    }

    return ret;
}

std::string insertDelimiter(bool& insertComma) {
    if (insertComma) {
        return ", ";
    } else {
        insertComma = true;
        return "";
    }
}

InstructionInfo getInstruction(const cpu::DecodedInstruction& dInst) {
    for (auto alias : aliasTable) {
        if ((dInst.inst.bits & alias.mask) == alias.condition) {
            return alias;
        }
    }

    // TODO less simple aliases

    return infoTable[static_cast<uint32_t>(dInst.id)];
}

std::string getBaseInstructionName(cpu::instructionID id) {
    return infoTable[static_cast<uint32_t>(id)].rawName;
}

bool disassemble(DecodedInstruction& dInst, Disassembly& dis, uint64_t addr) {
    std::stringstream ss;

    AArch64Inst& inst = dInst.inst;

    if (!mem::isValid(addr)) {
        return false;
    }

    if (dInst.id == cpu::instructionID::INVALID) {
        return false;
    }

    auto info = getInstruction(dInst);

    dis.name = info.name;

    bool insertComma = false;

    for (auto reg : info.regs) {
        switch (reg) {
        case cond_branch:
            dis.name += ".";
            dis.name += disassembleCondition(static_cast<Condition>(inst.cond));
            break;
        case cond_comp:
            ss << insertDelimiter(insertComma) << disassembleCondition(static_cast<Condition>(inst.cond_comp));
            break;
        case Wt:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, false, inst.Rt);
            break;
        case Xt:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::X, false, inst.Rt);
            break;
        case Bt:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::B, false, inst.Rt);
            break;
        case Ht:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::H, false, inst.Rt);
            break;
        case St:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::S, false, inst.Rt);
            break;
        case Dt:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::D, false, inst.Rt);
            break;
        case Qt:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::Q, false, inst.Rt);
            break;
        case Wt2:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, false, inst.Rt2);
            break;
        case Xt2:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::X, false, inst.Rt2);
            break;
        case Bt2:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::B, false, inst.Rt2);
            break;
        case Ht2:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::H, false, inst.Rt2);
            break;
        case St2:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::S, false, inst.Rt2);
            break;
        case Dt2:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::D, false, inst.Rt2);
            break;
        case Qt2:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::Q, false, inst.Rt2);
            break;
        case Wn:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, false, inst.Rn);
            break;
        case Xn:
            if ((dInst.id != instructionID::RET) || (inst.Rn != 30)) {
                ss << insertDelimiter(insertComma) << disassembleReg(RegType::X, false, inst.Rn);
            }
            break;
        case Bn:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::B, false, inst.Rn);
            break;
        case Hn:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::H, false, inst.Rn);
            break;
        case Sn:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::S, false, inst.Rn);
            break;
        case Dn:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::D, false, inst.Rn);
            break;
        case Qn:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::Q, false, inst.Rn);
            break;
        case Wn_WSP:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, true, inst.Rn);
            break;
        case Xn_SP:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::X, true, inst.Rn);
            break;
        case Wm:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, false, inst.Rm);
            break;
        case Xm:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::X, false, inst.Rm);
            break;
        case Bm:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::B, false, inst.Rm);
            break;
        case Hm:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::H, false, inst.Rm);
            break;
        case Sm:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::S, false, inst.Rm);
            break;
        case Dm:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::D, false, inst.Rm);
            break;
        case Qm:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::Q, false, inst.Rm);
            break;
        case Wa:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, false, inst.Ra);
            break;
        case Xa:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::X, false, inst.Ra);
            break;
        case Ba:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::B, false, inst.Ra);
            break;
        case Ha:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::H, false, inst.Ra);
            break;
        case Sa:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::S, false, inst.Ra);
            break;
        case Da:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::D, false, inst.Ra);
            break;
        case Qa:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::Q, false, inst.Ra);
            break;
        case Wd:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, false, inst.Rd);
            break;
        case Xd:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::X, false, inst.Rd);
            break;
        case Bd:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::B, false, inst.Rd);
            break;
        case Hd:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::H, false, inst.Rd);
            break;
        case Sd:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::S, false, inst.Rd);
            break;
        case Dd:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::D, false, inst.Rd);
            break;
        case Qd:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::Q, false, inst.Rd);
            break;
        case Wd_WSP:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, true, inst.Rd);
            break;
        case Xd_SP:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::X, true, inst.Rd);
            break;
        case Ws:
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, false, inst.Rs);
            break;
        case imm_tbz:
            ss << insertDelimiter(insertComma) << "#" << std::to_string(inst.b40);
            break;
        case imm_exceptions:
            ss << insertDelimiter(insertComma) << "#" << std::to_string(inst._5_20);
            break;
        case imm_hint:
            ss << insertDelimiter(insertComma) << "#" << std::to_string(inst._5_11);
            break;
        case imm_clrex:
            // TODO imm print optional?
            ss << insertDelimiter(insertComma) << "#" << std::to_string(inst._8_11);
            break;
        case imm_adr:
        {
            uint64_t off = signExtend<21>(static_cast<uint64_t>((inst.immhi << 2) | inst.immlo));
            ss << insertDelimiter(insertComma) << std::hex << (addr + off);
            break;
        }
        case imm_adrp:
        {
            uint64_t off = signExtend<21>(static_cast<uint64_t>((inst.immhi << 2) | inst.immlo)) << 12;
            ss << insertDelimiter(insertComma) << std::hex << ((addr + off) & 0xFFFFFFFFFFFFF000);
            break;
        }
        case imm_addsub:
        {
            bool shift = testBit<22>(inst.bits);
            uint64_t imm = static_cast<uint64_t>(inst._10_21) << ((shift)? 12 : 0);
            ss << insertDelimiter(insertComma) << "#0x" << std::hex << imm;
            break;
        }
        case imm_logical:
            ss << insertDelimiter(insertComma) << "#0x" << std::hex << getLogicalImmediate(inst);
            break;
        case imm_mov:
        {
            uint64_t shift = inst.hw * 16;
            uint64_t imm = static_cast<uint64_t>(inst._5_20) << shift;
            ss << insertDelimiter(insertComma) << "#0x" << std::hex << imm;
            break;
        }
        case imm_shift:
            ss << insertDelimiter(insertComma) << "#0x" << std::hex << inst.immr;
            break;
        case imm_bitfield:
            ss << insertDelimiter(insertComma) << "#0x" << std::hex << inst.immr << ", #0x" << std::hex << inst.imms;
            break;
        case imm_extract:
            ss << insertDelimiter(insertComma) << "#0x" << std::hex << inst.imms;
            break;
        case imm_compare:
            ss << insertDelimiter(insertComma) << "#0x" << std::hex << inst._16_20;
            break;
        // TODO better names?
        case label_loadstore:
        case label_cond_branch:
        {
            uint64_t off = signExtend<19>(static_cast<uint64_t>(inst._5_23)) << 2;
            ss << insertDelimiter(insertComma) << std::hex << (addr + off);
            break;
        }
        case label_branch:
        {
            uint64_t off = signExtend<26>(static_cast<uint64_t>(inst._0_25)) << 2;
            ss << insertDelimiter(insertComma) << std::hex << (addr + off);
            break;
        }
        case label_tbz:
        {
            uint64_t off = signExtend<14>(static_cast<uint64_t>(inst._5_18)) << 2;
            ss << insertDelimiter(insertComma) << std::hex << (addr + off);
            break;
        }
        case prefetch_op:
            ss << insertDelimiter(insertComma) << disassemblePrefetchOp(inst.Rt);
            break;
        case barrier_type:
            ss << insertDelimiter(insertComma) << disassembleBarrierType(inst.CRm);
            break;
        case shifted_reg:
            if (inst._10_15 != 0) {
                ss << insertDelimiter(insertComma) << disassembleShiftType(inst.shift) << " #" << std::to_string(inst._10_15);
            }
            break;
        // TODO can we collapse these next two?
        case W_extended_reg:
            // TODO omit extend sometimes?
            ss << insertDelimiter(insertComma) << disassembleReg(RegType::W, false, inst.Rm) << " ";
            ss << disassembleExtendType(inst.option) << " #" << std::to_string(inst._10_12);
            break;
        case X_extended_reg:
            // TODO omit extend sometimes?
            ss << insertDelimiter(insertComma) << disassembleReg(((inst.option == 3) || (inst.option == 7))? RegType::X : RegType::W, false, inst.Rm) << " ";
            ss << disassembleExtendType(inst.option) << " #" << std::to_string(inst._10_12);
            break;
        case cond_comp_flags:
            ss << insertDelimiter(insertComma) << "#" << std::to_string(inst.nzcv);
            break;
        case loadstore_no_off:
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << "]";
            break;
        case loadstore_post_off:
        {
            int64_t imm = signExtend<9>(static_cast<int64_t>(inst._12_20));
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << "], #0x" << std::hex << imm;
            break;
        }
        case loadstore_pre_off:
        {
            int64_t imm = signExtend<9>(static_cast<int64_t>(inst._12_20));
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]!";
            break;
        }
        case loadstore_unscaled_off:
        {
            int64_t imm = signExtend<9>(static_cast<int64_t>(inst._12_20));
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]";
            break;
        }
        case loadstore_unsigned_off_8:
        {
            // TODO don't print imm if == 0?
            int64_t imm = static_cast<int64_t>(inst._10_21) << 0;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]";
            break;
        }
        case loadstore_unsigned_off_16:
        {
            // TODO don't print imm if == 0?
            int64_t imm = static_cast<int64_t>(inst._10_21) << 1;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]";
            break;
        }
        case loadstore_unsigned_off_32:
        {
            // TODO don't print imm if == 0?
            int64_t imm = static_cast<int64_t>(inst._10_21) << 2;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]";
            break;
        }
        case loadstore_unsigned_off_64:
        {
            // TODO don't print imm if == 0?
            int64_t imm = static_cast<int64_t>(inst._10_21) << 3;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]";
            break;
        }
        case loadstore_unsigned_off_128:
        {
            // TODO don't print imm if == 0?
            int64_t imm = static_cast<int64_t>(inst._10_21) << 4;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]";
            break;
        }
        case loadstore_post_off_pair_32:
        {
            int64_t imm = signExtend<7>(static_cast<int64_t>(inst._15_21)) << 2;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << "], #0x" << std::hex << imm;
            break;
        }
        case loadstore_post_off_pair_64:
        {
            int64_t imm = signExtend<7>(static_cast<int64_t>(inst._15_21)) << 3;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << "], #0x" << std::hex << imm;
            break;
        }
        case loadstore_post_off_pair_128:
        {
            int64_t imm = signExtend<7>(static_cast<int64_t>(inst._15_21)) << 3;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << "], #0x" << std::hex << imm;
            break;
        }
        case loadstore_pre_off_pair_32:
        {
            int64_t imm = signExtend<7>(static_cast<int64_t>(inst._15_21)) << 2;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]!";
            break;
        }
        case loadstore_pre_off_pair_64:
        {
            int64_t imm = signExtend<7>(static_cast<int64_t>(inst._15_21)) << 3;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]!";
            break;
        }
        case loadstore_pre_off_pair_128:
        {
            int64_t imm = signExtend<7>(static_cast<int64_t>(inst._15_21)) << 4;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]!";
            break;
        }
        case loadstore_off_pair_32:
        {
            // TODO don't print imm if == 0?
            int64_t imm = signExtend<7>(static_cast<int64_t>(inst._15_21)) << 2;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]";
            break;
        }
        case loadstore_off_pair_64:
        {
            // TODO don't print imm if == 0?
            int64_t imm = signExtend<7>(static_cast<int64_t>(inst._15_21)) << 3;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]";
            break;
        }
        case loadstore_off_pair_128:
        {
            // TODO don't print imm if == 0?
            int64_t imm = signExtend<7>(static_cast<int64_t>(inst._15_21)) << 4;
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", #0x" << std::hex << imm << "]";
            break;
        }
        case float_zero_imm:
            ss << insertDelimiter(insertComma) << "#0.0";
            break;
        case loadstore_reg:
            ss << insertDelimiter(insertComma) << "[" << disassembleReg(RegType::X, true, inst.Rn) << ", ";
            if (testBit<13>(inst.bits)) {
                ss << disassembleReg(RegType::X, false, inst.Rm);
            } else {
                ss << disassembleReg(RegType::W, false, inst.Rm);
            }
            ss << ", ";
            switch (inst.option) {
            case 2:
                ss << "UXTW";
                break;
            case 3:
                ss << "LSL";
                break;
            case 6:
                ss << "SXTW";
                break;
            case 7:
                ss << "SXTX";
                break;
            default:
                ss << "UNKNOWN";
            }
            ss << ", " << ((testBit<12>(inst.bits))? 2 : 0) << "]";
            break;
        default:
            ss << insertDelimiter(insertComma) << "TODO_ARG";
            break;
        }
    }

    dis.args = ss.str();

    return true;
}

}

}