#include <array>

#include "encode.h"

namespace cpu {

namespace encoder {

#define INST(id, name, args, mask, encoding, ...) encoding,
std::array<uint32_t, static_cast<size_t>(instructionID::NUM_INSTRUCTIONS)> gEncodeInfo = {
    0, // UNDIFINED
#include "instructions.inl"
};
#undef INST

uint32_t encode(instructionID id) {
    if (static_cast<size_t>(id) >= gEncodeInfo.size()) {
        return 0;
    }
    return gEncodeInfo[static_cast<size_t>(id)];
}

} // namespace encoder

} // namespace cpu