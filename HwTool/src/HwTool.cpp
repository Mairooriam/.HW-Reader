#include "HwTool.h"

#include <filesystem>
#include <iostream>
#include <ranges>

#include "Commands/Commands.h"
#include "DataAccess/ModuleMermaidExporter.h"
#include "DataAccess/ModuleXmlExporter.h"
#include "DataAccess/ModuleXmlImporter.h"
#include "HardwareBuilder.h"
#include "HwUtils.h"


namespace HwTool {
    // TODO: make uses m_modules instead of supplying source.
    void Hw::resolveLinking() {
        for (auto& [name, module] : m_modules) {
            for (auto& con : module.connections) {
                if (!con.targetModuleName.empty()) {
                    auto it = m_modules.find(con.targetModuleName);
                    if (it != m_modules.end()) {
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


    std::string Hw::getCardBase(const std::string& target) {
        // TODO: add testing? just got up and runnig
        // Test some edge cases

        auto it = m_modules.find(target);
        if (it == m_modules.end()) {
            return std::string();
        } else {
            for (auto&& con : it->second.connections) {
                if (con.connector == ConnectorType::SL) {
                    return con.targetModuleName;
                }
            }
        }
        return std::string();
    }

    std::string Hw::getCardSource(const std::string& target) {
        auto targetCardBase = getCardBase(target);
        auto targetSource = getModuleWithConnectionSource(targetCardBase);
        return targetSource;
    }

    std::string Hw::getCardTarget(const std::string& target) {
        auto targetCardBase = getCardBase(target);
        auto targetTarget = getModuleWithConnectionTarget(targetCardBase);
        return targetTarget;
    }

    std::string Hw::getModuleWithConnectionSource(const std::string& target) {
        // TODO: add cached target info. this is bad
        for (const auto& [name, module] : this->getModules()) {
            for (const auto& con : module.connections) {
                if (con.targetModuleName == target && con.connector == ConnectorType::X2X1) {
                    // Found a module with a connection to 'target'
                    return name;
                }
            }
        }
        return std::string();
    }

    std::string Hw::getModuleWithConnectionTarget(const std::string& module) {
        auto modules = getModules();
        for (auto&& con : modules[module].connections) {
            if (con.connector == ConnectorType::X2X1) {
                return con.targetModuleName;
            }
        }
        return std::string();
    }

    std::set<ConnectorType> Hw::getModuleConnectors(const Module& module) {
        std::set<ConnectorType> res;
        for (auto&& con : module.connections) {
            res.insert(con.connector);
            res.insert(con.targetConnector);
        }
        return res;
    }

    std::string Hw::getRootBase(const ModuleMap& modules) {
        std::vector<std::string> emptyTargets;
        for (auto& [modName, module] : modules) {
            if (Utils::isBase(module))
            {
                if (!module.connections.empty())
                {
                    for (auto& connection : module.connections) {
                        if (connection.targetModuleName.empty()) {
                            emptyTargets.push_back(modName);
                        }
                    }
                }else {
                    emptyTargets.push_back(modName);
                }
                
            }
            
        }
       
        

        // TODO: hitting this is edge case that will cause bug later sorry :))
        if (emptyTargets.size() > 1)
        {
            assert(("Multiple root bases found", false));
        }
        
        if (emptyTargets.size() == 1)
            return emptyTargets[0];

        return std::string();
    }

    std::string Hw::getBaseWithoutTarget(const std::unordered_map<std::string,Module>& modules)
    {
        std::set<std::string> baseNames;
        std::set<std::string> pointedBases;

        // Collect all base module names
        for (const auto& [name, module] : modules) {
            if (Utils::isBase(module)) {
                baseNames.insert(name);
            }
        }

        // Collect all targetModuleNames from all connections
        for (const auto& [name, module] : modules) {
            if (Utils::isBase(module))
            {
                for (const auto& con : module.connections) {
                    if (!con.targetModuleName.empty()) {
                        pointedBases.insert(con.targetModuleName);
                    }
                }
            }
            
        }

        // Find base that is not pointed by anything
        for (const auto& base : baseNames) {
            if (!pointedBases.contains(base)) {
                return base;
            }
        }

        return std::string();
    }

    Hw::Hw() : m_cmdManager(*this) {}

    Hw::~Hw() {}

    void Hw::importHW(const std::filesystem::path& path, const std::string& version) {
        printf("importing HW\n");

        ModuleXmlImporter importer(path);
        if (importer.valid()) {
            importer.mapModules();
            m_modules = importer.getModules();

        } else {
            importer.printErrors();
        }
        resolveLinking();
        // TODO: status printing if it was succesfull etc.
    }

    void Hw::LinkToTargetInternal(const std::string& targetModule) {
        assert(!m_addCardBuffer.empty() &&
               "You must call createCard() before calling LinkToTarget()");
        auto tSource = getCardSource(targetModule);
        auto TargetModuleBase = getCardBase(targetModule);
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

        resolveLinking();
    }

    void Hw::createCard(const std::string& name, cardType type) {
        HardwareBuilder hwb;
        auto keys = m_modules | std::views::keys;
        auto currentModules = std::set<std::string>(keys.begin(), keys.end());
        m_addCardBuffer = hwb.createCard(name, type, currentModules);
    }

    void Hw::deleteCardInternal(const std::string& name) {
        // TODO: these should be reworked. easy to make bugs with the methods
        // TODO: test edge cases.
        auto modules = this->getModules();

        std::vector<std::string> modulesToDelete;
        for (auto&& con : modules[name].connections) {
            if (con.connector == ConnectorType::SS1 || con.connector == ConnectorType::SL) {
                modulesToDelete.push_back(con.targetModuleName);
            }
        }

        auto deleteCardSource = getCardSource(name);
        auto deleteCardTarget = getCardTarget(name);

        for (auto&& module : modulesToDelete) {
            m_modules.erase(module);
        }
        m_modules.erase(name);

        for (auto&& con : m_modules[deleteCardSource].connections) {
            if (con.connector == ConnectorType::X2X1) {
                con.targetModuleName = deleteCardTarget;
            }
        }

        this->resolveLinking();
    }

    void Hw::linkToTarget(const std::string& targetModule) {
        m_cmdManager.execute(std::make_unique<LinkToTargetCommand>(targetModule));
    }

    void Hw::undo() {
        m_cmdManager.undo();
    }

    void Hw::redo() {
        m_cmdManager.redo();
    }

    void Hw::deleteCard(const std::string& card) {
        m_cmdManager.execute(std::make_unique<deleteCardCommand>(card));
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

    
    ModuleMap Hw::importCSV(const std::filesystem::path& path, const std::string& version) {
        auto keys = m_modules | std::views::keys;
        auto currentModules = std::set<std::string>(keys.begin(), keys.end());
        ModuleCsvImporter csvImporter(path,currentModules);
        if (csvImporter.valid())
            return csvImporter.getModules();
   
        return ModuleMap();
    }

    void Hw::combineToExisting(ModuleMap& modules, const std::string& target) {
        // TODO: bug when adding modules in batch inbetween existing HW.
        // ->> re linkin of old modules is not implmeented correctly
        // So only linking to "end of" X20BM11 works. 
        // Need to implement propagateTargetUpdateUpTheChain();


        auto rootBase = getRootBase(modules);
        auto testing = Utils::getRootBase(modules);
        auto rootBaseTarget = getRootBase(m_modules);

        auto targetBase = Utils::getCardBase(target, m_modules);
        auto targetBaseSource = Utils::getBaseSource(targetBase, m_modules); 
        modules[rootBase].connections[0].targetModuleName = targetBase;
        for (const auto& [name, module] : modules) {
            m_modules[name] = module;
        }
        auto rootBaseSource = Utils::getBaseSource(rootBase, m_modules); //BM11f
        auto importEnd = getBaseWithoutTarget(modules);
        
        // if target doesnt have source skip changing the targets of the previous cards. since they dont exist
        if (!targetBaseSource.empty()){
            //m_modules[rootBaseSource].connections[0].targetModuleName = targetBaseSource;
            m_modules[targetBaseSource].connections[0].targetModuleName = importEnd;
        }
        resolveLinking();
    }

}  // namespace HwTool
