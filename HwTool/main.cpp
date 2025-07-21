#include <iostream>
#include <unordered_set>
#include <vector>

#include "tinyxml2.h"

using namespace tinyxml2;
#include <fstream>
#include <memory>
#include <set>
#include <sstream>

#include "hardware/ConnectionAnalyzer.h"
#include "hardware/HardwareBuilder.h"
#include "hardware/ModuleXmlExporter.h"
#include "hardware/hwUtils.h"
#include "types.h"

void printElement(const tinyxml2::XMLElement* element, int indent = 0) {
    if (!element)
        return;
    // Print indentation
    for (int i = 0; i < indent; ++i)
        std::cout << "  ";
    // Print element name
    std::cout << "<" << element->Name();
    // Print all attributes
    const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
    while (attr) {
        std::cout << " " << attr->Name() << "=\"" << attr->Value() << "\"";
        attr = attr->Next();
    }
    std::cout << ">" << std::endl;

    // Print children
    for (const tinyxml2::XMLElement* child = element->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        printElement(child, indent + 1);
    }
}

void resolveLinking(std::unordered_map<std::string, Module>& modules) {
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

std::unordered_map<std::string, Module> mapModules(tinyxml2::XMLElement* hw) {
    std::unordered_map<std::string, Module> modules;
    for (auto moduleElem = hw->FirstChildElement("Module"); moduleElem;
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
                    con.targetConnector = magic_enum::enum_cast<ConnectorType>(targetConnectorAttr)
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
            //      <Connection Connector="SL" TargetModule="X20BB52" TargetConnector="SL1" />
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
                std::cerr << "Unexpected child element in " << moduleElem->FirstAttribute()->Value()
                          << ": <" << childName << ">\n";
            }
        }
        modules[m.name] = m;
    }

    return modules;
}

std::string toMermaid(const std::unordered_map<std::string, Module>& modules) {
    std::ostringstream oss;
    oss << "```mermaid\n";
    oss << "graph TD\n";

    // Subgraph for all X20BM11 modules
    oss << "    subgraph BM11s [\"Base Modules\"]\n";
    for (const auto& [name, module] : modules) {
        if (name.find("X20BM11") == 0 || name.find("X20BB52") == 0) {
            oss << "        " << name << "[\"" << name << "<br/>"
                << magic_enum::enum_name(module.type) << "\"]\n";
        }
    }
    oss << "    end\n";

    // Other modules as nodes
    for (const auto& [name, module] : modules) {
        if (name.find("X20BM11") != 0 || name.find("X20BB52") != 0) {
            oss << "    " << name << "[\"" << name << "<br/>" << magic_enum::enum_name(module.type)
                << "\"]\n";
        }
    }

    // Connections as edges
    for (const auto& [name, module] : modules) {
        for (const auto& con : module.connections) {
            if (con.targetModule) {
                oss << "    " << name << " -- " << magic_enum::enum_name(con.connector) << " --> "
                    << con.targetModule->name << "\n";
            }
        }
    }
    oss << "```";
    return oss.str();
}

int main(int argc, char const* argv[]) {
    XMLDocument doc;
    if (doc.LoadFile("Hardware.hw") != XML_SUCCESS) {
        std::cerr << "Failed to load file\n";
        return 1;
    }

    XMLPrinter printer;
    doc.Print(&printer);
    std::cout << printer.CStr();

    auto hw = doc.FirstChildElement("Hardware");
    if (!hw) {
        std::cerr << "No <Hardware> element found\n";
        return 1;
    }

    auto hardwareName = hw->Value();
    auto attributename = hw->FirstAttribute()->Name();
    auto hwattribute = hw->FirstAttribute()->Value();
    printf("hardware name: %s \n", hardwareName);
    printf("HW first attribute = %s : %s\n", attributename, hwattribute);

    auto c1 = hw->FirstChild();
    // printElement(c1->ToElement());

    printf("c1 value : %s \n", c1->Value());
    auto modules = mapModules(hw);
    resolveLinking(modules);

    std::set<std::string> bases;
    std::vector<std::string> tbs;

    for (auto&& m : modules) {
        if (m.second.type == cardType::X20BM11) {
            bases.insert(m.first);
        }
    }
    // std::string str = "X20BM11";
    // for (size_t i = 0; i < 100; i++)
    // {
    //     std::string newStr = incrementStr(str,bases);
    //     bases.insert(newStr);
    // }

    HardwareBuilder hwb;
    std::string previousBase = "X20BM11d";
    std::set<std::string> afs;
    for (size_t i = 0; i < 3; i++) {
        std::string nextX20BM11 = incrementStr("X20BM11", bases);

        Module newBM11 = hwb.X20BM11(nextX20BM11, "1.1.0.0", nullptr, previousBase);
        Module newTB12 = hwb.X20TB12(incrementStr("X20TB12", modules));
        Module newIO = hwb.IOCARD2(incrementStr("AF", afs), cardType::X20AI4622, newTB12.name, newBM11.name);
        afs.insert(newIO.name);
        modules[newBM11.name] = newBM11;
        modules[newTB12.name] = newTB12;
        modules[newIO.name] = newIO;
        bases.insert(nextX20BM11);
        previousBase = nextX20BM11;
    }

    // auto test = hwb.X20BM11("Testing");
    //  auto test = hwb.IOCARD("test_card", cardType::X20AI4622, "X20BM11d");
    //  for (auto &&c : test)
    //  {
    //      modules[c.name] = c;
    //  }

    resolveLinking(modules);

    std::string mermaid = toMermaid(modules);
    // std::cout << "\nMermaid diagram:\n" << mermaid << std::endl;

    ModuleXmlExporter exporter;
    exporter.serialize(modules, "test.xml");

    std::ofstream out("diagram.md");
    if (out) {
        out << mermaid;
        out.close();
        std::cout << "Mermaid diagram saved to diagram.mmd\n";
    } else {
        std::cerr << "Failed to write diagram.mmd\n";
    }

    return 0;
}
