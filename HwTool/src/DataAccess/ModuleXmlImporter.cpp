#include "ModuleXmlImporter.h"

#include <tinyxml2.h>

#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <print>
#include <variant>

#include "HardwareBuilder.h"

using namespace tinyxml2;
namespace HwTool {
    namespace V2 {

        ModuleXmlImporter::ModuleXmlImporter(const std::filesystem::path& path) {
            if (!std::filesystem::exists(path)) {
                m_status = ImportStatus::FileNotFound;
                std::cout << "File does not exist: " << path.string() << std::endl;
                return;
            }
            if (!std::filesystem::is_regular_file(path)) {
                m_status = ImportStatus::FileNotFound;
                std::cout << "Not a regular file: " << path.string() << std::endl;
                return;
            }

            tinyxml2::XMLError result = m_doc.LoadFile(path.string().c_str());
            if (result != tinyxml2::XML_SUCCESS) {
                m_status = ImportStatus::XmlParseError;
                std::cout << "TinyXML2 error (" << result
                          << "): " << (m_doc.ErrorStr() ? m_doc.ErrorStr() : "Unknown error")
                          << std::endl;
                return;
            }

            // TODO: Validate version etc.
            //  <?AutomationStudio Version=6.0.2.177 FileVersion="4.9"?>

            m_hw = m_doc.FirstChildElement("Hardware");
            if (!m_hw) {
                m_status = ImportStatus::NoHardwareElement;
                std::cout << "No <Hardware> element found in file: " << path.string() << std::endl;
                return;
            }
        }
        ModuleXmlImporter::~ModuleXmlImporter() {}

        // TODO: add error handling for each branch
        ModuleMap ModuleXmlImporter::mapModules() {
            ModuleMap modules;
            std::unordered_map<std::string, std::string> link;

            for (auto moduleElem = m_hw->FirstChildElement("Module"); moduleElem;
                    moduleElem = moduleElem->NextSiblingElement("Module")) {
                ParsedType type = parseModuleType(moduleElem);

                if (type.kind == ParsedType::Kind::IO) {
                    auto io = parseIO(moduleElem);
                    modules[io.name] = std::move(io);
                } else if (type.kind == ParsedType::Kind::CPU) {
                    auto cpu = parseCPU(moduleElem);

                    for (const auto& error : cpu.errors) {
                        std::cerr << "CPU Parse Error: " << error.message 
                                << " (Module: " << error.moduleName << ")" << std::endl;
                    }
                    
                    if (cpu.hasValue()) {
                        modules[cpu.value->name] = std::move(*cpu.value);
                    } else {
                        std::cerr << "Failed to parse CPU module - skipping" << std::endl;
                    }
                    
                    printf("cpu");
                } else if (type.kind == ParsedType::Kind::Base)
                {
                    printf("hello");
                    const char* name = moduleElem->Attribute("Name");
                    const char* target = moduleElem->FirstChildElement()->Attribute("TargetModule");

                    if (name && target)
                    {
                        link[name] = target;
                    }
                    

                    
                }else if (type.kind == ParsedType::Kind::Bus)
                {
                    assert(false && "Not implemented yeat");
                }else {
                    printf("not implemented yeat");
                }
            }


            for (auto &&[k,v] : modules)
            {
                if (std::holds_alternative<ModuleIO>(v))
                {
                    auto& io = std::get<ModuleIO>(v);
                    std::string target = link[io.base];
                    io.nextModuleName = target;
                    
                }else if (std::holds_alternative<ModuleCPU>(v))
                {

                }else if (std::holds_alternative<ModuleBUS>(v))
                {
                    assert(false && "not implemented");
                }else
                {
                    assert(false && "not implemented");
                }
                
                
                
                
            }
            

            return modules;
        }

        V2::ModuleIO ModuleXmlImporter::parseIO(tinyxml2::XMLElement* io) {
            return V2::ModuleIO(io->Attribute("Name"),
                    magic_enum::enum_cast<IoCardType>(io->Attribute("Type"))
                            .value_or(IoCardType::ERROR),
                    io->Attribute("Version"),
                    io->FirstChildElement()->Attribute("TargetModule"),
                    io->LastChildElement()->Attribute("TargetModule"));
        }

