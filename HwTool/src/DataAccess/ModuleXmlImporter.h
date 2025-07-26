#pragma once
#include <unordered_map>
#include "Types.h"
#include <tinyxml2.h>
#include <filesystem>
#include <vector>
#include <string>
//TODO: for now enum later bitwise flags?
enum class ImportStatus {
    OK,
    NotStarted,
    Success,
    FileNotFound,
    XmlParseError,
    NoHardwareElement,
    VersionMismatch,
    ImportError
};

namespace HwTool
{
   class ModuleXmlImporter {
    public:

    ModuleXmlImporter(const std::filesystem::path& path);
    ~ModuleXmlImporter();
    void mapModules(); // TODO: Error hanlding

    bool valid() { return m_status == ImportStatus::OK;}
    std::vector<std::string> getErrors();
    std::unordered_map<std::string, Module> getModules() { return m_modules;}
    void printErrors();

    // In the future, you can add:
    // void validateModules();
private:
    tinyxml2::XMLDocument m_doc;
    tinyxml2::XMLElement* m_hw;
    ImportStatus m_status;
    std::vector<std::string> m_errors;
    std::unordered_map<std::string, Module> m_modules;
    
};
}
