#include "HwTool.h"

#include <filesystem>


#include "ModuleXmlExporter.h"
#include "ModuleMermaidExporter.h"
#include "ModuleXmlImporter.h"
#include "HardwareBuilder.h"
#include "hwUtils.h"
#include <iostream>

#include <ranges>
namespace HwTool {
    //TODO: make uses m_modules instead of supplying source.
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

    void Hw::LinkToTarget(const std::string& targetModule) {
        assert(!m_addCardBuffer.empty() && "You must call createCard() before calling LinkToTarget()");
        auto tSource = getCardSource(targetModule, m_modules);
        //TODO: bad could cause crash. make validation somewhere so cannot
        m_addCardBuffer.base.connections[0].targetModuleName = targetModule;


        m_modules[tSource].connections[0].targetModuleName = m_addCardBuffer.base.name;
        m_modules[m_addCardBuffer.base.name] = m_addCardBuffer.base;
        m_modules[m_addCardBuffer.tb.name] = m_addCardBuffer.tb;
        m_modules[m_addCardBuffer.card.name] = m_addCardBuffer.card;

        resolveLinking(m_modules);
    }

    void Hw::createCard(const std::string& name, cardType type) {
        HardwareBuilder hwb;
        auto keys = m_modules | std::views::keys;
        auto currentModules = std::set<std::string>(keys.begin(), keys.end());
        m_addCardBuffer = hwb.createCard(name, type, currentModules);
    }

    std::vector<std::string> Hw::getAvailableCards() {
        assert(!m_addCardBuffer.empty() && "You must call createCard() before calling getAvailableCards()");
        std::vector<std::string> res;
        
        auto connectors = getModuleConnectors(m_addCardBuffer.card);
        if (connectors.contains(ConnectorType::SL))
        {
            for (const auto& [name, module] : m_modules) {
                for (const auto& con : module.connections) {
                    if (con.connector == ConnectorType::SL) {
                        res.push_back(name);
                    }
                }
            }
        }
        return res;
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

    // Make more proper render API 
    void Hw::render() {
        printf("rendering\n");
        //         XMLPrinter printer;
        // doc.Print(&printer);
        // std::cout << printer.CStr();
        std::string command;
        while (true) {
        std::cout << "\nAvailable commands:\n";
        std::cout << "  list   - List all cards\n";
        std::cout << "  add    - Add a new card\n";
        std::cout << "  export - Export HW and Mermaid\n";
        std::cout << "  quit   - Exit\n";
        std::cout << "Enter command: ";
        std::cin >> command;

        if (command == "list") {

        } else if (command == "add") {
            std::string newName, targetName, typeStr;
            std::cout << "Enter new card name: ";
            std::cin >> newName;
            std::cout << "Enter card type (as int): ";
            std::cin >> typeStr;
            std::cout << "Enter target card name to link: ";
            std::cin >> targetName;
            // if (addCard(newName, magic_enum::enum_cast<cardType>(typeStr).value_or(cardType::X20DO9322), targetName)) {
            //     std::cout << "Card added and linked.\n";
            // } else {
            //     std::cout << "Failed to add card.\n";
            // }
        } else if (command == "export") {
            exportHW("interactive.hw");
            exportMermaid("interactive.md");
            std::cout << "Exported.\n";
        } else if (command == "quit") {
            break;
        } else {
            std::cout << "Unknown command.\n";
        }
    }
    }

}  // namespace HwTool
