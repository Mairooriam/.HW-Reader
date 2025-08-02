#pragma once
#include <tinyxml2.h>
#include <unordered_map>
#include <string>
#include "Types.h"
#include <filesystem>
namespace HwTool{
    class ModuleMermaidExporter
    {
    private:
        std::filesystem::path m_path;
        void traverseAndOutput(std::ostringstream& oss, const V2::ModuleVariant& current);
    public:
        void serialize(const std::unordered_map<std::string, Module>& modules);
        void serialize(const V2::ModuleMap& m); 
        ModuleMermaidExporter(const std::filesystem::path& path);
        ~ModuleMermaidExporter();
    };
}    
