#include "HwTool.h"

#include <filesystem>


#include "ModuleXmlExporter.h"
#include "ModuleMermaidExporter.h"
#include "ModuleXmlImporter.h"
#include "HardwareBuilder.h"
#include "hwUtils.h"
#include <iostream>
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

    bool Hw::addCard(const std::string& name, cardType type, const std::string& targetName) {
        printf("adding card\n");
        HardwareBuilder hwb;
        if(! m_modules.contains(targetName))
            return false; // error card doesnt exist in modules
        
        //TODO: currently ^^^ checks for card. so does the getCardBase. reduntant maybe?
        //TODO: instead of std:String just return Module*`?HJ
        std::string targetBase = getCardBase(targetName, m_modules);
        
        if (targetBase.empty())
        {
            return false;
        }
        
        //TODO: for now just free function. later cache into hwTool during import as member val
        // ->> update it on each import and add card etc.
        //auto bases = getAllBases(m_modules); // might be actually useless. can just check against whole M_modules
        // might still be good if big HW. to cache only bases

        //std::set<std::string> afs;
        std::string nextX20BM11 = incrementStr("X20BM11", m_modules);

        //TODO:: just barely works. gotta make more dynamic etc.
        auto tSource = getCardSource(targetName, m_modules);

        Module newBM11 = hwb.X20BM11(nextX20BM11, "1.1.0.0", nullptr, targetBase);
        Module newTB12 = hwb.X20TB12(incrementStr("X20TB12", m_modules));
        Module newIO = hwb.IOCARD2(name, cardType::X20AI4622, newTB12.name, newBM11.name);
        m_modules[newBM11.name] = newBM11;
        m_modules[newTB12.name] = newTB12;
        m_modules[newIO.name] = newIO;

        //TODO: just barely works gotta make more dynamic
        //TODO: test with invalid inputs etc.
        m_modules[tSource].connections[0].targetModuleName = newBM11.name;
        
        resolveLinking(m_modules);

        return true;
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
            if (addCard(newName, magic_enum::enum_cast<cardType>(typeStr).value_or(cardType::X20DO9322), targetName)) {
                std::cout << "Card added and linked.\n";
            } else {
                std::cout << "Failed to add card.\n";
            }
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
