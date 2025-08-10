#include <iostream>
#include <HwTool.h>
#include <format>

class Application {
private:
    HwTool::Hw m_hw;
    std::unique_ptr<Mir::Window> m_window;
    bool m_needsUpdate = true;
    
public:
    Application() {

        m_window = std::make_unique<Mir::Window>(1200, 800, "HW Tool GUI");
        
        setupCallbacks();
        
        initializeData();
    }
    
    void run() {
        HwTool::RenderData renderData = m_hw.getRenderData();
        
        while (!m_window->shouldClose()) {
            if (m_needsUpdate) {
                renderData = m_hw.getRenderData(); // Ensure getRenderData returns by const reference or value
                m_needsUpdate = false;
            }
            m_window->render(renderData);
        }
    }
    
private:
    void setupCallbacks() {
        auto& imguiLayer = m_window->getImGuiLayer();
        
        imguiLayer.setImportCallback([this](const std::string& filename) {
            m_hw.importHW(filename);
            m_needsUpdate = true;
        });
        
        imguiLayer.setExportHWCallback([this](const std::string& filename) {
            m_hw.exportHW(filename);
        });
        
        imguiLayer.setExportMermaidCallback([this](const std::string& filename) {
            m_hw.exportMermaid(filename);
        });
        
        imguiLayer.setCreateCardCallback([this](const std::string& name, HwTool::cardType type, const std::string& target) {
            m_hw.createCard(name, type);
            m_hw.linkToTarget(target);
            m_needsUpdate = true;
        });
       
        //TODO: not implemented maybe dont need?
        // imguiLayer.setLinkToTargetCallback([this](const std::string& target) {
        //     m_hw.linkToTarget(target);
        // });
        
        imguiLayer.setUndoCallback([this]() {
            m_hw.undo();
        });
        
        imguiLayer.setImportCSVCallback([this](const std::string& filename, const std::string& target) {
            auto modules = m_hw.importCSV(filename);
            m_hw.combineToExisting(modules, target);
        });
    }
    
    void initializeData() {
        try {
            m_hw.importHW("hardware.hw");
        } catch (...) {
            std::cout << "No default hardware.hw file found, starting with empty project\n";
        }
    }
    

};

int main(int argc, char const *argv[])
{
    try {
        Application app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}