        ParseResult<V2::ModuleCPU> ModuleXmlImporter::parseCPU(tinyxml2::XMLElement* io) {

            ParseResult<V2::ModuleCPU> result;
            
            // Validate required attributes
            const char* nameAttr = io->Attribute("Name");
            std::string typeAttr = io->Attribute("Type");
            const char* versionAttr = io->Attribute("Version");
            
            if (!nameAttr) {
                result.errors.push_back({"Missing required attribute 'Name'", "", "CPU"});
                return result;
            }
            
            std::string name = nameAttr;
            
            if (typeAttr.empty()) {
                result.errors.push_back({"Missing required attribute 'Type'", name, "CPU"});
            }
            
            if (!versionAttr) {
                result.errors.push_back({"Missing required attribute 'Version'", name, "CPU"});
            }
            
            // Parse type with validation
            std::replace(typeAttr.begin(), typeAttr.end(), '-', '_');
            auto cpuType = magic_enum::enum_cast<CpuType>(typeAttr).value_or(CpuType::ERROR);
            if (cpuType == CpuType::ERROR && typeAttr.empty()) {
                result.errors.push_back({"Invalid CPU type: " + std::string(typeAttr), name, "CPU"});
            }
            
            std::string version = versionAttr ?: "";
            std::string base = "";
            V2::Connector connector;
            std::unordered_map<std::string, std::string> parameters;
            std::string group = "";
            std::string nextModuleName = "";
            
            // Parse child elements
            for (auto childElem = io->FirstChildElement(); childElem;
                childElem = childElem->NextSiblingElement()) {
                
                std::string childName = childElem->Name();
                
                if (childName == "Connection") {
                    const char* targetModule = childElem->Attribute("TargetModule");
                    if (targetModule) {
                        base = targetModule;  
                    } else {
                        result.errors.push_back({"Connection element missing TargetModule attribute", name, "CPU"});
                    }
                    
                } else if (childName == "Connector") {
                    connector.name = childElem->Attribute("Name") ?: "";
                    
                    // Parse connector parameters
                    for (auto paramElem = childElem->FirstChildElement("Parameter"); paramElem;
                        paramElem = paramElem->NextSiblingElement("Parameter")) {
                        
                        const char* id = paramElem->Attribute("ID");
                        const char* value = paramElem->Attribute("Value");
                        if (id && value) {
                            connector.parameters[id] = value;
                        } else {
                            result.errors.push_back({"Connector Parameter missing ID or Value", name, "CPU"});
                        }
                    }
                    
                } else if (childName == "Parameter") {
                    const char* id = childElem->Attribute("ID");
                    const char* value = childElem->Attribute("Value");
                    if (id && value) {
                        parameters[id] = value;
                    } else {
                        result.errors.push_back({"Parameter missing ID or Value", name, "CPU"});
                    }
                    
                } else if (childName == "Group") {
                    const char* groupId = childElem->Attribute("ID");
                    if (groupId) {
                        group = groupId;
                    } else {
                        result.errors.push_back({"Group element missing ID attribute", name, "CPU"});
                    }
                } else {
                    result.errors.push_back({"Unexpected child element: " + childName, name, "CPU"});
                }
            }
            
            if (base.empty()) {
                result.errors.push_back({"CPU module has no base connection", name, "CPU"});
            }
        
            if (result.errors.empty() || cpuType != CpuType::ERROR) {
                result.value = V2::ModuleCPU(
                    name,
                    cpuType, 
                    version,
                    base,
                    connector,
                    parameters,
                    group,
                    nextModuleName
                );
            }
            
            return result;
        }

        // TODO: add error handling
        std::tuple<std::string, std::string, std::string> extractModuleNameTypeVersion(
                const tinyxml2::XMLElement* moduleElem) {
            std::string name, type, version;
            for (const tinyxml2::XMLAttribute* attr = moduleElem->FirstAttribute(); attr;
                    attr = attr->Next()) {
                std::string attrName = attr->Name();
                std::string attrValue = attr->Value();
                if (attrName == "Name") {
                    name = attrValue;
                } else if (attrName == "Type") {
                    type = attrValue;
                } else if (attrName == "Version") {
                    version = attrValue;
                }
            }
            return {name, type, version};
        }

    }  // namespace V2
    ModuleXmlImporter::ModuleXmlImporter(const std::filesystem::path& path) {
        m_status = ImportStatus::OK;

        if (!std::filesystem::exists(path)) {
            m_status = ImportStatus::FileNotFound;
            m_errors.push_back("File does not exist: " + path.string());
            return;
        }
        if (!std::filesystem::is_regular_file(path)) {
            m_status = ImportStatus::FileNotFound;
            m_errors.push_back("Not a regular file: " + path.string());
            return;
        }

        tinyxml2::XMLError result = m_doc.LoadFile(path.string().c_str());
        if (result != tinyxml2::XML_SUCCESS) {
            m_status = ImportStatus::XmlParseError;
            m_errors.push_back("TinyXML2 error (" + std::to_string(result) +
                               "): " + (m_doc.ErrorStr() ? m_doc.ErrorStr() : "Unknown error"));
            return;
        }

        // TODO: Validate version etc.
        //  <?AutomationStudio Version=6.0.2.177 FileVersion="4.9"?>

        m_hw = m_doc.FirstChildElement("Hardware");
        if (!m_hw) {
            m_status = ImportStatus::NoHardwareElement;
            m_errors.push_back("No <Hardware> element found in file: " + path.string());
            return;
        }
    }

    ModuleXmlImporter::~ModuleXmlImporter() {}
    std::unordered_map<std::string, Module> ModuleXmlImporter::getModules() {
        assert(m_status == ImportStatus::ModulesMapped && "Call MapModules() First!");

        return m_modules;
    }

