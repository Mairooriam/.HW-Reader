#include "MenuBar.h"
#include "imgui.h"
#include "EventTypes.h"

namespace {
    void renderFileMenu(EventDispatcher* dispatcher) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Import HW")) {
                if (dispatcher) {
                    dispatcher->notify(Event::ImportHW{"hardware.hw"});
                }
            }
            
            if (ImGui::MenuItem("Export HW")) {
                if (dispatcher) {
                    dispatcher->notify(Event::ExportHW{"exported_hardware.hw"});
                }
            }
            
            if (ImGui::MenuItem("Export Mermaid")) {
                if (dispatcher) {
                    dispatcher->notify(Event::ExportMermaid{"diagram.mmd"});
                }
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Import CSV")) {
                if (dispatcher) {
                    dispatcher->notify(Event::ImportCSV{"HW_version-1.0.0.csv", "target_module"});
                }
            }
            
            ImGui::EndMenu();
        }
    }
    
    void renderEditMenu(EventDispatcher* dispatcher) {
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                if (dispatcher) {
                    dispatcher->notify(Event::Undo{});
                }
            }
            ImGui::EndMenu();
        }
    }
}

namespace Mir::MenuBar {
    void render(EventDispatcher* dispatcher) {
        if (ImGui::BeginMainMenuBar()) {
            renderFileMenu(dispatcher);
            renderEditMenu(dispatcher);
            ImGui::EndMainMenuBar();
        }
    }
}