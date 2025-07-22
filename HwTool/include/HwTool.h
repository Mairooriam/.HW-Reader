#pragma once
#include <filesystem>
#include "types.h"
#include <unordered_map>
namespace HwTool
{

    class Hw 
    {
    private:
        std::unordered_map<std::string, Module> m_modules;
        void resolveLinking(std::unordered_map<std::string, Module>& modules);
    public:
        Hw(/* args */);
        ~Hw();
    
        void importCSV(const std::filesystem::path& path, const std::string& version = "");
        void importHW(const std::filesystem::path& path, const std::string& version = "");

        bool addCard(const std::string& name, cardType type, const std::string& targetName);
        
        void exportHW(const std::filesystem::path& path);
        void exportMermaid(const std::filesystem::path& path);

        // not implemented
        void render();
    
    };
    
} // namespace HwTool


