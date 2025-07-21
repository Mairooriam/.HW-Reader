#include "ModuleXmlImporter.h"

#include <tinyxml2.h>

#include <iostream>
#include <magic_enum/magic_enum.hpp>

using namespace tinyxml2;
namespace HwTool {
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
    }
    void ModuleXmlImporter::printErrors() {
        for (auto&& s : m_errors) {
            std::cout << s << "\n";
        }
    }




}  // namespace HwTool