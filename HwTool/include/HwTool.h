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
        ModulePack m_addCardBuffer;
        void resolveLinking(std::unordered_map<std::string, Module>& modules);
    public:
        Hw(/* args */);
        ~Hw();

        void importCSV(const std::filesystem::path& path, const std::string& version = "");
        void importHW(const std::filesystem::path& path, const std::string& version = "");
        void LinkToTarget(const std::string& targetModule);
        void createCard(const std::string& name, cardType type);
        std::vector<std::string> getAvailableCards();
        
        void exportHW(const std::filesystem::path& path);
        void exportMermaid(const std::filesystem::path& path);

        // not implemented
        void render();
    
    };
    
} // namespace HwTool


