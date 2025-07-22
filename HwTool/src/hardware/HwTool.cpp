#include "HwTool.h"

#include <filesystem>


#include "ModuleXmlExporter.h"
#include "ModuleMermaidExporter.h"
#include "ModuleXmlImporter.h"
#include <iostream>
namespace HwTool {
    void Hw::resolveLinking(std::unordered_map<std::string, Module>& modules) {
            for (auto& [name, module] : modules) {
                for (auto& con : module.connections) {
                    if (!con.targetModuleName.empty()) {
                        auto it = modules.find(con.targetModuleName);
                        if (it != modules.end()) {
                            con.targetModule = &it->second;
                        } else {
                            std::cerr << "Warning: Target module '" << con.targetModuleName
                                    << "' not found for connection in module '" << name << "'\n";
                            con.targetModule = nullptr;
                        }
                    }
                }
            }
        }       


    Hw::Hw() {}

    Hw::~Hw() {}

    void Hw::importCSV(const std::filesystem::path& path, const std::string& version) {
        printf("Importing CSV\n");
    }

    void Hw::importHW(const std::filesystem::path& path, const std::string& version) {
        printf("importing HW\n");

        ModuleXmlImporter importer(path);
        if (importer.valid())
        {
            importer.mapModules();
            m_modules = importer.getModules();

        }else{
            importer.printErrors();
        }
        resolveLinking(m_modules);
        //TODO: status printing if it was succesfull etc.
    }

    bool Hw::addCard(const std::string& name, cardType type, const std::string& targetName) {
        printf("adding card\n");
        return false;
    }

    void Hw::exportHW(const std::filesystem::path& path) {
        printf("Exporting HW\n");
        ModuleXmlExporter exporter;
        exporter.serialize(m_modules, path.string());
    }

    void Hw::exportMermaid(const std::filesystem::path& path) {
        printf("Exporting to mermaid\n");
        ModuleMermaidExporter exporter(path);
        exporter.serialize(m_modules);
    }

    void Hw::render() {
        printf("rendering\n");
        //         XMLPrinter printer;
        // doc.Print(&printer);
        // std::cout << printer.CStr();
    }

}  // namespace HwTool
