#include "types.h"
#include <iostream>
#include <cassert>

namespace HwTool::Utils {
    std::string getRootBase(const ModuleMap& m) {
        std::vector<std::string> emptyTargets;
        for (auto& [k, v] : m) {
            if (Utils::isValidBase(v))
            {
                if (v.connections[0].targetModuleName.empty())
                {
                    emptyTargets.push_back(v.name);
                }
                
            }
            
        }

        // TODO: hitting this is edge case that will cause bug later sorry :))
        // TODO: will remain. get root base should be called with cache with only one root
        if (emptyTargets.size() > 1)
        {
            assert(("Multiple root bases found", false));
        }
        
        if (emptyTargets.size() == 1)
            return emptyTargets[0];

 
        return std::string();
    }

    //TODO: For now looping trough all as "sanity check" but remove later
    std::string getEndBase(const ModuleMap& m) {
        std::vector<std::string> res;
        for (const auto &[k,v] : m)
        {
            if (!Utils::isValidBase(v))
                continue;
            
            
            if(Utils::getBaseSource(v, m).empty());
                res.push_back(v.name);

        }
        
        assert(res.size() > 1 && "Edge case not handled! GL :)");
        
        if (res.size() == 0)
            return std::string();

        return res[0];
    }

    std::string getCardBase(const Module& c) {
        if (!isCard(c)) {
            std::cerr << "[Utils Error] Module '" << c.name << "' is not a card type" << std::endl;
            assert(false && "getCardBase: Module is not a card or CPU type");
            return std::string();
        }
        
        if (c.connections.size() < 1 || c.connections.size() > 2) {
            std::cerr << "[Utils Error] Card '" << c.name << "' must have 1 or 2 connections, but has " << c.connections.size() << std::endl;
            assert(false && "getCardBase: Card must have 1 or 2 connections");
            return std::string();
        }
        return c.connections[1].targetModuleName;
    }

    std::string getCardBase(const std::string& b, const ModuleMap& m) {
        auto it = m.find(b);
        if (it == m.end()) {
            std::cerr << "[WARNING] getCardBase(string, modulemap) - Couldn't find base from the supplied map" << std::endl;
            assert(false && "getCardBase: Module not found in map");
            return std::string();
        }
    
        return getCardBase(it->second);
    }

    std::string getCpuBase(const Module& c) {
        if (!isCpu(c)) {
            std::cerr << "[Utils Error] Module '" << c.name << "' is not a CPU type" << std::endl;
            assert(false && "getCpuBase: Module is not a CPU type");
            return std::string();
        }
        
        //TODO: this will fail on new CPUS
        if (c.connections.size() != 1) {
            std::cerr << "[Utils Error] Card '" << c.name << "' must have 1 connections, but has " << c.connections.size() << std::endl;
            assert(false && "getCpuBase: Card must have 1 connections");
            return std::string();
        }
        return c.connections[0].targetModuleName;
    }

    std::string getCpuBase(const std::string& c, const ModuleMap& m) {
        auto it = m.find(c);
        if (it == m.end()) {
            std::cerr << "[WARNING] getCpuBase(string, modulemap) - Couldn't find Cpu from the supplied map" << std::endl;
            assert(false && "getCpuBase: Module not found in map");
            return std::string();
        }
    
        return getCpuBase(it->second);
    }

    std::string getBaseTarget(const Module& b) {
        if (!isBase(b)) {
            std::cerr << "[Utils Error] Module '" << b.name << "' is not a base type" << std::endl;
            assert(false && "getBaseTarget: Module is not a base type");
            return std::string();
        }
        
        if (b.connections.empty()) {
            std::cerr << "[Utils Error] Base '" << b.name << "' has no connections" << std::endl;
            assert(false && "getBaseTarget: Base has no connections");
            return std::string();
        }
        
        return b.connections[0].targetModuleName;
    }

    std::string getBaseSource(const Module& b, const ModuleMap& m) {
        if (!isBase(b)) {
            std::cerr << "[Utils Error] Module '" << b.name << "' is not a base type" << std::endl;
            assert(false && "getBaseSource: Module is not a base type");
            return std::string();
        }
        
        for (const auto& [k, v] : m) {
            if (!Utils::isValidBase(v))
                continue;
                
            if (v.connections[0].targetModuleName == b.name)
                return v.name;
        }
        

        std::cerr << "std::string getBaseSource(Module, ModuleMap) Failed\n";
        return std::string();
    }

