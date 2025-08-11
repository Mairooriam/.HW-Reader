#include "StateInfo.h"
#include "imgui.h"

namespace Mir::StateInfo {
    void render(const std::string& selectedCard) {
        ImGui::Begin("State Info");

        ImGui::Text("Selected Card:");
        if (!selectedCard.empty()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", selectedCard.c_str());
        } else {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "None");
        }
        
        ImGui::Separator();
        ImGui::End();
    }
}