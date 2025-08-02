#include "ModuleMermaidExporter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include "Types.h"

namespace HwTool {

    void ModuleMermaidExporter::serialize(const std::unordered_map<std::string, Module>& modules) {
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

    void ModuleMermaidExporter::serialize(const V2::ModuleMap& m) {
        std::ostringstream oss;
        oss << "```mermaid\n";
        oss << "graph TD\n";
        
        // Find the CPU (root)
        const V2::ModuleCPU* cpu = nullptr;
        std::string cpuName;
        for (const auto& [name, variant] : m) {
            if (std::holds_alternative<V2::ModuleCPU>(variant)) {
                cpu = &std::get<V2::ModuleCPU>(variant);
                cpuName = name;
                break;
            }
        }
        
        if (cpu) {
            traverseAndOutput(oss, m.at(cpuName));  // Changed from 'variant' to 'm.at(cpuName)'
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

    void ModuleMermaidExporter::traverseAndOutput(std::ostringstream& oss, const V2::ModuleVariant& current) {
        std::visit([&](const auto& module) {
            oss << "\tsubgraph " << module.name << "sub" << "[" << module.name << "]\n";
            oss << "\t" << module.name << "\n";
            
            if constexpr (std::is_same_v<std::decay_t<decltype(module)>, V2::ModuleIO>) {
                oss << "\t" << module.tb << "\n";
            }
            oss << "\t" << module.base << "\n";
            oss << "end\n";
            
            if (module.next) {
                // Get the next module from the variant pointer
                std::visit([&](const auto& nextModule) {  // Removed extra nested visit
                    oss << module.name << "sub --> " << nextModule.name << "sub\n";
                    
                    // Recursively process the next module directly
                    traverseAndOutput(oss, *module.next);  // Changed from allModules lookup to direct recursion
                }, *module.next);
            }
        }, current);
    }


    ModuleMermaidExporter::ModuleMermaidExporter(const std::filesystem::path& path) : m_path(path) {
        // TODO: Some checking on the path. maybe erorr messages etc.
    }

    ModuleMermaidExporter::~ModuleMermaidExporter() {}

}  // namespace HwTool
