#include "ToolBar.h"
#include "imgui.h"
#include "EventTypes.h"
#include "HwTool.h"
#include <cstring>

namespace {
    void renderCardCreation(EventDispatcher* dispatcher, const std::string& selectedCard) {
        ImGui::Text("Create New Card");
        
        static char cardName[256] = "";
        ImGui::InputText("Card Name", cardName, sizeof(cardName));
        
        static int cardTypeSelection = 0;
        const char* cardTypes[] = { "Module" };
        ImGui::Combo("Card Type", &cardTypeSelection, cardTypes, IM_ARRAYSIZE(cardTypes));
        
        if (!selectedCard.empty()) {
            ImGui::Text("Target: %s", selectedCard.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No target selected");
        }
        
        if (ImGui::Button("Create Card")) {
            if (strlen(cardName) > 0 && dispatcher) {
                HwTool::cardType type = static_cast<HwTool::cardType>(cardTypeSelection);
                dispatcher->notify(Event::CreateCard{std::string(cardName), type, selectedCard});
                cardName[0] = '\0';
            }
        }
        
        if (strlen(cardName) == 0) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Enter a name");
        }
    }
    
    void renderQuickActions(EventDispatcher* dispatcher) {
        ImGui::Text("Quick Actions");
        
        if (ImGui::Button("Undo Last Action")) {
            if (dispatcher) {
                dispatcher->notify(Event::Undo{});
            }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Import CSV")) {
            if (dispatcher) {
                dispatcher->notify(Event::ImportCSV{"HW_version-1.0.0.csv", ""});
            }
        }
    }
}

namespace Mir::ToolBar {
    void render(EventDispatcher* dispatcher, const std::string& selectedCard) {
        ImGui::Begin("Toolbar");
        
        renderCardCreation(dispatcher, selectedCard);
        ImGui::Separator();
        renderQuickActions(dispatcher);
        
        ImGui::End();
    }
}