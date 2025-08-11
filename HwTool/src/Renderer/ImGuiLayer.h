#pragma once
#include <string>
#include "EventDispatcher.h"


struct GLFWwindow;
namespace HwTool { struct RenderData; }

namespace Mir {
    class ImGuiLayer {
    private:
        GLFWwindow* m_window;
        EventDispatcher* m_eventDispatcher = nullptr;
        

        
        struct State {
            std::string selectedCard;
        } state_;

    public:
        ImGuiLayer(GLFWwindow* window);
        ~ImGuiLayer();
        
        void setEventDispatcher(EventDispatcher* dispatcher);
        void begin();
        void End();
        void render(const HwTool::RenderData& hw);
        void terminate();
    };

}  // namespace Mir
