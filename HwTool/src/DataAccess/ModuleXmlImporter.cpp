#include "ModuleXmlImporter.h"

#include <tinyxml2.h>

#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <variant>
#include <print>
#include "HardwareBuilder.h"
using namespace tinyxml2;
namespace HwTool {
    namespace V2 {


        ModuleXmlImporter::ModuleXmlImporter(const std::filesystem::path& path){
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
                std::cout << "TinyXML2 error (" << result << "): "
                        << (m_doc.ErrorStr() ? m_doc.ErrorStr() : "Unknown error") << std::endl;
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
        ModuleXmlImporter::~ModuleXmlImporter(){}

        //TODO: split into smaller pieces? maybe?
        ModuleMap ModuleXmlImporter::mapModules(){
            HardwareBuilder hwb;
            ModuleVariant parsedModule;
            std::vector<std::shared_ptr<HwTool::V2::ModuleIO>> parsedIO; // maybe delete

            ModuleMap res;
            std::unordered_map<std::string, std::string> baseLinkMap;
            

            for (auto moduleElem = m_hw->FirstChildElement("Module"); moduleElem;){
                auto* nextElem = moduleElem->NextSiblingElement("Module");
                auto [name, type, version] = extractModuleNameTypeVersion(moduleElem->ToElement());
                auto parsed = parseModuleType(type);
                switch (parsed.kind) {
                    case ParsedType::Kind::IO:{
                        std::string tb;
                        std::string bm;
                        printf("IO");
                        // Use std::get<IoCardType>(parsed.value)
                        for (auto childElem = moduleElem->FirstChildElement(); childElem;
                                childElem = childElem->NextSiblingElement()) {
                            std::string childName = childElem->Name();
                            if (childName == "Connection") {
                                //TODO: Ignoring connectors -> test if causes problems anywhere
                                const char* connectorAttr = childElem->Attribute("Connector");
                                const char* targetModuleAttr = childElem->Attribute("TargetModule");
                                
                                    if (targetModuleAttr && connectorAttr && std::string(connectorAttr) == "SS1") {
                                        tb = targetModuleAttr;
                                    } else if (targetModuleAttr && connectorAttr && std::string(connectorAttr) == "SL") {
                                        bm = targetModuleAttr;
                                    }else{
                                    assert(false && "shouldnt probably happen :)");
                                }
                            }
                        }
                        parsedIO.push_back(hwb.createModuleIoCard(name, std::get<IoCardType>(parsed.value), "1.1.0.0", tb, bm));
                        //parsedModule = hwb.createModuleIoCard(name, std::get<IoCardType>(parsed.value), "1.1.0.0", tb, bm);
                        //res[name] = parsedModule;
                        m_hw->DeleteChild(moduleElem);
                        break;}
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
                                //TODO: Ignoring connectors -> test if causes problems anywhere
                                const char* connectorAttr = childElem->Attribute("Connector");
                                const char* targetModuleAttr = childElem->Attribute("TargetModule");
                                
                                    if (targetModuleAttr && connectorAttr && std::string(connectorAttr) == "X2X1") {
                                        baseLinkMap[name] = targetModuleAttr;
                                    } else{
                                    //TODO: in this it will happen on anything other than BM11
                                    assert(false && "shouldnt probably happen :)");
                                }
                            }
                        }
                        
                        break;
                    case ParsedType::Kind::CPU:
                        printf("Cpu");
                    default:
                        printf("[N]");
                        break;
                }
                moduleElem = nextElem;
            }

            //TODO: think of better linking this is bad.
            // not probably for performance but interesintg problem
            // Maybe during initial io parsing create additional cache of base 
            // map that has base -> iocard. 
            // and use that in this step to have o(1) lookup to skip the second for loop?

            //TODO: implement this for the moduleVariant since it is currently only for IO
            // not finding BB52 for example since its cpu
            for (const auto& m : parsedIO) {
                std::string targetBase = baseLinkMap[m->base];
                for (auto &&m2 : parsedIO)
                {
                    if (m2->base == targetBase)
                    {
                        m->next = m2;
                        m2->previous =m;
                        res[m->name] = m;
                        break;
                    }
                    
                }
                
            }
            std::cout << "hehehe" << "\n";
            return res;
        };

        //TODO: add error handling
        std::tuple<std::string, std::string, std::string> extractModuleNameTypeVersion(
            const tinyxml2::XMLElement* moduleElem) {
            std::string name, type, version;
            for (const tinyxml2::XMLAttribute* attr = moduleElem->FirstAttribute(); attr; attr = attr->Next()) {
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