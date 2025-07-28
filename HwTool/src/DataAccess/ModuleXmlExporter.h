#pragma once
#include <tinyxml2.h>
#include <unordered_map>
#include <string>
#include "Types.h"

namespace HwTool{
    class ModuleXmlExporter {
    private:
        /* data */
    public:
        ModuleXmlExporter(/* args */);
        ~ModuleXmlExporter();
        void serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const Connection& conn);
        void serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const Parameter& param);
        void serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const Group& group);
        void serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const Connector& connector);
        void serialize(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, const Module& mod);
        void serialize(const std::unordered_map<std::string, Module>& modules, const std::string& filename);
    };

}
