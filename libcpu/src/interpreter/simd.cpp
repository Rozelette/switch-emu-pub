#include <stdio.h>
#include <stdint.h>

#include "cpu.h"
#include "bitutils.h"
#include "instutils.h"
#include "interpreter.h"

namespace cpu {

namespace interpreter {

// TODO helper function for applying an operation on every element in a vector?

enum class ElementSize {
    ELEM_SIZE_8  = 0,
    ELEM_SIZE_16 = 1,
    ELEM_SIZE_32 = 2,
    ELEM_SIZE_64 = 3,
};

constexpr uint64_t elemNumBits(ElementSize size) {
    return (size == ElementSize::ELEM_SIZE_8) ? 8 :
        (size == ElementSize::ELEM_SIZE_16) ? 16 :
        (size == ElementSize::ELEM_SIZE_32) ? 32 :
        64;
}

constexpr uint64_t replicate64(uint64_t imm, ElementSize size) {
    return (size == ElementSize::ELEM_SIZE_8) ? imm * 0x0101010101010101 :
        (size == ElementSize::ELEM_SIZE_16) ? imm * 0x0001000100010001 :
        (size == ElementSize::ELEM_SIZE_32) ? imm * 0x0000000100000001 :
        imm;
}

void dupGeneral(Core* core, AArch64Inst inst) {
    uint64_t numElem = 128;
    uint64_t size = 0;
    if ((inst._16_20 & 1) != 0) {
        size = 0;
    } else if ((inst._16_20 & 2) != 0) {
        size = 1;
    } else if((inst._16_20 & 4) != 0) {
        size = 2;
    } else if((inst._16_20 & 8) != 0) {
        size = 3;
    }
    uint64_t esize = 8llu << size;

    if (!testBit<30>(inst.bits)) {
        numElem /= 2;
    }

    numElem /= esize;

    uint64_t data = core->GPRegs[inst.Rn];

    switch (size) {
    case 0:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].B[i] = static_cast<uint8_t>(data);
        }
        break;
    case 1:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].H[i] = static_cast<uint16_t>(data);
        }
        break;
    case 2:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].S[i] = static_cast<uint32_t>(data);
        }
        break;
    case 3:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].D[i] = data;
        }
        break;
    }
}

void insGeneral(Core* core, AArch64Inst inst) {
    uint64_t index = 0;
    uint64_t size = 0;
    if ((inst._16_20 & 1) != 0) {
        size = 0;
    } else if ((inst._16_20 & 2) != 0) {
        size = 1;
    } else if ((inst._16_20 & 4) != 0) {
        size = 2;
    } else if ((inst._16_20 & 8) != 0) {
        size = 3;
    }
    index = inst._16_20 >> (size + 1);
    uint64_t data = core->GPRegs[inst.Rn];

    switch (size) {
    case 0:
        core->VRegs[inst.Rd].B[index] = static_cast<uint8_t>(data);
        break;
    case 1:
        core->VRegs[inst.Rd].H[index] = static_cast<uint16_t>(data);
        break;
    case 2:
        core->VRegs[inst.Rd].S[index] = static_cast<uint32_t>(data);
        break;
    case 3:
        core->VRegs[inst.Rd].D[index] = data;
        break;
    }
}

void fmovVectorImmSingle(Core* core, AArch64Inst inst) {
    uint32_t imm = 0; // TODO
    uint32_t imm8 = bitRange<5, 9>(inst.bits) | (bitRange<16, 18>(inst.bits) << 5);
    imm = (testBit<7>(imm8) ? 0x80000000 : 0) | (testBit<6>(imm8) ? 0x3E000000 : 0x40000000) | (bitRange<0, 5>(imm8) << 19);
    core->VRegs[inst.Rd].S[0] = imm;
    core->VRegs[inst.Rd].S[1] = imm;
    if (testBit<30>(inst.bits)) {
        core->VRegs[inst.Rd].S[2] = imm;
        core->VRegs[inst.Rd].S[3] = imm;
    } else {
        core->VRegs[inst.Rd].S[2] = 0;
        core->VRegs[inst.Rd].S[3] = 0;
    }
}

