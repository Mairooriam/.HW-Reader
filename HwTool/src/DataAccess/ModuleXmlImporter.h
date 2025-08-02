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
    ImportError,
    ModulesMapped
};
//TODO: Rethink error hanndling
namespace HwTool
{
   class ModuleXmlImporter {
    public:

    ModuleXmlImporter(const std::filesystem::path& path);
    ~ModuleXmlImporter();
    void mapModules(); // TODO: Error hanlding

    bool valid() { return m_status == ImportStatus::OK;}
    std::vector<std::string> getErrors() const { return m_errors; }
    std::unordered_map<std::string, Module> getModules(); 
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
    namespace V2 {
        struct ParsedType {
            enum class Kind { Base, Bus, IO, CPU, Unknown } kind;
            std::variant<V2::BaseType, V2::BusModuleType, V2::IoCardType, V2::CpuType> value;
        };

        inline ParsedType parseModuleType(const std::string& typeStr) {
            std::string normalized  = typeStr;
            std::replace(normalized.begin(), normalized.end(), '-', '_');
            using namespace HwTool::V2;
            if (auto io = magic_enum::enum_cast<IoCardType>(normalized); io.has_value()) {
                return {ParsedType::Kind::IO, io.value()};
            }
            if (auto bus = magic_enum::enum_cast<BusModuleType>(normalized); bus.has_value()) {
                return {ParsedType::Kind::Bus, bus.value()};
            }
            if (auto base = magic_enum::enum_cast<BaseType>(normalized); base.has_value()) {
                return {ParsedType::Kind::Base, base.value()};
            } if (auto base = magic_enum::enum_cast<CpuType>(normalized); base.has_value()){
                return {ParsedType::Kind::CPU, base.value()};
            }
            return {ParsedType::Kind::Unknown, IoCardType::ERROR}; 
        }

        static std::tuple<std::string, std::string, std::string>
        extractModuleNameTypeVersion(const tinyxml2::XMLElement* moduleElem); 

        class ModuleXmlImporter
        {
        public:
            ModuleXmlImporter(const std::filesystem::path& path);
            ~ModuleXmlImporter();
            const ModuleMap& mapModules(); // TODO: Error hanlding
            bool valid() { return m_status == ImportStatus::OK;}
            ImportStatus getStatus(){ return m_status; }
            



        private:
            tinyxml2::XMLDocument m_doc;
            tinyxml2::XMLElement* m_hw;
            V2::ModuleMap m_modules;
            ImportStatus m_status;
        };
        
    }

}
