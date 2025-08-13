#pragma once
#include <filesystem>
#include <memory>
#include <set>
#include <unordered_map>

#include "Commands/ICommand.h"
#include "DataAccess/ModuleCsvImporter.h"
#include "HwUtils.h"
#include "Renderer/IRenderer.h"
#include "Types.h"
#include "RenderData.h"
// TODO: make addCardBatch ->> addCard has performance problems regarding iterating over m_modules +
// incrementStr etc. could be fixed internally but easiest is "batch" add
// TODO: Rework getCardBase, getCardSource, getCardTarget. relies on strings not strongly typed.
// easy to make misstake
// TODO: Rework types.h module implementation. will take alot. currently works on raw pointers.
// Would benefit from smart pointers and shared pointers.
// TODO: Remove LinkToTargetInternal etc.
namespace HwTool {


    class Hw {
        public:
        ModuleMap m_modules;
    private:
        ModuleMap m_cacheModules;
        ModuleMap m_cacheBase;
        ModuleMap m_cacheCard;
        ModuleMap m_cacheImportCsv;

        std::unordered_map<std::string, std::string> m_cacheBaseLink;
        std::unordered_map<std::string, std::string> m_cacheBaseLinkReverse;

        std::vector<std::string> m_chain;
        std::vector<std::string> m_cardChain;

        IRenderer* m_renderer = nullptr;

        ModuleMap getModules() const {
            return m_modules;
        };
        void setModules(const ModuleMap& modules) {
            m_modules = modules;
            resolveLinking();
        }
        ModulePack m_addCardBuffer;
        void resolveLinking();
        CommandManager m_cmdManager;
        void LinkToTargetInternal(const std::string& targetModule);
        void deleteCardInternal(const std::string& name);
        void rebuildCaches();

        // UTILS
        std::string getCardBase(const std::string& card);
        std::string getCardSource(const std::string& card);
        std::string getCardTarget(const std::string& card);
        std::vector<std::string> getModulesToDelete(const std::string& card);
        std::string getModuleWithConnectionTarget(const std::string& target);
        std::string getModuleWithConnectionSource(const std::string& module);
        std::set<ConnectorType> getModuleConnectors(const Module& module);
        std::string getRootBase(const ModuleMap& modules);
        std::string getBaseWithoutTarget(const ModuleMap& modules);

    public:
        Hw(/* args */);
        ~Hw();

        void importHW(const std::filesystem::path& path, const std::string& version = "");
        void deleteCard(const std::string& name);
        void linkToTarget(const std::string& targetModule);
        void createCard(const std::string& name, cardType type);
        std::vector<std::string> getAvailableCards();

        // void executeCommand(std::unique_ptr<ICommand> cmd);

        void exportHW(const std::filesystem::path& path);
        void exportMermaid(const std::filesystem::path& path);

        // not implemented
        void render();
        void setRenderer(IRenderer* renderer) { m_renderer = renderer; }
        ModuleMap importCSV(const std::filesystem::path& path, const std::string& version = "");
        ModuleMap importCSV2(const std::filesystem::path& path, const std::string& version = "");
        void combineToExisting(ModuleMap& modules, const std::string& target);

        void undo();
        void redo();

        const ModuleMap& getCardCache() { return m_cacheCard; };
        
        //TODO: make const
        RenderData getRenderData() {
            return RenderData(m_modules, m_cacheBaseLink, m_cacheBaseLinkReverse,  m_cacheModules, 
                            m_cacheBase, m_cacheCard, m_cacheImportCsv, getAvailableCards(), m_chain, m_cardChain);
        }
    private:
        friend class LinkToTargetCommand;
        friend class deleteCardCommand;
    };

}  // namespace HwTool
