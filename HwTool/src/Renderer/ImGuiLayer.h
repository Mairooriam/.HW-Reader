#pragma once
#include <imgui.h>
#include "DataAccess/ModuleXmlExporter.h"
namespace HwTool { class Hw; }

struct GLFWwindow;
namespace Mir
{
    struct RenderState;

    class ImGuiLayer
    {
    private:
        GLFWwindow* m_window;
        std::string m_cachedString;
        HwTool::ModuleXmlExporter m_exporter;
    public:
        ImGuiLayer(GLFWwindow* window);
        ~ImGuiLayer();
        void render(HwTool::Hw& hw);
        void terminate();
        void begin();
        void End();
    };

} // namespace 
