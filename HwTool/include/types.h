#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <magic_enum/magic_enum.hpp>
#include <tinyxml2.h>

//TODO: move implementations to .cpp file for the printings or separate

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

struct Module;

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

inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<std::string, Module>& modules) {
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