#include "ImGuiLayer.h"
#include "OpenglWindow.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "HwTool.h"

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
        
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Handle platform viewports (if enabled)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            // Need to save the current context
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            
            // Make sure all platform windows update properly
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            
            // Restore the GL context to avoid problems
            glfwMakeContextCurrent(backup_current_context);
        }
    }
    
    void ImGuiLayer::render(HwTool::Hw& hw) {
        ImGui::ShowDemoWindow();
    ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); 
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); 
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    ImGui::PopStyleColor(2); 
        ImGui::Begin("testing");
            if(ImGui::Button("Import Hw")){
                hw.importHW("hardware.hw");
                std::stringstream ss;
                m_exporter.serialize(hw.getCardCache(), ss);
                m_cachedString = ss.str();
            }
    
            ImGui::Text(m_cachedString.c_str());

        ImGui::End();

    }
    void ImGuiLayer::terminate() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}  // namespace Mir