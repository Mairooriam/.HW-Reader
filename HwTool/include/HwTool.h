#pragma once
#include <filesystem>
#include <memory>
#include <set>
#include <unordered_map>

#include "ICommand.h"
#include "IRenderer.h"
#include "Types.h"

namespace HwTool {

    class Hw {
    private:
        std::unordered_map<std::string, Module> m_modules;
        std::unordered_map<std::string, Module> getModules() const {
            return m_modules;
        };
        ModulePack m_addCardBuffer;
        void resolveLinking();
        CommandManager m_cmdManager;

        void LinkToTargetInternal(const std::string& targetModule);

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

        void importCSV(const std::filesystem::path& path, const std::string& version = "");
        void importHW(const std::filesystem::path& path, const std::string& version = "");

        void linkToTarget(const std::string& targetModule);
        void createCard(const std::string& name, cardType type);
        void deleteCard(const std::string& name);
        std::vector<std::string> getAvailableCards();

        // void executeCommand(std::unique_ptr<ICommand> cmd);

        void exportHW(const std::filesystem::path& path);
        void exportMermaid(const std::filesystem::path& path);

        // not implemented
        void render(IRenderer& renderer) const;

    private:
        friend class LinkToTargetCommand;
    };

}  // namespace HwTool
