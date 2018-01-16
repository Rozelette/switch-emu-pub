#include <sstream>

#include "debugUIWindowMemory.h"
#include "mem.h"

namespace debug::ui {

void MemoryWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Memory");

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGuiIO& io = ImGui::GetIO();

    if (ImGui::IsWindowFocused()) {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) || (io.MouseWheel > 0.0)) {
            if (mBaseAddr > 0) {
                mBaseAddr -= 16;
            }
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) || (io.MouseWheel < 0.0)) {
            if (mBaseAddr < 0xFFFFFFFFFFFFFF00) {
                mBaseAddr += 16;
            }
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp))) {
            if (mBaseAddr < 16 * 16) {
                mBaseAddr = 0;
            } else {
                mBaseAddr -= 16 * 16;
            }
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown))) {
            if (mBaseAddr > 0xFFFFFFFFFFFFFE00) {
                mBaseAddr = 0xFFFFFFFFFFFFFF00;
            } else {
                mBaseAddr += 16 * 16;
            }
        }
    }

    mBaseAddr &= ~0xF;
    mBaseAddr = std::min(mBaseAddr, 0xFFFFFFFFFFFFFF00);

    for (auto i = 0llu; i < 16; i++) {
        uint64_t addr = i * 16 + mBaseAddr;
        ImGui::Text("%016llX: ", addr);

        for (int n = 0; n < 16; n++) {
            ImGui::SameLine();
            if (mem::isValid(addr)) {
                ImGui::Text("%02X ", mem::inspect<uint8_t>(addr + n));
            } else {
                ImGui::Text("?? ");
            }
        }

        ImGui::SameLine();
        ImGui::Text("| ");

        for (int n = 0; n < 16; n++) {
            ImGui::SameLine();
            if (mem::isValid(addr)) {
                uint8_t data = mem::inspect<uint8_t>(addr + n);
                ImGui::Text("%c", ((data > 31) && (data < 127)) ? data : '.');
            } else {
                ImGui::Text(".");
            }
        }
    }

    ImGui::Separator();

    ImGui::Text("Go to address:");
    ImGui::SameLine();

    if (ImGui::InputText("", mAddrInput, 20, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::istringstream is(mAddrInput);

        uint64_t newAddr;
        is >> std::hex >> newAddr;

        newAddr -= (newAddr % 16);
        mBaseAddr = newAddr;
    }

    ImGui::PopStyleVar();

    ImGui::End();
}

} // namespace debug::ui