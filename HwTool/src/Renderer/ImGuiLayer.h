#pragma once
#include <string>
#include "EventDispatcher.h"
#include "ImGui/HardwareView.h"

struct GLFWwindow;
namespace HwTool { struct RenderData; }

namespace Mir {
    class ImGuiLayer {
    private:
        GLFWwindow* m_window;
        EventDispatcher* m_eventDispatcher = nullptr;
        
        HardwareView m_importCsvView;
        HardwareView m_allModulesView;
        HardwareView m_cardsOnlyView;
        HardwareView m_basesOnlyView;
        HardwareView m_cpusOnlyView;
        HardwareView m_ai4622CardsView;
        HardwareView m_diModulesView;
        HardwareView m_connectedModulesView;
        
        
        public:
        struct State {
            std::string selectedCard;
        } state_;
        ImGuiLayer(GLFWwindow* window);
        ~ImGuiLayer();

        void setEventDispatcher(EventDispatcher* dispatcher);
        void begin();
        void End();
        void render(const HwTool::RenderData& hw);
        void terminate();
    };

}  // namespace Mir
