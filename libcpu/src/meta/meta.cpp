#include "meta.h"

namespace cpu::meta{

bool isBranch(cpu::instructionID id) {
    return id == cpu::instructionID::B_cond ||
        id == cpu::instructionID::BR ||
        id == cpu::instructionID::BLR ||
        id == cpu::instructionID::RET ||
        id == cpu::instructionID::ERET ||
        id == cpu::instructionID::DRPS ||
        id == cpu::instructionID::B ||
        id == cpu::instructionID::BL ||
        id == cpu::instructionID::CBZ_32 ||
        id == cpu::instructionID::CBNZ_32 ||
        id == cpu::instructionID::CBZ_64 ||
        id == cpu::instructionID::CBNZ_64 ||
        id == cpu::instructionID::TBZ ||
        id == cpu::instructionID::TBNZ;
}

} // namespace cpu::meta