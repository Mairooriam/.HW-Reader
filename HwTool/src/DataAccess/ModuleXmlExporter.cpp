
#include "ModuleXmlExporter.h"
#include "iostream"
namespace HwTool {
    ModuleXmlExporter::ModuleXmlExporter() {}
    namespace V2 {
        ModuleXmlExporter::ModuleXmlExporter(){}
        ModuleXmlExporter::~ModuleXmlExporter(){}

        void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent,
                                              const V2::ModuleIO& mod) {
            //TODO: make more dynamic? or just keep explicit like this idk.
            //Handle IO Card
            auto* modElem = doc.NewElement("Module");
            modElem->SetAttribute("Name", mod.name.c_str());
            modElem->SetAttribute("Type", magic_enum::enum_name(mod.type).data());
            modElem->SetAttribute("Version", mod.version.c_str());

            auto* child = doc.NewElement("Connection");
            child->SetAttribute("Connector", "SS1");
            child->SetAttribute("TargetModule", mod.tb.c_str());
            child->SetAttribute("TargetConnector" , "SS");
            modElem->InsertEndChild(child);

            child = doc.NewElement("Connection");
            child->SetAttribute("Connector", "SL");
            child->SetAttribute("TargetModule", mod.base.c_str());
            child->SetAttribute("TargetConnector" , "SL1");
            modElem->InsertEndChild(child);

            // Handle Base
            auto* baseElem = doc.NewElement("Module");
            baseElem->SetAttribute("Name", mod.base.c_str());
            baseElem->SetAttribute("Type", "X20BM11");
            baseElem->SetAttribute("Version", "1.1.0.0");

            child = doc.NewElement("Connection");
            child->SetAttribute("Connector", "X2X1");
            child->SetAttribute("TargetModule", mod.nextModuleName.c_str());

            child->SetAttribute("TargetConnector", "X2X2");
            baseElem->InsertEndChild(child);
            parent->InsertEndChild(baseElem);
            parent->InsertEndChild(modElem);
        }
        void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const V2::Connector& mod){
            auto* connectorElem = doc.NewElement("Connector");
            connectorElem->SetAttribute("Name", mod.name.c_str());

            // Add <Parameter ID="..." Value="..."/> for each parameter
            for (const auto& [id, value] : mod.parameters) {
                auto* paramElem = doc.NewElement("Parameter");
                paramElem->SetAttribute("ID", id.c_str());
                paramElem->SetAttribute("Value", value.c_str());
                connectorElem->InsertEndChild(paramElem);
            }

            // Attach to parent
            parent->InsertEndChild(connectorElem);
        }
        void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent,
                                          const V2::ModuleBUS& mod) {

            // Handle bus module
            auto* modElem = doc.NewElement("Module");
            modElem->SetAttribute("Name", mod.name.c_str());
            modElem->SetAttribute("Type", magic_enum::enum_name(mod.type).data());
            modElem->SetAttribute("Version", mod.version.c_str());
            modElem->SetAttribute("NodeNumber", mod.nodeNumber);

            // Connection 1
            auto* child = doc.NewElement("Connection");
            child->SetAttribute("Connector", "PLK1");
            child->SetAttribute("TargetModule", "NOT_IMPLEMENTED");//TODO: Ad handling for previous Remember module card always has previous. if it doesnt its not valid.
            child->SetAttribute("TargetConnector" , "IF3");

            // cable
            //TODO: Are all modules connected with powerlinkcable?
            // possibly need handling for different connections.
            //TODO: what specifies the lenght? default seems like 10
            //TODO: what specifies cable version?
            auto* cable = doc.NewElement("Cable");
            cable->SetAttribute("Type", "PowerLinkCable");
            cable->SetAttribute("Lenght", "10");
            cable->SetAttribute("Version", "1.0.0.3");
            child->InsertEndChild(cable);

            // Connection 2
            auto* con2 = doc.NewElement("Connection");
            con2->SetAttribute("Connector", "SL");
            con2->SetAttribute("TargetModule", mod.base.c_str());
            con2->SetAttribute("TargetConnector" , "SL1");


            modElem->InsertEndChild(child);
            modElem->InsertEndChild(con2);

            parent->InsertEndChild(modElem);
        }

        void ModuleXmlExporter::serialize(
                tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const V2::ModuleCPU& mod) {
                    // initial module
                    auto* modElem = doc.NewElement("Module");
                    modElem->SetAttribute("Name", mod.name.c_str());
                    modElem->SetAttribute("Type", magic_enum::enum_name(mod.type).data());
                    modElem->SetAttribute("Version", mod.version.c_str());

                    // Connection 
                    auto* child = doc.NewElement("Connection");
                    child->SetAttribute("Connector", "SL1");
                    child->SetAttribute("TargetModule", mod.base.c_str());
                    child->SetAttribute("TargetConnector" , "SL1");
                    modElem->InsertEndChild(child);
                    serialize(doc, modElem, mod.connector);

                    // Parameters
                    for (const auto& [id, value] : mod.parameters) {
                        auto* paramElem = doc.NewElement("Parameter");
                        paramElem->SetAttribute("ID", id.c_str());
                        paramElem->SetAttribute("Value", value.c_str());
                        modElem->InsertEndChild(paramElem);
                    }
                    // Group 
                    if (!mod.group.empty()) {
                        auto* groupElem = doc.NewElement("Group");
                        groupElem->SetAttribute("ID", mod.group.c_str());
                        modElem->InsertEndChild(groupElem);
                    }

                    parent->InsertEndChild(modElem);
                }

        void ModuleXmlExporter::serialize(const V2::ModuleMap& modules,
                                              const std::string& filename) {
            tinyxml2::XMLDocument doc;

            auto* decl = doc.NewDeclaration(R"(xml version="1.0" encoding="utf-8")");
            doc.InsertFirstChild(decl);
            auto* asInstr =
                doc.NewDeclaration(R"(AutomationStudio Version=6.0.2.177 FileVersion="4.9")");
            doc.InsertEndChild(asInstr);

            auto* root = doc.NewElement("Hardware");
            root->SetAttribute("xmlns", "http://br-automation.co.at/AS/Hardware");
            doc.InsertEndChild(root);

            //TODO: mayeb upate to serialize overloads when you have types
            // for now just this so i dont need to impliment all
            for (const auto& [name, mod] : modules) {
                
                std::visit([&](auto&& arg){
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, V2::ModuleIO>) {
                    serialize(doc, root, arg); 
                }else if constexpr (std::is_same_v<T, V2::ModuleBUS>)
                {
                    serialize(doc, root, arg);
                }else if constexpr (std::is_same_v<T, V2::ModuleCPU>){
                    serialize(doc, root, arg);
                }
                 
                else {
                    assert(false && "Not handling all types yet.");
                }
            }, mod);
            }

            doc.SaveFile(filename.c_str());
        }

    }  // namespace V2

    ModuleXmlExporter::~ModuleXmlExporter() {}

    void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* moduleElem,
                                      const Connection& conn) {
        auto* connElem = doc.NewElement("Connection");
        connElem->SetAttribute("Connector", magic_enum::enum_name(conn.connector).data());
        connElem->SetAttribute("TargetModule", conn.targetModuleName.c_str());
        connElem->SetAttribute("TargetConnector",
                               magic_enum::enum_name(conn.targetConnector).data());
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
    void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent,
                                      const Parameter& param) {
        auto* paramElem = doc.NewElement("Parameter");
        paramElem->SetAttribute("ID", param.id.c_str());
        paramElem->SetAttribute("Value", param.value.c_str());
        parent->InsertEndChild(paramElem);
    }

    void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent,
                                      const Group& group) {
        auto* groupElem = doc.NewElement("Group");
        groupElem->SetAttribute("ID", group.id.c_str());
        parent->InsertEndChild(groupElem);
    }

    void ModuleXmlExporter::serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent,
                                      const Connector& connector) {
        auto* connElem = doc.NewElement("Connector");
        connElem->SetAttribute("Name", connector.name.c_str());
        for (const auto& param : connector.parameters) {
            serialize(doc, connElem, param);
        }
        parent->InsertEndChild(connElem);
    }

    void ModuleXmlExporter::serialize(const std::unordered_map<std::string, Module>& modules,
                                      const std::string& filename) {
        tinyxml2::XMLDocument doc;

        auto* decl = doc.NewDeclaration(R"(xml version="1.0" encoding="utf-8")");
        doc.InsertFirstChild(decl);
        auto* asInstr =
            doc.NewDeclaration(R"(AutomationStudio Version=6.0.2.177 FileVersion="4.9")");
        doc.InsertEndChild(asInstr);

        auto* root = doc.NewElement("Hardware");
        root->SetAttribute("xmlns", "http://br-automation.co.at/AS/Hardware");
        doc.InsertEndChild(root);

        for (const auto& [name, mod] : modules) {
            serialize(doc, root, mod);
        }

        doc.SaveFile(filename.c_str());
    }

}  // namespace HwTool