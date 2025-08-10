#pragma once
#include <imgui.h>

#include "DataAccess/ModuleXmlExporter.h"
#include "RenderData.h"
#include "types.h"

namespace HwTool {
    class Hw;
}

struct GLFWwindow;
namespace Mir {
    struct RenderState;

    
    
    class ImGuiLayer {
        private:
        GLFWwindow* m_window;
        std::string m_cachedString;
        HwTool::ModuleXmlExporter m_exporter;
        
        struct State{
            std::string selectedCard;
        }state_;
    public:
        ImGuiLayer(GLFWwindow* window);
        ~ImGuiLayer();
        void render(const HwTool::RenderData& hw);
        void renderCardHover(const HwTool::Module& m, const HwTool::RenderData& hw);
        void renderCardBody(const HwTool::Module& m, const HwTool::RenderData& hw);
        void renderCard(const HwTool::Module& m, const HwTool::RenderData& hw);
        void renderStateInfo();
        void terminate();
        void begin();
        void End();

    public:
        void setImportCallback(std::function<void(const std::string&)> callback) {
            m_onImport = callback;
        }

        void setExportHWCallback(std::function<void(const std::string&)> callback) {
            m_onExportHW = callback;
        }

        void setExportMermaidCallback(std::function<void(const std::string&)> callback) {
            m_onExportMermaid = callback;
        }

        void setCreateCardCallback(
            std::function<void(const std::string&, HwTool::cardType, const std::string&)> callback) {
            m_onCreateCard = callback;
        }

        void setLinkToTargetCallback(std::function<void(const std::string&)> callback) {
            //m_onLinkToTarget = callback;
            assert(false && "setLinkToTargetCallback not implemented yet");
        }

        void setUndoCallback(std::function<void()> callback) {
            m_onUndo = callback;
        }

        void setImportCSVCallback(
            std::function<void(const std::string&, const std::string&)> callback) {
            m_onImportCSV = callback;
        }

    private:
        std::function<void(const std::string&)> m_onImport;
        std::function<void(const std::string&)> m_onExportHW;
        std::function<void(const std::string&)> m_onExportMermaid;
        std::function<void(const std::string&, HwTool::cardType, const std::string& target)> m_onCreateCard;
        // TODO:: MAYBE USELESS std::function<void(const std::string&)> m_onLinkToTarget;
        std::function<void()> m_onUndo;
        std::function<void(const std::string&, const std::string&)> m_onImportCSV;
    };

}  // namespace Mir