void orrRegisterVector(Core* core, AArch64Inst inst) {
    core->VRegs[inst.Rd].D[0] = core->VRegs[inst.Rn].D[0] | core->VRegs[inst.Rm].D[0];

    // TODO seperate opcodes?
    if (testBit<30>(inst.bits)) {
        core->VRegs[inst.Rd].D[1] = core->VRegs[inst.Rn].D[1] | core->VRegs[inst.Rm].D[1];
    } else {
        core->VRegs[inst.Rd].D[1] = 0;
    }
}

void ucvtfScalarInt(Core* core, AArch64Inst inst) {
    if (testBit<22>(inst.bits)) {
        core->VRegs[inst.Rd].floatD.val = static_cast<double>(core->VRegs[inst.Rn].D[0]);
    } else {
        core->VRegs[inst.Rd].floatS.val = static_cast<float>(core->VRegs[inst.Rn].S[0]);
    }
}

void scvtfScalarInt(Core* core, AArch64Inst inst) {
    if (testBit<22>(inst.bits)) {
        core->VRegs[inst.Rd].floatD.val = static_cast<double>(static_cast<int64_t>(core->VRegs[inst.Rn].D[0]));
    } else {
        core->VRegs[inst.Rd].floatS.val = static_cast<float>(static_cast<int32_t>(core->VRegs[inst.Rn].S[0]));
    }
}

enum MoviFlags {
    MoviShifted =      1 << 0,
    MoviShiftingOnes = 1 << 1,
};

// TODO is64 template parameter? (would need more decoded opcodes)
template<ElementSize size, unsigned flags = 0>
void moviGeneral(Core* core, AArch64Inst inst) {
    uint64_t imm = bitRange<5, 9>(inst.bits) | (bitRange<16, 18>(inst.bits) << 5);

    if constexpr (flags & MoviShifted) {
        uint64_t shift = bitRange<13, 14>(inst.bits);
        imm <<= shift * 8;
    } else if constexpr (flags & MoviShiftingOnes) {
        uint64_t shift = bitRange<12, 12>(inst.bits);
        imm <<= (shift + 1) * 8;
        imm |= bitmask<uint64_t>((shift + 1) * 8);
    } else if constexpr (size == ElementSize::ELEM_SIZE_64) {
        imm = ((testBit<0>(imm)) ? 0x00000000000000FF : 0) |
            ((testBit<1>(imm)) ? 0x000000000000FF00 : 0) |
            ((testBit<2>(imm)) ? 0x0000000000FF0000 : 0) |
            ((testBit<3>(imm)) ? 0x00000000FF000000 : 0) |
            ((testBit<4>(imm)) ? 0x000000FF00000000 : 0) |
            ((testBit<5>(imm)) ? 0x0000FF0000000000 : 0) |
            ((testBit<6>(imm)) ? 0x00FF000000000000 : 0) |
            ((testBit<7>(imm)) ? 0xFF00000000000000 : 0)
    }

    imm = replicate64(imm, size);

    if (testBit<30>(inst.bits)) {
        core->VRegs[inst.Rd].D[0] = imm;
        core->VRegs[inst.Rd].D[1] = imm;
    } else {
        core->VRegs[inst.Rd].D[0] = imm;
        core->VRegs[inst.Rd].D[1] = 0; // TODO are the upper bits cleared too?
    }
}

void movi8(Core* core, AArch64Inst inst) {
    moviGeneral<ElementSize::ELEM_SIZE_8>(core, inst);
}

void movi16ShiftedImm(Core* core, AArch64Inst inst) {
    moviGeneral<ElementSize::ELEM_SIZE_16, MoviShifted>(core, inst);
}

void movi32ShiftedImm(Core* core, AArch64Inst inst) {
    moviGeneral<ElementSize::ELEM_SIZE_32, MoviShifted>(core, inst);
}

void movi32ShiftingOnes(Core* core, AArch64Inst inst) {
    moviGeneral<ElementSize::ELEM_SIZE_32, MoviShiftingOnes>(core, inst);
}

void movi64Scalar(Core* core, AArch64Inst inst) {
    moviGeneral<ElementSize::ELEM_SIZE_64>(core, inst);
}

