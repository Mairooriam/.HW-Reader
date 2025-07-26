#pragma once
#include <filesystem>
#include <memory>
#include <set>
#include <unordered_map>

#include "Commands/ICommand.h"
#include "Renderer/IRenderer.h"
#include "Types.h"
#include "HwUtils.h"
#include "DataAccess/ModuleCsvImporter.h"
//TODO: make addCardBatch ->> addCard has performance problems regarding iterating over m_modules + incrementStr etc. could be fixed internally but easiest is "batch" add
//TODO: Rework getCardBase, getCardSource, getCardTarget. relies on strings not strongly typed. easy to make misstake
//TODO: Rework types.h module implementation. will take alot. currently works on raw pointers. Would benefit from smart pointers and shared pointers.
//TODO: Remove LinkToTargetInternal etc.
namespace HwTool {

    class Hw {
    private:
    std::unordered_map<std::string, Module> m_modules;
    std::unordered_map<std::string, Module> getModules() const {
        return m_modules;
    };
    void setModules(const std::unordered_map<std::string, Module>& modules) {
        m_modules = modules;
        resolveLinking();
    }
    ModulePack m_addCardBuffer;
    void resolveLinking();
    CommandManager m_cmdManager;
    
    void LinkToTargetInternal(const std::string& targetModule);
    void deleteCardInternal(const std::string& name);
    
    // void createCardInternal(const std::string& name, cardType type);
    
    // UTILS
    std::string getCardBase(const std::string& card);
    std::string getCardSource(const std::string& card);
    std::string getCardTarget(const std::string& card);
    std::vector<std::string> getModulesToDelete(const std::string& card);
    std::string getModuleWithConnectionTarget(const std::string& target);
    std::string getModuleWithConnectionSource(const std::string& module);
    std::set<ConnectorType> getModuleConnectors(const Module& module);
    
    public:
    
    Hw(/* args */);
    ~Hw();
    
        std::string getRootBase(const std::unordered_map<std::string, Module>& modules);
        void importHW(const std::filesystem::path& path, const std::string& version = "");
        void deleteCard(const std::string& name);
        void linkToTarget(const std::string& targetModule);
        void createCard(const std::string& name, cardType type);
        std::vector<std::string> getAvailableCards();

        // void executeCommand(std::unique_ptr<ICommand> cmd);

        void exportHW(const std::filesystem::path& path);
        void exportMermaid(const std::filesystem::path& path);

        // not implemented
        void render(IRenderer& renderer) const;
        std::unordered_map<std::string, Module> importCSV(const std::filesystem::path& path, const std::string& version = "");
        void combineToExisting(std::unordered_map<std::string, Module>& modules, const std::string& root, const std::string& target);

        void undo();
        void redo();

    private:
        friend class LinkToTargetCommand;
        friend class deleteCardCommand;
    };

}  // namespace HwTool
