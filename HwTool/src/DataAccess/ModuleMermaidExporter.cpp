#include "ModuleMermaidExporter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "Types.h"

namespace HwTool {

    void ModuleMermaidExporter::serialize(const ModuleMap& modules) {
        std::ostringstream oss;
        oss << "```mermaid\n";
        oss << "graph TD\n";

        // Subgraph for all X20BM11 modules and BB52
        // TODO: make more dynamic to maybe config? which has all "base" models?
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
                oss << "    " << name << "[\"" << name << "<br/>"
                    << magic_enum::enum_name(module.type) << "\"]\n";
            }
        }

        // Connections as edges
        for (const auto& [name, module] : modules) {
            for (const auto& con : module.connections) {
                if (con.targetModule) {
                    oss << "    " << name << " -- " << magic_enum::enum_name(con.connector)
                        << " --> " << con.targetModule->name << "\n";
                }
            }
        }
        oss << "```";
        std::ofstream out(m_path);
        if (out) {
            out << oss.str();
            out.close();
        } else {
            std::cerr << "Failed to open file for writing: " << m_path << std::endl;
        }
    }

    ModuleMermaidExporter::ModuleMermaidExporter(const std::filesystem::path& path) : m_path(path) {
        // TODO: Some checking on the path. maybe erorr messages etc.
    }

    ModuleMermaidExporter::~ModuleMermaidExporter() {}

}  // namespace HwTool
