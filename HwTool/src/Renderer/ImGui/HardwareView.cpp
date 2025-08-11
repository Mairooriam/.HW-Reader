#include "HardwareView.h"
#include "imgui.h"
#include <format>

namespace {
    void renderCard(const HwTool::Module& m, const HwTool::RenderData& hw, std::string& selectedCard);
    void renderCardBody(const HwTool::Module& m, const HwTool::RenderData& hw);
    void renderCardHover(const HwTool::Module& m, const HwTool::RenderData& hw);
    void renderConnections(const HwTool::Module& m);
    void renderParameters(const HwTool::Module& m);
    void renderConnector(const HwTool::Module& m);
    void renderGroup(const HwTool::Module& m);
    
    void renderCard(const HwTool::Module& m, const HwTool::RenderData& hw, std::string& selectedCard) {
        std::string name = "Name: " + m.name + "\t Type: " + std::string(magic_enum::enum_name<HwTool::cardType>(m.type)) + "\t version: " + m.version + "\n";
        bool treeOpen = ImGui::TreeNode(name.c_str());
        
        if (ImGui::IsItemActive()) {
            selectedCard = m.name;
        }
        
        renderCardHover(m, hw);
        
        if (treeOpen) {
            renderCardBody(m, hw);
            ImGui::TreePop();
        }
    }
    
    void renderCardBody(const HwTool::Module& m, const HwTool::RenderData& hw) {
        renderConnections(m);
        renderParameters(m);
        renderConnector(m);
        renderGroup(m);
    }
    
    void renderConnections(const HwTool::Module& m) {
        if (!m.connections.empty() && ImGui::TreeNode("Connections")) {
            for (size_t i = 0; i < m.connections.size(); ++i) {
                const auto& conn = m.connections[i];
                std::string connLabel = "Connection " + std::to_string(i);
                if (ImGui::TreeNode(connLabel.c_str())) {
                    ImGui::Text("Connector: %s", magic_enum::enum_name(conn.connector).data());
                    ImGui::Text("Target Module: %s", conn.targetModuleName.c_str());
                    ImGui::Text("Target Connector: %s", magic_enum::enum_name(conn.targetConnector).data());
                    if (conn.targetModule) {
                        ImGui::Text("Target Module Ptr: Valid");
                    } else {
                        ImGui::Text("Target Module Ptr: nullptr");
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    
    void renderParameters(const HwTool::Module& m) {
        if (!m.parameters.empty() && ImGui::TreeNode("Parameters")) {
            for (size_t i = 0; i < m.parameters.size(); ++i) {
                const auto& param = m.parameters[i];
                ImGui::Text("ID: %s, Value: %s", param.id.c_str(), param.value.c_str());
            }
            ImGui::TreePop();
        }
    }
    
    void renderConnector(const HwTool::Module& m) {
        if (!m.connector.name.empty() && ImGui::TreeNode("Connector")) {
            ImGui::Text("Name: %s", m.connector.name.c_str());
            if (!m.connector.parameters.empty()) {
                if (ImGui::TreeNode("Connector Parameters")) {
                    for (const auto& param : m.connector.parameters) {
                        ImGui::Text("ID: %s, Value: %s", param.id.c_str(), param.value.c_str());
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    
    void renderGroup(const HwTool::Module& m) {
        if (!m.group.id.empty() && ImGui::TreeNode("Group")) {
            ImGui::Text("ID: %s", m.group.id.c_str());
            ImGui::TreePop();
        }
    }

    void renderCardHover(const HwTool::Module& m, const HwTool::RenderData& hw) {
        if (ImGui::IsItemHovered()) {
            static float hoverTime = 0.0f;
            static const void* lastHoveredItem = nullptr;
            const void* currentItem = &m; // Use module address as unique identifier
            
            if (lastHoveredItem != currentItem) {
                hoverTime = 0.0f; // Reset timer for new item
                lastHoveredItem = currentItem;
            }
            
            hoverTime += ImGui::GetIO().DeltaTime;
            
            if (hoverTime > 0.5f) { // Show tooltip after 0.5 seconds
                ImGui::BeginTooltip();
                for (size_t i = 0; i < m.connections.size(); ++i) {
                    const auto& con = m.connections[i];
                    std::string nameOfTarget = "";
                    std::string text = "";
                    if (con.connector == HwTool::ConnectorType::SL) {
                        nameOfTarget = HwTool::Utils::getBaseCard(con.targetModuleName, *hw.cacheCard);
                        text = std::format("Target_{}: {} [{}]", i + 1, con.targetModuleName, nameOfTarget);
                    } else {
                        text = std::format("Target_{}: {} ", i + 1, con.targetModuleName);
                    }
                    
                    ImGui::Text("%s", text.c_str());
                }
                ImGui::EndTooltip();
            }
        } else {
            static float hoverTime = 0.0f;
            static const void* lastHoveredItem = nullptr;
            hoverTime = 0.0f; // Reset when not hovering
            lastHoveredItem = nullptr;
        }
    }
}

namespace Mir::HardwareView {
    void render(const HwTool::RenderData& hw, std::string& selectedCard) {
        ImGui::Begin("Hardware View");
        
        for (auto&& [key, module] : *hw.cacheCard) {
            renderCard(module, hw, selectedCard);
        }
        
        ImGui::End();
    }
}