#include "ImGuiLayer.h"
// Include UI components here instead of in the header
#include "ImGui/MenuBar.h"
#include "ImGui/ToolBar.h"
#include "ImGui/HardwareView.h"
#include "ImGui/StateInfo.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "HwTool.h"
#include <GLFW/glfw3.h>  // Add this for GLFW functions
#include <format>
#include "imgui_stdlib.h"
#include "EventTypes.h"
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

        MenuBar::render(m_eventDispatcher);
        ToolBar::render(m_eventDispatcher, state_.selectedCard);
        HardwareView::render(hw, state_.selectedCard);
        StateInfo::render(state_.selectedCard);
        }







        void ImGuiLayer::terminate() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

}  // namespace Mir