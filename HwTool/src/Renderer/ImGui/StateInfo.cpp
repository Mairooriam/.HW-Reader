#include "StateInfo.h"
#include "imgui.h"

namespace Mir::StateInfo {
    void render(const ImGuiLayer::State& state) {
        ImGui::Begin("State Info");

        ImGui::Text("Selected Card:");
        if (!state.selectedCard.empty()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", state.selectedCard.c_str());
        } else {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "None");
        }
        


        ImGui::Separator();
        ImGui::End();
    }
}