void movi64Vector(Core* core, AArch64Inst inst) {
    moviGeneral<ElementSize::ELEM_SIZE_64>(core, inst);
}

template<bool is64, bool isSigned>
void movGeneral(Core* core, AArch64Inst inst) {
    uint64_t data;
    uint64_t size;

    if (testBit<16>(inst.bits)) {
        uint64_t index = bitRange<17, 20>(inst.bits);
        size = 8;
        data = core->VRegs[inst.Rn].B[index];
    } else if (testBit<17>(inst.bits)) {
        uint64_t index = bitRange<18, 20>(inst.bits);
        size = 16;
        data = core->VRegs[inst.Rn].H[index];
    } else if(testBit<18>(inst.bits)) {
        uint64_t index = bitRange<19, 20>(inst.bits);
        size = 32;
        data = core->VRegs[inst.Rn].S[index];
    } else {
        uint64_t index = bitRange<20, 20>(inst.bits);
        size = 64;
        data = core->VRegs[inst.Rn].D[index];
    }

    if constexpr (isSigned) {
        data = signExtend<uint64_t>(data, size);
    }

    if (!is64) {
        data &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = data;
}

void umov32(Core* core, AArch64Inst inst) {
    movGeneral<false, false>(core, inst);
}

void umov64(Core* core, AArch64Inst inst) {
    movGeneral<true, false>(core, inst);
}

void smov32(Core* core, AArch64Inst inst) {
    movGeneral<false, true>(core, inst);
}

void smov64(Core* core, AArch64Inst inst) {
    movGeneral<true, true>(core, inst);
}

// TODO generic with sub
void addVector(Core* core, AArch64Inst inst) {
    uint64_t numElem = 128;
    uint64_t size = inst._22_23;
    uint64_t esize = 8llu << size;

    if (!testBit<30>(inst.bits)) {
        numElem /= 2;
    }

    numElem /= esize;

    switch (size) {
    case 0:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].B[i] = core->VRegs[inst.Rn].B[i] + core->VRegs[inst.Rm].B[i];
        }
        break;
    case 1:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].H[i] = core->VRegs[inst.Rn].H[i] + core->VRegs[inst.Rm].H[i];
        }
        break;
    case 2:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].S[i] = core->VRegs[inst.Rn].S[i] + core->VRegs[inst.Rm].S[i];
        }
        break;
    case 3:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].D[i] = core->VRegs[inst.Rn].D[i] + core->VRegs[inst.Rm].D[i];
        }
        break;
    }
}

// TODO decode is128bit?
void andVector(Core* core, AArch64Inst inst) {
    core->VRegs[inst.Rd].D[0] = core->VRegs[inst.Rn].D[0] & core->VRegs[inst.Rm].D[0];
    if (testBit<30>(inst.bits)) {
        core->VRegs[inst.Rd].D[1] = core->VRegs[inst.Rn].D[1] & core->VRegs[inst.Rm].D[1];
    }
}

template<bool isZero>
void cmeqVecGeneric(Core* core, AArch64Inst inst) {
    uint64_t numElem = 128;
    uint64_t size = inst._22_23;
    uint64_t esize = 8llu << size;

    if (!testBit<30>(inst.bits)) {
        numElem /= 2;
    }

    numElem /= esize;

    switch (size) {
    case 0:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].B[i] = (core->VRegs[inst.Rn].B[i] == ((isZero)? 0 : core->VRegs[inst.Rm].B[i])) ? 0xFF : 0;
        }
        break;
    case 1:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].H[i] = (core->VRegs[inst.Rn].H[i] == ((isZero) ? 0 : core->VRegs[inst.Rm].H[i])) ? 0xFFFF : 0;
        }
        break;
    case 2:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].S[i] = (core->VRegs[inst.Rn].S[i] == ((isZero) ? 0 : core->VRegs[inst.Rm].S[i])) ? 0xFFFFFFFF : 0;
        }
        break;
    case 3:
        for (int i = 0; i < numElem; i++) {
            core->VRegs[inst.Rd].D[i] = (core->VRegs[inst.Rn].D[i] == ((isZero) ? 0 : core->VRegs[inst.Rm].D[i])) ? 0xFFFFFFFFFFFFFFFF : 0;
        }
        break;
    }
}

