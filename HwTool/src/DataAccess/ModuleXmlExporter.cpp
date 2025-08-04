
#include "ModuleXmlExporter.h"
namespace HwTool{

    ModuleXmlExporter::ModuleXmlExporter() {}
    
    ModuleXmlExporter::~ModuleXmlExporter() {}
    
    void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* moduleElem,
                                      const Connection& conn) {
        auto* connElem = doc.NewElement("Connection");
        connElem->SetAttribute("Connector", magic_enum::enum_name(conn.connector).data());
        connElem->SetAttribute("TargetModule", conn.targetModuleName.c_str());
        connElem->SetAttribute("TargetConnector", magic_enum::enum_name(conn.targetConnector).data());
        moduleElem->InsertEndChild(connElem);
    }
    
    void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent,
                                      const Module& mod) {
        auto* modElem = doc.NewElement("Module");
        modElem->SetAttribute("Name", mod.name.c_str());
        // just for special cards for example X20CP0484-1 -> 
        // enum cannot be ..84-1 so it is 84_1
        // when serializing need to convert "_" back to "-"
        std::string typeName = std::string(magic_enum::enum_name(mod.type));
        std::replace(typeName.begin(), typeName.end(), '_', '-');
        modElem->SetAttribute("Type", typeName.c_str());
        modElem->SetAttribute("Version", mod.version.c_str());
    
        if (!mod.connections.empty()) {
            for (const auto& conn : mod.connections) {
                serialize(doc, modElem, conn);
            }
        }
    
        if (!mod.parameters.empty()) {
            for (const auto& param : mod.parameters) {
                serialize(doc, modElem, param);
            }
        }
    
        if (!mod.connector.name.empty() || !mod.connector.parameters.empty()) {
            serialize(doc, modElem, mod.connector);
        }
    
        if (!mod.group.id.empty()) {
            serialize(doc, modElem, mod.group);
        }
    
        parent->InsertEndChild(modElem);
    }
    void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const Parameter& param) {
        auto* paramElem = doc.NewElement("Parameter");
        paramElem->SetAttribute("ID", param.id.c_str());
        paramElem->SetAttribute("Value", param.value.c_str());
        parent->InsertEndChild(paramElem);
    }
    
    void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const Group& group) {
        auto* groupElem = doc.NewElement("Group");
        groupElem->SetAttribute("ID", group.id.c_str());
        parent->InsertEndChild(groupElem);
    }
    
    void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const Connector& connector) {
        auto* connElem = doc.NewElement("Connector");
        connElem->SetAttribute("Name", connector.name.c_str());
        for (const auto& param : connector.parameters) {
            serialize(doc, connElem, param);
        }
        parent->InsertEndChild(connElem);
    }
    
    
    void ModuleXmlExporter::serialize(const ModuleMap& modules,
                                      const std::string& filename) {
        tinyxml2::XMLDocument doc;
    
        auto* decl = doc.NewDeclaration(R"(xml version="1.0" encoding="utf-8")");
        doc.InsertFirstChild(decl);
        auto* asInstr = doc.NewDeclaration(R"(AutomationStudio Version=6.0.2.177 FileVersion="4.9")");
        doc.InsertEndChild(asInstr);
    
        auto* root = doc.NewElement("Hardware");
        root->SetAttribute("xmlns", "http://br-automation.co.at/AS/Hardware");
        doc.InsertEndChild(root);
    
        for (const auto& [name, mod] : modules) {
            serialize(doc, root, mod);
        }
    
        doc.SaveFile(filename.c_str());
    }
}