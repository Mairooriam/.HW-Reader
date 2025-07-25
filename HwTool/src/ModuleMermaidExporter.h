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
    public:
        void serialize(const std::unordered_map<std::string, Module>& modules);
        
        ModuleMermaidExporter(const std::filesystem::path& path);
        ~ModuleMermaidExporter();
    };
}    
