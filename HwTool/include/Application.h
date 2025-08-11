#include <iostream>
#include <HwTool.h>
#include <format>
#include "EventDispatcher.h"
#include "Renderer/OpenglWindow.h" 
#include "EventTypes.h"
class Application {
private:
    HwTool::Hw m_hw;
    std::unique_ptr<Mir::Window> m_window;
    bool m_needsUpdate = true;
    EventDispatcher m_dispatcher; 

public:
    Application() {
        m_window = std::make_unique<Mir::Window>(1200, 800, "HW Tool GUI");
        setupCallbacks();
        initializeData();
        m_window->getImGuiLayer().setEventDispatcher(&m_dispatcher);
    }

    void run() {
        HwTool::RenderData renderData = m_hw.getRenderData();
        while (!m_window->shouldClose()) {
            if (m_needsUpdate) {
                renderData = m_hw.getRenderData();
                m_needsUpdate = false;
            }
            m_window->render(renderData);
        }
    }

private:
    void setupCallbacks() {
        m_dispatcher.subscribe<Event::ImportHW>([this](const Event::ImportHW& e) {
            m_hw.importHW(e.filename);
            m_needsUpdate = true;
        });

        m_dispatcher.subscribe<Event::CreateCard>([this](const Event::CreateCard& e) {
            m_hw.createCard(e.name, e.type);
            m_hw.linkToTarget(e.target);
            m_needsUpdate = true;
        });

        m_dispatcher.subscribe<Event::ExportHW>([this](const Event::ExportHW& e) {
            m_hw.exportHW(e.filename);
        });

        m_dispatcher.subscribe<Event::ExportMermaid>([this](const Event::ExportMermaid& e) {
            m_hw.exportMermaid(e.filename);
        });

        m_dispatcher.subscribe<Event::Undo>([this](const Event::Undo&) {
            m_hw.undo();
            m_needsUpdate = true;
        });

        m_dispatcher.subscribe<Event::ImportCSV>([this](const Event::ImportCSV& e) {
            auto modules = m_hw.importCSV(e.filename);
            //m_hw.combineToExisting(modules, e.target);
            m_needsUpdate = true;
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