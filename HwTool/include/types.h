#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <magic_enum/magic_enum.hpp>
#include <tinyxml2.h>
#include <algorithm>
#include <iostream>
//TODO: move implementations to .cpp file for the printings or separate

template<typename T, typename Key>
concept HasContains = requires(T t, Key k) {
    { t.contains(k) } -> std::convertible_to<bool>;
};
namespace HwTool{
    
    struct Module;

    using ModuleMap = std::unordered_map<std::string, Module>;
    
    enum class cardType{
        ERROR = 0,
    
        //CARDS
        X20DO9322 = 1,
        X20DI9372,
        X20AI4622,
        X20AI4632,
        
        //BASES
        X20BB52,
        X20BB80,
        X20BM11,
    
        //CONNECTION_BLOCKS
        X20TB12,
    
        //CPUS
        X20CP0484_1,
        
    
        //POWERS
        X20PS9600,
    
        //EXTENSION_CARDS
        X20BC0083,
    
        NONE = 128,
    };
  
    
    enum class ConnectorType{
        ERROR = 0,
        SS1 = 1,
        SL = 2,
        SL1,
        SS,
        PS,
        PS1,
        X2X1,
        X2X2,
        IF6,
    
        NONE = 128,
    };
    
    
    struct Connection{
        ConnectorType connector;
        std::string targetModuleName;
        Module* targetModule;
        ConnectorType targetConnector;
    
        Connection() = default;
        Connection(ConnectorType conn, ConnectorType targetConn, Module* targetMod = nullptr, std::string targetName = "")
            : connector(conn), targetModuleName(std::move(targetName)), targetModule(targetMod), targetConnector(targetConn) {}
    };
    
    struct Parameter{
        std::string id;
        std::string value;
    };
    
    struct Group{
        std::string id;
    };
    
    struct Connector{
        std::string name;
        std::vector<Parameter> parameters;
    };
    
    struct Module{
        std::string name;
        cardType type;
        std::string version;
        std::vector<Connection> connections;
        std::vector<Parameter> parameters;
        Connector connector;
        Group group;
    
        Module() = default;
        Module(std::string n, cardType t, std::string v,
               std::vector<Connection> conns = {},
               std::vector<Parameter> params = {},
               Group g = {})
            : name(std::move(n)), type(t), version(std::move(v)),
              connections(std::move(conns)), parameters(std::move(params)), group(std::move(g)) {}
    };
    
    struct ModulePack {
        Module card, base, tb;
        bool empty() const {
            return card.name.empty() && base.name.empty() && tb.name.empty();
        }
    };
    namespace Utils{

        //TODO: later maybe vetor? -> dynamic -> load from json etc.
        constexpr std::array<cardType, 3> baseTypes = {
            cardType::X20BM11
        };

        constexpr std::array<cardType, 10> cardTypes = {
            cardType::X20AI4622,
            cardType::X20AI4632,
            cardType::X20DI9372,
            cardType::X20DO9322,
        };


        constexpr bool isBase(const Module& m) {
            return std::find(baseTypes.begin(), baseTypes.end(), m.type) != baseTypes.end();
        }
        constexpr bool isCard(const Module& m) {
            return std::find(cardTypes.begin(), cardTypes.end(), m.type) != cardTypes.end();
        }
        constexpr bool isCpu(const Module& m) {
            return m.type == cardType::X20CP0484_1;
        }
        constexpr bool isBus(const Module& m) {
            // Implement logic for bus type if needed, currently returns false
            return false;
        }

        std::string getRootBase(const ModuleMap& m);
        std::string getEndBase(const ModuleMap& m);
        std::string getModuleBase(const Module& c);
        std::string getModuleBase(const std::string& b, const ModuleMap& m);
        std::string getBaseTarget(const Module& b);
        std::string getBaseSource(const Module& b, const ModuleMap& m);
        std::string getBaseSource(const std::string& b, const ModuleMap& m);
        std::vector<std::string> getModulesWithConnector(ConnectorType t, const ModuleMap& m);
        std::string getCardSource(const Module& c);
        
        bool isValidBase(const Module& m);
        bool isValidCard(const Module& m);
        bool isValidCpu(const Module& m);
        bool isValidModule(const Module& m);
    }
    inline std::ostream& operator<<(std::ostream& os, const Connection& c) {
        os << "Connection(connector=" << "[" << static_cast<int>(c.connector) << "]:" << magic_enum::enum_name(c.connector)
           << ", targetModule=" << (c.targetModule ? c.targetModule->name : "nullptr")
           << ", targetConnector=" << "[" << static_cast<int>(c.targetConnector) << "]:" << magic_enum::enum_name(c.targetConnector)
           << ")";
        return os;
    }
    
    inline std::ostream& operator<<(std::ostream& os, const Parameter& p) {
        os << "Parameter(id=" << p.id << ", value=" << p.value << ")";
        return os;
    }
    
    inline std::ostream& operator<<(std::ostream& os, const Group& g) {
        os << "Group(id=" << g.id << ")";
        return os;
    }
    
    inline std::ostream& operator<<(std::ostream& os, const Connector& c) {
        os << "Connector(name=" << c.name << ", parameters=[";
        for (size_t i = 0; i < c.parameters.size(); ++i) {
            os << c.parameters[i];
            if (i + 1 < c.parameters.size()) os << ", ";
        }
        os << "])";
        return os;
    }
    
    inline std::ostream& operator<<(std::ostream& os, const Module& m) {
        os << "Module(name=" << m.name
           << ", type=[" << static_cast<int>(m.type) << "]:" << magic_enum::enum_name(m.type)
           << ", version=" << m.version
           << ", connections=[";
        for (size_t i = 0; i < m.connections.size(); ++i) {
            os << m.connections[i];
            if (i + 1 < m.connections.size()) os << ", ";
        }
        os << "], parameters=[";
        for (size_t i = 0; i < m.parameters.size(); ++i) {
            os << m.parameters[i];
            if (i + 1 < m.parameters.size()) os << ", ";
        }
        os << "], group=" << m.group;
        os << ")";
        return os;
    }
    
    inline std::ostream& operator<<(std::ostream& os, const ModuleMap& modules) {
        os << "{\n";
        size_t count = 0;
        for (const auto& [key, value] : modules) {
            os << "  \"" << key << "\": " << value;
            if (++count < modules.size()) os << ",";
            os << "\n";
        }
        os << "}";
        return os;
    }
}