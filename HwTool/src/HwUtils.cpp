#include "HwUtils.h"
#include <unordered_map>
#include <string>
#include <types.h>
std::string getCardBase(const std::string& target, const std::unordered_map<std::string, Module>& modules) {
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

std::string getCardSource(const std::string& target,
                        const std::unordered_map<std::string, Module>& modules) {
    
    auto test = getCardBase(target, modules);
    auto test2 = ModuleWithConnectionTarget(test, modules);
    return test2;
}

std::string ModuleWithConnectionTarget(const std::string& target, const std::unordered_map<std::string, Module>& modules) {
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

std::set<ConnectorType> getModuleConnectors(const Module& module) {
    std::set<ConnectorType> res;
    for (auto &&con : module.connections)
    {
        res.insert(con.connector);
        res.insert(con.targetConnector);
    }
    return res;
}

// std::set<std::string> getAllBases(const std::unordered_map<std::string, Module>& modules) {

//     return std::set<std::string>();
// }
