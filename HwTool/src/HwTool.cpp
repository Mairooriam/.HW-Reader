#include "HwTool.h"

#include <filesystem>
#include <iostream>
#include <ranges>

#include "HardwareBuilder.h"
#include "ModuleMermaidExporter.h"
#include "ModuleXmlExporter.h"
#include "ModuleXmlImporter.h"
#include "HwUtils.h"
#include "Commands/Commands.h"

namespace HwTool {
    // TODO: make uses m_modules instead of supplying source.
    void Hw::resolveLinking(std::unordered_map<std::string, Module>& modules) {
        for (auto& [name, module] : modules) {
            for (auto& con : module.connections) {
                if (!con.targetModuleName.empty()) {
                    auto it = modules.find(con.targetModuleName);
                    if (it != modules.end()) {
                        con.targetModule = &it->second;
                    } else {
                        std::cerr << "Warning: Target module '" << con.targetModuleName
                                  << "' not found for connection in module '" << name << "'\n";
                        con.targetModule = nullptr;
                    }
                }
            }
        }
    }

    std::string Hw::getCardBase(const std::string& target,
                                const std::unordered_map<std::string, Module>& modules)
    {
        // TODO: add testing? just got up and runnig
        // Test some edge cases
        
        auto it = modules.find(target);
        if (it == modules.end())
        {
            return std::string();
        }else{
            for (auto &&con : it->second.connections)
            {
                if (con.connector == ConnectorType::SL)
                {
                    return con.targetModuleName;
                }
                
            }
        }
        return std::string();
    }

    std::string Hw::getCardSource(const std::string& target,
                                  const std::unordered_map<std::string, Module>& modules)  {
    
        auto targetCardBase = getCardBase(target, modules);
        auto targetSource = ModuleWithConnectionTarget(targetCardBase, modules);
        return targetSource;
    }

    std::string Hw::ModuleWithConnectionTarget(
        const std::string& target, const std::unordered_map<std::string, Module>& modules) {
        //TODO: add cached target info. this is bad
        for (const auto& [name, module] : modules) {
            for (const auto& con : module.connections) {
                if (con.targetModuleName == target && con.connector == ConnectorType::X2X1) {
                    // Found a module with a connection to 'target'
                    return name;
                }
            }
        }
        return std::string();
    }

    std::set<ConnectorType> Hw::getModuleConnectors(const Module& module) {
            std::set<ConnectorType> res;
            for (auto &&con : module.connections)
            {
                res.insert(con.connector);
                res.insert(con.targetConnector);
            }
            return res;
    }

    Hw::Hw() : m_cmdManager(*this) {}

    Hw::~Hw() {}

    void Hw::importCSV(const std::filesystem::path& path, const std::string& version) {
        printf("Importing CSV\n");
    }

    void Hw::importHW(const std::filesystem::path& path, const std::string& version) {
        printf("importing HW\n");

        ModuleXmlImporter importer(path);
        if (importer.valid()) {
            importer.mapModules();
            m_modules = importer.getModules();

        } else {
            importer.printErrors();
        }
        resolveLinking(m_modules);
        // TODO: status printing if it was succesfull etc.
    }

    void Hw::LinkToTargetInternal(const std::string& targetModule) {
        assert(!m_addCardBuffer.empty() &&
               "You must call createCard() before calling LinkToTarget()");
        auto tSource = getCardSource(targetModule, m_modules);
        auto TargetModuleBase = getCardBase(targetModule, m_modules);
        // TODO: bad could cause crash. make validation somewhere so cannot
        m_addCardBuffer.base.connections[0].targetModuleName = TargetModuleBase;

        // if tSource empty -> nothing linked to that module. "top module" no need to update linking
        // for it
        if (!tSource.empty()) {
            m_modules[tSource].connections[0].targetModuleName = m_addCardBuffer.base.name;
        }
        m_modules[m_addCardBuffer.base.name] = m_addCardBuffer.base;
        m_modules[m_addCardBuffer.tb.name] = m_addCardBuffer.tb;
        m_modules[m_addCardBuffer.card.name] = m_addCardBuffer.card;

        resolveLinking(m_modules);
    }

    void Hw::createCard(const std::string& name, cardType type) {
        HardwareBuilder hwb;
        auto keys = m_modules | std::views::keys;
        auto currentModules = std::set<std::string>(keys.begin(), keys.end());
        m_addCardBuffer = hwb.createCard(name, type, currentModules);
    }

    void Hw::deleteCard(const std::string& name) {

    }

    void Hw::linkToTarget(const std::string& targetModule) {
        m_cmdManager.execute(std::make_unique<LinkToTargetCommand>(targetModule));
    }

    std::vector<std::string> Hw::getAvailableCards() {
        assert(!m_addCardBuffer.empty() &&
               "You must call createCard() before calling getAvailableCards()");
        std::vector<std::string> res;

        auto connectors = getModuleConnectors(m_addCardBuffer.card);
        if (connectors.contains(ConnectorType::SL)) {
            for (const auto& [name, module] : m_modules) {
                for (const auto& con : module.connections) {
                    if (con.connector == ConnectorType::SL) {
                        res.push_back(name);
                    }
                }
            }
        }
        return res;
    }



    void Hw::exportHW(const std::filesystem::path& path) {
        printf("Exporting HW\n");
        ModuleXmlExporter exporter;
        exporter.serialize(m_modules, path.string());
    }

    void Hw::exportMermaid(const std::filesystem::path& path) {
        printf("Exporting to mermaid\n");
        ModuleMermaidExporter exporter(path);
        exporter.serialize(m_modules);
    }

    void Hw::render(IRenderer& renderer) const {
        renderer.render(m_modules);
    }

}  // namespace HwTool
