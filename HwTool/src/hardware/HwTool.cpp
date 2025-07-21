#include "HwTool.h"

#include <filesystem>


#include "ModuleXmlExporter.h"
#include "ModuleXmlImporter.h"
#include <iostream>
#include "moduleUtils.h"
namespace HwTool {
    Hw::Hw() {}

    Hw::~Hw() {}

    void Hw::importCSV(const std::filesystem::path& path, const std::string& version) {
        printf("Importing CSV\n");
    }

    void Hw::importHW(const std::filesystem::path& path, const std::string& version) {
        printf("importing HW\n");

        ModuleXmlImporter importer(path);
        std::unordered_map<std::string, Module> modules_debug;
        if (importer.valid())
        {
            importer.mapModules();
            modules_debug = importer.getModules();

        }else{
            importer.printErrors();
        }
        // TODO: better place for resolve linking since its used in many places
        resolveLinking(modules_debug);
        std::cout << modules_debug << "\n";
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
    }

    void Hw::render() {
        printf("rendering\n");
        //         XMLPrinter printer;
        // doc.Print(&printer);
        // std::cout << printer.CStr();
    }

}  // namespace HwTool