void cmeqVecReg(Core* core, AArch64Inst inst) {
    return cmeqVecGeneric<false>(core, inst);
}

void cmeqVecZero(Core* core, AArch64Inst inst) {
    return cmeqVecGeneric<true>(core, inst);
}
void addpVec(Core* core, AArch64Inst inst) {
    uint64_t numElem = 128;
    uint64_t size = inst._22_23;
    uint64_t esize = 8llu << size;

    if (!testBit<30>(inst.bits)) {
        numElem /= 2;
    }

    numElem /= esize;

    switch (size) {
    case 0:
        for (int i = 0; i < (numElem / 2); i++) {
            core->VRegs[inst.Rd].B[i] = core->VRegs[inst.Rn].B[2 * i] + core->VRegs[inst.Rn].B[2 * i + 1];
            core->VRegs[inst.Rd].B[i + numElem / 2] = core->VRegs[inst.Rm].B[2 * i] + core->VRegs[inst.Rm].B[2 * i + 1];
        }
        break;
    case 1:
        for (int i = 0; i < (numElem / 2); i++) {
            core->VRegs[inst.Rd].H[i] = core->VRegs[inst.Rn].H[2 * i] + core->VRegs[inst.Rn].H[2 * i + 1];
            core->VRegs[inst.Rd].H[i + numElem / 2] = core->VRegs[inst.Rm].H[2 * i] + core->VRegs[inst.Rm].H[2 * i + 1];
        }
        break;
    case 2:
        for (int i = 0; i < (numElem / 2); i++) {
            core->VRegs[inst.Rd].S[i] = core->VRegs[inst.Rn].S[2 * i] + core->VRegs[inst.Rn].S[2 * i + 1];
            core->VRegs[inst.Rd].S[i + numElem / 2] = core->VRegs[inst.Rm].S[2 * i] + core->VRegs[inst.Rm].S[2 * i + 1];
        }
        break;
    case 3:
        for (int i = 0; i < (numElem / 2); i++) {
            core->VRegs[inst.Rd].D[i] = core->VRegs[inst.Rn].D[2 * i] + core->VRegs[inst.Rn].D[2 * i + 1];
            core->VRegs[inst.Rd].D[i + numElem / 2] = core->VRegs[inst.Rm].D[2 * i] + core->VRegs[inst.Rm].D[2 * i + 1];
        }
        break;
    }
}

void registerSIMDInstructions() {
    registerInstruction(instructionID::DUP_general, dupGeneral);
    registerInstruction(instructionID::INS_general, insGeneral);
    registerInstruction(instructionID::MOVI_8_imm, movi8);
    registerInstruction(instructionID::MOVI_16_shifted_imm, movi16ShiftedImm);
    registerInstruction(instructionID::MOVI_32_shifted_imm, movi32ShiftedImm);
    registerInstruction(instructionID::MOVI_32_shifting_ones, movi32ShiftingOnes);
    registerInstruction(instructionID::MOVI_64_scalar, movi64Scalar);
    registerInstruction(instructionID::MOVI_64_vec, movi64Vector);
    registerInstruction(instructionID::FMOV_vec_imm_single, fmovVectorImmSingle);
    registerInstruction(instructionID::ORR_vec_reg, orrRegisterVector);
    registerInstruction(instructionID::UCVTF_scalar_int, ucvtfScalarInt);
    registerInstruction(instructionID::SCVTF_scalar_int, scvtfScalarInt);
    registerInstruction(instructionID::UMOV_32, umov32);
    registerInstruction(instructionID::UMOV_64, umov64);
    registerInstruction(instructionID::SMOV_32, smov32);
    registerInstruction(instructionID::SMOV_64, smov64);
    registerInstruction(instructionID::ADD_vec, addVector);
    registerInstruction(instructionID::AND_vec, andVector);
    registerInstruction(instructionID::CMEQ_vec_reg, cmeqVecReg);
    registerInstruction(instructionID::CMEQ_vec_zero, cmeqVecZero);
    registerInstruction(instructionID::ADDP_vec, addpVec);
}

} // namespace interpreter

} // namespace cpu