    std::string getBaseSource(const std::string& b, const ModuleMap& m) {
        auto it = m.find(b);
        if (it == m.end()){
            std::cerr << "[Utils Error] getBaseSource(string, map): Base '" << b << "' not found in map" << std::endl;
            assert(false && "getBaseSource: Base not found in map");
            return std::string();
        }
        
        return getBaseSource(it->second, m);
    }

    std::string getCpuBaseSource(const Module& b, const ModuleMap& m) {
            if (!isCpuBase(b)) {
            std::cerr << "[Utils Error] Module '" << b.name << "' is not a Cpubase type" << std::endl;
            assert(false && "getCpuBaseSource: Module is not a base type");
            return std::string();
        }
        
        for (const auto& [k, v] : m) {
            if (!Utils::isValidBase(v)) //TODO: currently this is causing comparison only against normal bases not bus bases
                continue;
                
            if (v.connections[0].targetModuleName == b.name)
                return v.name;
        }
        

        std::cerr << "std::string getBaseSource(Module, ModuleMap) Failed\n";
        return std::string();
    }

    std::string getCpuBaseSource(const std::string& b, const ModuleMap& m) {
        auto it = m.find(b);
        if (it == m.end()){
            std::cerr << "[Utils Error] getCpuBaseSource(string, map): Base '" << b << "' not found in map" << std::endl;
            assert(false && "getCpuBaseSource: Base not found in map");
            return std::string();
        }
        
        return getCpuBaseSource(it->second, m);
    }

    std::vector<std::string> getModulesWithConnector(ConnectorType t, const ModuleMap& m) {
        std::vector<std::string> res;

        for (const auto &[k,v] : m)
        {
            for (const auto & c: v.connections)
            {
                if (c.connector == t)
                {
                    res.push_back(v.name);
                }
                   
            }
            
        }
        
        return res;
    }

    std::string getCardSource(const Module& c) {
        assert(true && "NOT IMPLEMENTED");
        if (!isCard(c)) {
            std::cerr << "[Utils Error] Module '" << c.name << "' is not a card type" << std::endl;
            assert(false && "getCardSource: Module is not a card type");
            return std::string();
        }
        
        return std::string();
    }

    bool isValidBase(const Module& m) {
        if (!isBase(m)) {
            std::cout << "[Validation Info] Module '" << m.name << "' is not a base type" << std::endl;
            //TODO: add assert here later once its working -> change it so only "cached" data with valid data is supplied
            return false;
        }
        
        if (m.connections.size() != 1) {
            std::cerr << "[Validation Error] Base module '" << m.name 
                      << "' should have exactly 1 connection, but has " 
                      << m.connections.size() << std::endl;
            assert(false && "isValidBase: Base doesn't have exactly 1 connection");
            return false;
        }
        
        return true;
    }
    
    bool isValidCard(const Module& m) {
        if (!isCard(m)) {
            std::cerr << "[Validation Error] Module '" << m.name << "' is not a card type" << std::endl;
            assert(false && "isValidCard: Module is not a card type");
            return false;
        }
        
        if (m.connections.size() != 2) {
            std::cerr << "[Validation Error] Card module '" << m.name 
                      << "' should have exactly 2 connections, but has " 
                      << m.connections.size() << std::endl;
            assert(false && "isValidCard: Card doesn't have exactly 2 connections");
            return false;
        }
        
        return true;
    }
    
    bool isValidCpu(const Module& m) {
        if (!isCpu(m)) {
            std::cerr << "[Validation Error] Module '" << m.name << "' is not a CPU type" << std::endl;
            assert(false && "isValidCpu: Module is not a CPU type");
            return false;
        }
        
        if (m.connections.empty()) {
            std::cerr << "[Validation Error] CPU module '" << m.name 
                      << "' should have at least 1 connection" << std::endl;
            assert(false && "isValidCpu: CPU has no connections");
            return false;
        }
        
        return true;
    }
    
    bool isValidModule(const Module& m) {
        if (isBase(m)) return isValidBase(m);
        if (isCard(m)) return isValidCard(m);
        if (isCpu(m)) return isValidCpu(m);
        
        std::cerr << "[Validation Error] Module '" << m.name << "' has unsupported type for validation" << std::endl;
        assert(false && "isValidModule: Unsupported module type for validation");
        return false; 
    }

}