    void ModuleXmlImporter::mapModules() {
        std::unordered_map<std::string, Module> modules;

        for (auto moduleElem = m_hw->FirstChildElement("Module"); moduleElem;
                moduleElem = moduleElem->NextSiblingElement("Module")) {
            // Finds Module and its attributes
            // <Module Name="X20PS9600" Type="X20PS9600" Version="1.6.2.0">
            Module m;
            const tinyxml2::XMLAttribute* attrModule = moduleElem->ToElement()->FirstAttribute();
            int attrCountModules = 0;
            while (attrModule) {
                std::string name = attrModule->Name();
                std::string value = attrModule->Value();
                if (name == "Name") {
                    m.name = value;
                } else if (name == "Type") {
                    if (value.find('-') != std::string::npos) {
                        std::replace(value.begin(), value.end(), '-', '_');
                    }
                    m.type = magic_enum::enum_cast<cardType>(value).value_or(cardType::ERROR);
                } else if (name == "Version") {
                    m.version = value;
                } else {
                    std::cerr << "Unexpected attribute in <Module>: " << name << "=\"" << value
                              << "\"\n";
                }
                attrModule = attrModule->Next();
                attrCountModules++;
            }
            // only Name, type, Version.
            // <Module Name="X20PS9600" Type="X20PS9600" Version="1.6.2.0">
            if (attrCountModules != 3) {
                std::cerr << "Warning: Module should have exactly 3 attributes, found "
                          << attrCountModules << std::endl;
            }

            // Handling for module's children for example in this case the connections are children
            //      <Module Name="X20PS9600" Type="X20PS9600" Version="1.6.2.0">
            //          <Connection Connector="PS" TargetModule="X20BB52" TargetConnector="PS1" />
            //          <Connection Connector="SS1" TargetModule="X20TB12" TargetConnector="SS" />
            //      </Module>
            for (auto childElem = moduleElem->FirstChildElement(); childElem;
                    childElem = childElem->NextSiblingElement()) {
                std::string childName = childElem->Name();
                if (childName == "Connection") {
                    Connection con;
                    const char* connectorAttr = childElem->Attribute("Connector");
                    if (connectorAttr)
                        con.connector = magic_enum::enum_cast<ConnectorType>(connectorAttr)
                                                .value_or(ConnectorType::ERROR);

                    const char* targetModuleAttr = childElem->Attribute("TargetModule");
                    if (targetModuleAttr) {
                        con.targetModuleName = targetModuleAttr;
                        con.targetModule = nullptr;
                    }

                    const char* targetConnectorAttr = childElem->Attribute("TargetConnector");
                    if (targetConnectorAttr)
                        con.targetConnector =
                                magic_enum::enum_cast<ConnectorType>(targetConnectorAttr)
                                        .value_or(ConnectorType::ERROR);

                    m.connections.push_back(con);
                } else if (childName == "Parameter") {
                    Parameter param;
                    const char* targetIdAttr = childElem->Attribute("ID");
                    if (targetIdAttr)
                        param.id = targetIdAttr;

                    const char* targetValueAttr = childElem->Attribute("Value");
                    if (targetValueAttr)
                        param.value = targetValueAttr;
                    m.parameters.push_back(param);

                } else if (childName == "Group") {
                    Group group;
                    const char* targetIdAttr = childElem->Attribute("ID");
                    if (targetIdAttr)
                        group.id = targetIdAttr;
                    m.group = group;
                    // Handle special children of modules For example in BB52 and CPU
                    //  <Module Name="X20CP0484_1" Type="X20CP0484-1" Version="1.8.1.0">
                    //      <Connection Connector="SL" TargetModule="X20BB52" TargetConnector="SL1"
                    //      />
                    //          <Connector Name="IF2">
                    //              <Parameter ID="ActivateDevice" Value="1" />
                    //          </Connector>
                    //      <Parameter ID="ConfigurationID" Value="Mir_Mir_config1" />
                } else if (childName == "Connector") {
                    Connector connector;
                    const char* connectorNameAttr = childElem->Attribute("Name");
                    if (connectorNameAttr)
                        connector.name = connectorNameAttr;

                    // Parse all <Parameter> children of <Connector>
                    for (auto paramElem = childElem->FirstChildElement("Parameter"); paramElem;
                            paramElem = paramElem->NextSiblingElement("Parameter")) {
                        Parameter param;
                        const char* paramId = paramElem->Attribute("ID");
                        if (paramId)
                            param.id = paramId;
                        const char* paramValue = paramElem->Attribute("Value");
                        if (paramValue)
                            param.value = paramValue;
                        connector.parameters.push_back(param);
                    }
                    m.connector = connector;
                } else {
                    std::cerr << "Unexpected child element in "
                              << moduleElem->FirstAttribute()->Value() << ": <" << childName
                              << ">\n";
                }
            }
            modules[m.name] = m;
        }

        m_modules = modules;
        m_status = ImportStatus::ModulesMapped;
    }
    void ModuleXmlImporter::printErrors() {
        for (auto&& s : m_errors) {
            std::cout << s << "\n";
        }
    }

}  // namespace HwTool