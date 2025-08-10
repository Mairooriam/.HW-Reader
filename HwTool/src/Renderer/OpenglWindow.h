#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "ImGuiLayer.h"
#include <memory>
#include "IExample.h"
#include <memory>
#include "ExampleManager.h"
#include "RenderData.h"
namespace Mir
{
    struct RenderState {
        GLenum polygonMode = GL_LINE;
        bool showWireframe = true;
        float clearColor[4] = {0.2f, 0.3f, 0.3f, 1.0f};
        size_t selectedExample;
        ExampleManager* exampleManager;
    };

    class Window
    {
    private:
        GLFWwindow* m_window; 
        std::unique_ptr<ImGuiLayer> m_imguiLayer;
        RenderState m_renderState;
        std::vector<std::unique_ptr<IExample>> m_examples;
        ExampleManager m_exampleManager;
        float m_frameTime = 0.0f;
        float m_fps = 0.0f;
        float m_frameTimeAccum = 0.0f;
        int m_frameCount = 0;
        double m_lastFrameTime = 0.0f;
    private:
        void processInput();
    public:
        Window(int w = 800, int h = 600, const char* name = "Learning OpenGL");
        void render(const HwTool::RenderData& hw);
        void addExample(std::unique_ptr<IExample> example);
        void selectExample(size_t index);
        ExampleManager& getExampleManager() { return m_exampleManager; }
        ~Window();
        ImGuiLayer& getImGuiLayer() { return *m_imguiLayer; }


        bool shouldClose() const { 
            return glfwWindowShouldClose(m_window); 
        }
    };
    

} // namespace Mir
