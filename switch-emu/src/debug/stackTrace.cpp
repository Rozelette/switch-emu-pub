#include "stackTrace.h"

#include "log.h"
#include "cpu.h"
#include "mem.h"
#include "hle\kernel\kernel.h"

namespace debug {

void printStackTrace() {
    cpu::Core* core = cpu::this_core::getCore();
    logger::error("Dump of core %d:", core->coreNum);
    logger::error("PC: %016llX", core->PC);
    logger::error("SP: %016llX", core->SP);
    for (int i = 0; i < 32; i++) {
        logger::error("GPREG%d: %016llX", i, core->GPRegs[i]);
    }
    for (int i = 0; i < 32; i++) {
        logger::error("Q%02d:%016llX%016llX S:%f D:%f", i, core->VRegs[i].D[1], core->VRegs[i].D[0], core->VRegs[i].floatS.val, core->VRegs[i].floatD.val);
    }

    // TODO this would be useful as a seperate dubugger window
    // TODO make this better (more accurate, currently is doesn't always find the return addr)
    logger::warn("StackTrace:");
    uint64_t SP = core->SP;
    uint64_t PC = core->PC;
    uint64_t X30 = core->GPRegs[30];
    while ((PC != 0xBADC0DE) && (PC != 0) && mem::isValid(PC) && mem::isValid(SP)) {
        logger::warn("PC:%016llX <<< %s", PC, kernel::getLoader().findNearestSymbol(PC).c_str());
        logger::warn("PC:%016llX", PC);
        PC = X30;
        X30 = mem::inspect<uint64_t>(SP + 8);
        SP = mem::inspect<uint64_t>(SP);
    }
}

} // namespace debug