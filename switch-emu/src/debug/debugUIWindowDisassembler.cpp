#include <sstream>

#include "debugUI.h"
#include "types.h"
#include "cpu.h"
#include "disassembler.h"
#include "decode.h"
#include "mem.h"
#include "analysis.h"
#include "hle\kernel\kernel.h"
#include "debug.h"
#include "meta.h"
#include "debugUIWindowDisassembler.h"

namespace debug::ui {

void DisassemblerWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Disassembly");

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGuiIO& io = ImGui::GetIO();
    auto drawList = ImGui::GetWindowDrawList();
    auto core = cpu::getCore(0); // TODO handle other cores

                                 // Invalidate the selected address if it somehow got messed up
    if (!mem::isValid(mSelectedAddr)) {
        mSelectedAddr = INVALID_SELECTION;
    }

    if ((mSelectedAddr & 0x3) != 0) {
        mSelectedAddr = INVALID_SELECTION;
    }

    if (ImGui::IsWindowFocused()) {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) || (io.MouseWheel > 0.0)) {
            if (mBaseAddr > 0) {
                mBaseAddr -= 4;
            }
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) || (io.MouseWheel < 0.0)) {
            if (mBaseAddr < 0xFFFFFFFFFFFFFF80) {
                mBaseAddr += 4;
            }
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp))) {
            if (mBaseAddr < 4 * 32) {
                mBaseAddr = 0;
            } else {
                mBaseAddr -= 4 * 32;
            }
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown))) {
            if (mBaseAddr > 0xFFFFFFFFFFFFFF00) {
                mBaseAddr = 0xFFFFFFFFFFFFFF80;
            } else {
                mBaseAddr += 4 * 32;
            }
        }

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z))) {
            cpu::sendInterrupts(0, cpu::DEBUG_BREAK_INTERRUPT);
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X))) {
            debug::resumeFromDebugBreak();
        }

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A))) {
            // TODO step only current core?
            // TODO move cpu logic into resumeFromDebugBreakForSingleStep?
            cpu::setSingleStepMode(true);
            debug::resumeFromDebugBreakForSingleStep();
        }

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V))) {
            cpu::addSingleUseBreakpoint(core->PC + 4);
            // TODO different func for step over?
            debug::resumeFromDebugBreakForSingleStep();
        }

        // When enter is pressed and a branch is selected, jump to the branch's target
        if ((mSelectedAddr != INVALID_SELECTION) && mem::isValid(mSelectedAddr)) {
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
                auto bits = mem::inspect<uint32_t>(mSelectedAddr);
                cpu::DecodedInstruction inst;
                cpu::decoder::decode(bits, inst);

                if (cpu::meta::isBranch(inst.id)) {
                    auto branchInfo = analysis::getBranchInfo(inst, mSelectedAddr, core);
                    if (!branchInfo.isVariable) {
                        mBaseAddr = branchInfo.target;
                        mSelectedAddr = branchInfo.target;
                    }
                }
            }
        }

        // Clear the selected address if the user has clicked. This avoids highlighting the old selection
        if (ImGui::IsMouseClicked(0) || ImGui::IsMouseDown(0)) {
            mSelectedAddr = INVALID_SELECTION;
        }
    }

    mBaseAddr &= ~0x3;
    mBaseAddr = std::min(mBaseAddr, 0xFFFFFFFFFFFFFF80);

    for (auto i = 0llu; i < 32; i++) {
        uint64_t addr = i * 4 + mBaseAddr;

        ImGui::Text("%s", (addr == core->PC) ? ">" : " ");
        ImGui::SameLine();

        ImGui::Text("%016llX: ", addr);
        ImGui::SameLine();

        if (mem::isValid(addr)) {
            auto bits = mem::inspect<uint32_t>(addr);
            cpu::DecodedInstruction inst;
            cpu::decoder::decode(bits, inst);

            auto info = analysis::get(addr);

            auto rootPos = ImGui::GetCursorScreenPos();
            auto boxMin = ImVec2{ rootPos.x, rootPos.y };
            auto boxMax = ImVec2{ rootPos.x + ImGui::GetWindowContentRegionWidth(), rootPos.y + ImGui::GetTextLineHeight() };

            if (ImGui::IsMouseHoveringRect(boxMin, boxMax)) {
                if (ImGui::IsMouseClicked(0) || ImGui::IsMouseDown(0)) {
                    mSelectedAddr = addr;
                }
            }

            if (mSelectedAddr == addr) {
                drawList->AddRectFilled(boxMin, boxMax, mSelectCol);
            }

            cpu::Disassembly dis;
            if (cpu::disassembler::disassemble(inst, dis, addr)) {
                ImGui::Text("%-10s %-30s", dis.name.c_str(), dis.args.c_str());
            } else {
                ImGui::Text("INVALID");
            }

            if (cpu::meta::isBranch(inst.id)) {
                auto branchInfo = analysis::getBranchInfo(inst, addr, core);
                if (!branchInfo.isVariable) {
                    ImGui::SameLine();
                    ImGui::Text(" ; <%s>", kernel::getLoader().findNearestSymbol(branchInfo.target).c_str());
                }
            }

            std::string* funcName = kernel::getLoader().findSymbol(addr);
            if (funcName) {
                ImGui::SameLine();
                ImGui::Text(" ; <<< %s", funcName->c_str());
            }

            if (info.func) {
                ImGui::SameLine();
                ImGui::Text(" ; %s", info.func->name.c_str());
            }
        } else {
            ImGui::Text("???");
        }
    }

    ImGui::Separator();

    ImGui::Text("Go to address:");
    ImGui::SameLine();

    if (ImGui::InputText("", mAddrInput, 20, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::istringstream is(mAddrInput);

        uint64_t newAddr;
        is >> std::hex >> newAddr;

        newAddr -= (newAddr % 4);
        mBaseAddr = newAddr;
    }

    if (ImGui::Button("Go To PC")) {
        mBaseAddr = core->PC;
    }

    ImGui::PopStyleVar();

    ImGui::End();
}

} // namespace debug::ui