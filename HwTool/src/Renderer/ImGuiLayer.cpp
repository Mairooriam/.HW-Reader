#include "ImGuiLayer.h"
#include "OpenglWindow.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "HwTool.h"
#include <format>
#include "imgui_stdlib.h"
#include <sstream>
namespace Mir
{
    ImGuiLayer::ImGuiLayer(GLFWwindow* window): m_window(window) {
        // Setup Dear ImGui context - make sure we're saving this context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplGlfw_SetCallbacksChainForAllWindows(true); 
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    ImGuiLayer::~ImGuiLayer() {

    }
    
    void ImGuiLayer::begin() {
        // Make sure we're operating on the right ImGui context before starting a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    
    void ImGuiLayer::End(){
        ImGuiIO& io = ImGui::GetIO();
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        io.DisplaySize = ImVec2((float)width, (float)height);
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

            glfwMakeContextCurrent(backup_current_context);
        }
    }
    
    void ImGuiLayer::render(const HwTool::RenderData& hw) {
        ImGui::ShowDemoWindow();
        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); 
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); 
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        ImGui::PopStyleColor(2); 
        renderStateInfo();
        // Main menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Import HW")) {
                    if (m_onImport) {
                        m_onImport("hardware.hw");
                    }
                }
                
                if (ImGui::MenuItem("Export HW")) {
                    if (m_onExportHW) {
                        m_onExportHW("exported_hardware.hw");
                    }
                }
                
                if (ImGui::MenuItem("Export Mermaid")) {
                    if (m_onExportMermaid) {
                        m_onExportMermaid("diagram.mmd");
                    }
                }
                
                ImGui::Separator();
                
                if (ImGui::MenuItem("Import CSV")) {
                    if (m_onImportCSV) {
                        m_onImportCSV("data.csv", "target_module");
                    }
                }
                
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                    if (m_onUndo) {
                        m_onUndo();
                    }
                }
                ImGui::EndMenu();
            }
            
                ImGui::EndMainMenuBar();
            }

            ImGui::Begin("Toolbar");
            
            static char cardName[256] = "";
            ImGui::InputText("Card Name", cardName, sizeof(cardName));
            
            static int cardTypeSelection = 0;
            const char* cardTypes[] = { "Module" };
            ImGui::Combo("Card Type", &cardTypeSelection, cardTypes, IM_ARRAYSIZE(cardTypes));
            
            if (ImGui::Button("Create Card")) {
                if (strlen(cardName) > 0 && m_onCreateCard) {
                    HwTool::cardType type = static_cast<HwTool::cardType>(cardTypeSelection);
                    m_onCreateCard(std::string(cardName), type, state_.selectedCard);
                    cardName[0] = '\0';
                }
            }
            

            
            
            ImGui::End();

            ImGui::Begin("Hardware View");
            
            for (auto &&v : hw.cardChain)
            {
                auto it = hw.modules->find(v);
                if (it != hw.modules->end()) {
                    renderCard(it->second, hw);
                }
            }
            ImGui::End();
        }
        void ImGuiLayer::renderCardHover(const HwTool::Module& m, const HwTool::RenderData& hw ) {
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
                        if (con.connector == HwTool::ConnectorType::SL)
                        {
                            nameOfTarget = HwTool::Utils::getBaseCard(con.targetModuleName, *hw.cacheCard);
                            text = std::format("Target_{}: {} [{}]", i + 1, con.targetModuleName, nameOfTarget);
                        }else{

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

        void ImGuiLayer::renderCardBody(const HwTool::Module& m, const HwTool::RenderData& hw ) {
            // Connections
            if (!m.connections.empty() && ImGui::TreeNode("Connections"))
            {
                for (size_t i = 0; i < m.connections.size(); ++i) {
                    const auto& conn = m.connections[i];
                    std::string connLabel = "Connection " + std::to_string(i);
                    if (ImGui::TreeNode(connLabel.c_str()))
                    {
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
            
            // Parameters
            if (!m.parameters.empty() && ImGui::TreeNode("Parameters"))
            {
                for (size_t i = 0; i < m.parameters.size(); ++i) {
                    const auto& param = m.parameters[i];
                    ImGui::Text("ID: %s, Value: %s", param.id.c_str(), param.value.c_str());
                }
                ImGui::TreePop();
            }
            
            // Connector
            if (!m.connector.name.empty() && ImGui::TreeNode("Connector"))
            {
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
            
            // Group
            if (!m.group.id.empty() && ImGui::TreeNode("Group"))
            {
                ImGui::Text("ID: %s", m.group.id.c_str());
                ImGui::TreePop();
            }
        }

        void ImGuiLayer::renderCard(const HwTool::Module& m, const HwTool::RenderData& hw ) {
            std::string name = "Name: " + m.name + "\t Type: " + std::string(magic_enum::enum_name<HwTool::cardType>(m.type)) + "\t version: " + m.version + "\n";
            bool treeOpen = ImGui::TreeNode(name.c_str());
            if(ImGui::IsItemActive()){
                state_.selectedCard = m.name;
            }
            renderCardHover(m, hw);
            
            if (treeOpen) {
                renderCardBody(m, hw);
                ImGui::TreePop();
            }

        }

        void ImGuiLayer::renderStateInfo() {
            ImGui::Begin("State Info");

            ImGui::Text("Selected Card:");
            if (!state_.selectedCard.empty()) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", state_.selectedCard.c_str());
            } else {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "None");
            }
            
            ImGui::Separator();
            ImGui::End();
        }

        void ImGuiLayer::terminate() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

}  // namespace Mir