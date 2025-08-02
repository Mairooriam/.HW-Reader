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

        // TODO: split into smaller pieces? maybe?
        const ModuleMap& ModuleXmlImporter::mapModules() {
            HardwareBuilder hwb;
            ModuleVariant parsedModule;
            std::vector<std::shared_ptr<HwTool::V2::ModuleIO>> parsedIO;  // maybe delete
            std::unordered_map<std::string, std::string> cachedBaseToModule;
            std::unordered_map<std::string, std::string> baseLinkMap;

            for (auto moduleElem = m_hw->FirstChildElement("Module"); moduleElem;) {
                auto* nextElem = moduleElem->NextSiblingElement("Module");
                auto [name, type, version] = extractModuleNameTypeVersion(moduleElem->ToElement());
                auto parsed = parseModuleType(type);
                switch (parsed.kind) {
                    case ParsedType::Kind::IO: {
                        std::string tb;
                        std::string bm;
                        printf("IO");
                        // Use std::get<IoCardType>(parsed.value)
                        for (auto childElem = moduleElem->FirstChildElement(); childElem;
                                childElem = childElem->NextSiblingElement()) {
                            std::string childName = childElem->Name();
                            if (childName == "Connection") {
                                // TODO: Ignoring connectors -> test if causes problems anywhere
                                const char* connectorAttr = childElem->Attribute("Connector");
                                const char* targetModuleAttr = childElem->Attribute("TargetModule");

                                if (targetModuleAttr && connectorAttr &&
                                        std::string(connectorAttr) == "SS1") {
                                    tb = targetModuleAttr;
                                } else if (targetModuleAttr && connectorAttr &&
                                           std::string(connectorAttr) == "SL") {
                                    bm = targetModuleAttr;
                                } else {
                                    assert(false && "shouldnt probably happen :)");
                                }
                            }
                        }
                        // parsedIO.push_back(hwb.createModuleIoCard(name,
                        // std::get<IoCardType>(parsed.value), "1.1.0.0", tb, bm));
                        m_modules[name] = hwb.createModuleIoCard(
                                name, std::get<IoCardType>(parsed.value), "1.1.0.0", tb, bm);
                        cachedBaseToModule[bm] = name;
                        m_hw->DeleteChild(moduleElem);
                        break;
                    }
                    case ParsedType::Kind::Bus:
                        printf("bus");
                        // Use std::get<BusModuleType>(parsed.value)
                        break;
                    case ParsedType::Kind::Base:
                        printf("base");
                        for (auto childElem = moduleElem->FirstChildElement(); childElem;
                                childElem = childElem->NextSiblingElement()) {
                            std::string childName = childElem->Name();
                            if (childName == "Connection") {
                                // TODO: Ignoring connectors -> test if causes problems anywhere
                                const char* connectorAttr = childElem->Attribute("Connector");
                                const char* targetModuleAttr = childElem->Attribute("TargetModule");

                                if (targetModuleAttr && connectorAttr &&
                                        std::string(connectorAttr) == "X2X1") {
                                    baseLinkMap[targetModuleAttr] = name;
                                } else {
                                    // TODO: in this it will happen on anything other than BM11
                                    assert(false && "shouldnt probably happen :)");
                                }
                            }
                        }

                        break;
                    case ParsedType::Kind::CPU: {
                        printf("Cpu\n");
                        std::string baseName;
                        V2::Connector connector;
                        std::unordered_map<std::string, std::string> parameters;
                        std::string group;

                        for (auto childElem = moduleElem->FirstChildElement(); childElem;
                                childElem = childElem->NextSiblingElement()) {
                            std::string childName = childElem->Name();
                            if (childName == "Connection") {
                                const char* connectorAttr = childElem->Attribute("Connector");
                                const char* targetModuleAttr = childElem->Attribute("TargetModule");
                                if (targetModuleAttr && connectorAttr &&
                                        std::string(connectorAttr) == "SL") {
                                    baseName = targetModuleAttr;
                                }
                            } else if (childName == "Connector") {
                                const char* connName = childElem->Attribute("Name");
                                if (connName)
                                    connector.name = connName;
                                for (auto paramElem = childElem->FirstChildElement("Parameter");
                                        paramElem;
                                        paramElem = paramElem->NextSiblingElement("Parameter")) {
                                    const char* id = paramElem->Attribute("ID");
                                    const char* val = paramElem->Attribute("Value");
                                    if (id && val)
                                        connector.parameters[id] = val;
                                }
                            } else if (childName == "Parameter") {
                                const char* id = childElem->Attribute("ID");
                                const char* val = childElem->Attribute("Value");
                                if (id && val)
                                    parameters[id] = val;
                            } else if (childName == "Group") {
                                const char* id = childElem->Attribute("ID");
                                if (id)
                                    group = id;
                            }
                        }

                        auto cpu = hwb.createModuleCpu(
                                name, std::get<CpuType>(parsed.value), baseName, version);
                        cpu.connector = connector;
                        cpu.group = group;
                        cpu.parameters = parameters;
                        m_modules[name] = cpu;
                        cachedBaseToModule[baseName] = name;
                        break;
                    }
                    default: printf("[N]"); break;
                }
                moduleElem = nextElem;
            }

            for (const auto& [name, moduleVariant] : m_modules) {
                if (std::holds_alternative<V2::ModuleIO>(moduleVariant)) {
                    auto& m = std::get<V2::ModuleIO>(m_modules.at(name));
                    std::string targetBase = baseLinkMap[m.base];
                    auto nextIt = cachedBaseToModule.find(targetBase);
                    if (nextIt != cachedBaseToModule.end()) {
                        const std::string& nextName = nextIt->second;
                        auto resIt = m_modules.find(nextName);
                        if (resIt != m_modules.end() &&
                                std::holds_alternative<V2::ModuleIO>(
                                        resIt->second)) {
                            auto& nextIO = std::get<V2::ModuleIO>(resIt->second);
                            m.next = &(resIt->second);
                            nextIO.previous = &(m_modules.at(name));
                        }
                    }
                    std::cout << "IO: " << m.name << "\n";
                } else if (std::holds_alternative<V2::ModuleBUS>(moduleVariant)) {
                    auto m = std::get<V2::ModuleBUS>(moduleVariant);
                    // Handle BUS module - convert to pointer to variant
                    std::cout << "BUS: " << m.name << "\n";
                } else if (std::holds_alternative<V2::ModuleCPU>(moduleVariant)) {
                    auto& m = std::get<V2::ModuleCPU>(m_modules.at(name));
                    std::string targetBase = baseLinkMap[m.base];
                    auto nextIt = cachedBaseToModule.find(targetBase);
                    if (nextIt != cachedBaseToModule.end()) {
                        const std::string& nextName = nextIt->second;
                        auto resIt = m_modules.find(nextName);
                        if (resIt != m_modules.end()) {
                            // Cast to ModuleVariant* by taking address of the variant in the map
                            m.next = &(resIt->second); 
                            
                            // For the reverse link, you need to handle different types
                            std::visit([&](auto& targetModule) {
                                using T = std::decay_t<decltype(targetModule)>;
                                if constexpr (std::is_same_v<T, V2::ModuleIO>) {
                                    targetModule.previous = &(m_modules.at(name));
                                } else if constexpr (std::is_same_v<T, V2::ModuleBUS>) {
                                    targetModule.previous = &(m_modules.at(name));
                                }
                            }, resIt->second);
                        }
                    }
                    std::cout << "CPU: " << m.name << "\n";
                } else {
                    std::cout << "Unknown module type for: " << name << "\n";
                }
            }

            std::cout << "hehehe" << "\n";
            return m_modules;
        };

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