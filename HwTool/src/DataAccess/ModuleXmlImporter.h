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
        
        struct ValidationError {
            std::string message;
            std::string moduleName;
            std::string elementType;
        };
        
        template<typename T>
        struct ParseResult {
            std::optional<T> value;
            std::vector<ValidationError> errors;
            
            bool isValid() const { return value.has_value() && errors.empty(); }
            bool hasValue() const { return value.has_value(); }
            bool hasErrors() const { return !errors.empty(); }
        };

        struct ParsedType {
            enum class Kind { Base, Bus, IO, CPU, Unknown } kind;
            std::variant<V2::BaseType, V2::BusModuleType, V2::IoCardType, V2::CpuType> value;
        };

        inline static ParsedType parseModuleType(const tinyxml2::XMLElement* module) {
            std::string normalized = module->Attribute("Type") ?: "";
            if (normalized.empty())
                assert(false && "Invalid module. doesn't have type");
            
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




        class ModuleXmlImporter
        {
        public:
            ModuleXmlImporter(const std::filesystem::path& path);
            ~ModuleXmlImporter();
            ModuleMap mapModules(); // TODO: Error hanlding
            bool valid() { return m_status == ImportStatus::OK;}
            ImportStatus getStatus(){ return m_status; }




        private:
            V2::ModuleIO parseIO(tinyxml2::XMLElement* io);
            ParseResult<V2::ModuleCPU> parseCPU(tinyxml2::XMLElement* io);
            tinyxml2::XMLDocument m_doc;
            tinyxml2::XMLElement* m_hw;
            V2::ModuleMap m_modules;
            ImportStatus m_status;
        };
        
    }

}
