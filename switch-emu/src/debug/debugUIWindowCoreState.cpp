#include "debugUIWindowCoreState.h"
#include "instruction.h"
#include "disassembler.h"
#include "jitStats.h"

namespace debug::ui {

void CoreStateWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Core State");

    mCurrent = std::chrono::system_clock::now();

    // TODO proper init
    static std::chrono::time_point<std::chrono::system_clock> sStart = mCurrent;

    // update stats
    for (int coreNum = 0; coreNum < cpu::NUM_CORES; coreNum++) {
        auto core = cpu::getCore(coreNum);

        // TODO proper init
        if (mLastInst[coreNum] != 0) {
            std::chrono::duration<double> elapsed = mCurrent - mThen;
            mInstPerSec[coreNum] = (core->numInsts - mLastInst[coreNum]) / elapsed.count();

            std::chrono::duration<double> elapsedFromStart = mCurrent - sStart;
            mTotalInstPerSec[coreNum] = core->numInsts / elapsedFromStart.count();
        }

        mLastInst[coreNum] = core->numInsts;
    }

    mThen = mCurrent;

    auto glyphWidth = ImGui::CalcTextSize("FF").x - ImGui::CalcTextSize("F").x;
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, glyphWidth * 0.5f);

    if (ImGui::TreeNodeEx("Core 0", ImGuiTreeNodeFlags_CollapsingHeader)) {
        drawCore(0);
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Core 1", ImGuiTreeNodeFlags_CollapsingHeader)) {
        drawCore(1);
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Core 2", ImGuiTreeNodeFlags_CollapsingHeader)) {
        drawCore(2);
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Core 3", ImGuiTreeNodeFlags_CollapsingHeader)) {
        drawCore(3);
        ImGui::TreePop();
    }

    ImGui::PopStyleVar();

    ImGui::End();
}

void CoreStateWindow::drawCore(int coreNum) {
    auto core = cpu::getCore(coreNum);
    if (ImGui::TreeNode("GPRegs")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        for (int i = 0; i < 8; i++) {
            ImGui::Text("X%02d:%016llX X%02d:%016llX X%02d:%016llX X%02d:%016llX", i, core->GPRegs[i], i + 8, core->GPRegs[i + 8], i + 16, core->GPRegs[i + 16], i + 24, core->GPRegs[i + 24]);
        }
        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("VRegs")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        for (int i = 0; i < 32; i++) {
            ImGui::Text("Q%02d:%016llX%016llX S:%f D:%f", i, core->VRegs[i].D[1], core->VRegs[i].D[0], core->VRegs[i].floatS.val, core->VRegs[i].floatD.val);
        }
        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Other Regs")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::Text("PC:%016llX", core->PC);
        ImGui::Text("SP:%016llX", core->SP);
        ImGui::Text("N:%s Z:%s C:%s V:%s",
            (core->PSTATE.N) ? "1" : "0",
            (core->PSTATE.Z) ? "1" : "0",
            (core->PSTATE.C) ? "1" : "0",
            (core->PSTATE.V) ? "1" : "0");
        ImGui::Text("Exclusive Access:%u", core->exclusiveAccess);
        ImGui::Text("Exclusive Address:%016llX", core->exclusiveAccessAddr);
        ImGui::Text("Exclusive Data0:%016llX", core->exclusiveData0);
        ImGui::Text("Exclusive Data1:%016llX", core->exclusiveData1);
        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Stats")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::Text("%f inst/sec", mInstPerSec[coreNum]);
        ImGui::Text("%f total inst/sec", mTotalInstPerSec[coreNum]);

        // TODO better way
        // TODO this is no longer per-core, so it should be somewhere else
        using tableEntry = std::pair<std::string, uint64_t>;
        std::vector<tableEntry> interpreterTable;
        interpreterTable.reserve(static_cast<size_t>(cpu::instructionID::NUM_INSTRUCTIONS));
        for (size_t i = 0; i < static_cast<size_t>(cpu::instructionID::NUM_INSTRUCTIONS); i++) {
            cpu::instructionID id = static_cast<cpu::instructionID>(i);
            interpreterTable.push_back(std::make_pair(cpu::disassembler::getBaseInstructionName(id), cpu::jit::getFallbackCount(id)));
        }
        std::sort(interpreterTable.begin(), interpreterTable.end(), [](tableEntry l, tableEntry r) -> bool {
            return l.second > r.second;
        });

        for (auto& e : interpreterTable) {
            ImGui::Text("%s: %d", e.first.c_str(), e.second);
        }

        /*
        for (size_t i = 0; i < core->fallbackInst.size(); i++) {
        ImGui::Text("%d: %d", i, core->fallbackInst[i]);
        }
        */

        ImGui::PopStyleVar();

        ImGui::TreePop();
    }
}

} // namespace debug::ui
