#pragma once
#include <tinyxml2.h>

#include <array>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

// TODO: move implementations to .cpp file for the printings or separate

template <typename T, typename Key>
concept HasContains = requires(T t, Key k) {
    { t.contains(k) } -> std::convertible_to<bool>;
};
enum class cardType {
    ERROR = 0,

    // CARDS
    X20DO9322 = 1,
    X20DI9372,
    X20AI4622,
    X20AI4632,

    // BASES
    X20BB52,
    X20BB80,
    X20BM11,

    // CONNECTION_BLOCKS
    X20TB12,

    // CPUS
    X20CP0484_1,

    // POWERS
    X20PS9600,

    // EXTENSION_CARDS
    X20BC0083,

    NONE = 128,
};

enum class ConnectorType {
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

namespace HwTool {

    namespace V2 {
        //TODO: better alternative to enums.
        enum class IoCardType {
            ERROR = 0,

            X20DO9322 = 1,
            X20DI9372,
            X20AI4622,
            X20AI4632,
        };

        enum class BusModuleType{
            ERROR = 0,

            X20BX0083,
        };

        enum class BaseType{
            ERROR = 0,

            X20BM11,
            X20BB52,
            X20BB80,
        };

        enum class CpuType{
            ERROR,

            X20CP0484_1,
        };
        struct ModuleIO;
        struct ModuleBUS;
        struct ModuleCPU;
        using ModuleVariant = std::variant<ModuleIO, ModuleBUS,
                                           ModuleCPU>;

        using BaseMap = std::unordered_map<std::string, BaseType>;

        using ModuleMap = std::unordered_map<std::string, ModuleVariant>;

        template<typename T>
        std::optional<T> getNextTyped(const T& module) {
            static_assert(
                std::is_same_v<T, ModuleIO> ||
                std::is_same_v<T, ModuleBUS> ||
                std::is_same_v<T, ModuleCPU>,
                "T must be one of the module types"
            );
            
            if (module.next.has_value()) {
                if (auto nextPtr = std::get_if<T>(&*module.next)) {
                    return *nextPtr;
                }
            }
            return std::nullopt;
        }
        struct Connector {
            std::string name;
            std::unordered_map<std::string, std::string> parameters;
        };
       

        struct ModuleCPU {
            std::string name;
            CpuType type;
            std::string version;
            std::string base;
            V2::Connector connector;
            std::unordered_map<std::string, std::string> parameters;
            std::string group;
            ModuleVariant* next;
            ModuleCPU() = default;
            ModuleCPU(std::string n, CpuType t, std::string v, std::string b,
                V2::Connector conn = {}, std::unordered_map<std::string, std::string> params = {},
                std::string g = {}, ModuleVariant* nxt = nullptr)
            : name(std::move(n)),
            type(t),
            version(std::move(v)),
            base(std::move(b)),
            connector(std::move(conn)),
            parameters(std::move(params)),
            group(std::move(g)),
            next(std::move(nxt)) {}
        };


        // TODO: add also Powersupply? maybe own struct?
        struct ModuleBUS {
            std::string name;
            BusModuleType type;
            std::string version;
            std::string base;
            int nodeNumber;
            ModuleVariant* previous;
            ModuleVariant* next;
            ModuleBUS() = default;
            ModuleBUS(std::string n, BusModuleType t, std::string v, std::string b, int nodeNum,
                    ModuleVariant* prev = nullptr, ModuleVariant* nxt = nullptr)
                : name(std::move(n)),
                type(t),
                version(std::move(v)),
                base(std::move(b)),
                nodeNumber(nodeNum),
                previous(std::move(prev)),
                next(std::move(nxt)) {}
        };

        struct ModuleIO {
            std::string name;
            IoCardType type;
            std::string version;
            std::string tb;
            std::string base;
            ModuleVariant* previous;
            ModuleVariant* next;
            ModuleIO() = default;
            ModuleIO(std::string n, IoCardType t, std::string v, std::string tb_, std::string base_, 
                     ModuleVariant* prev = nullptr,
                     ModuleVariant* nxt = nullptr)
                : name(std::move(n)),
                  type(t),
                  version(std::move(v)),
                  tb(std::move(tb_)),
                  base(std::move(base_)),
                  previous(std::move(prev)),
                  next(std::move(nxt)) {}
        };

        namespace utils {}

    }  // namespace V2

    struct Module;

    struct Connection {
        ConnectorType connector;
        std::string targetModuleName;
        Module* targetModule;
        ConnectorType targetConnector;

        Connection() = default;
        Connection(ConnectorType conn, ConnectorType targetConn, Module* targetMod = nullptr,
                   std::string targetName = "")
            : connector(conn),
              targetModuleName(std::move(targetName)),
              targetModule(targetMod),
              targetConnector(targetConn) {}
    };

    struct Parameter {
        std::string id;
        std::string value;
    };

    struct Group {
        std::string id;
    };

    struct Connector {
        std::string name;
        std::vector<Parameter> parameters;
    };

    struct Module {
        std::string name;
        cardType type;
        std::string version;
        std::vector<Connection> connections;
        std::vector<Parameter> parameters;
        Connector connector;
        Group group;

        Module() = default;
        Module(std::string n, cardType t, std::string v, std::vector<Connection> conns = {},
               std::vector<Parameter> params = {}, Group g = {})
            : name(std::move(n)),
              type(t),
              version(std::move(v)),
              connections(std::move(conns)),
              parameters(std::move(params)),
              group(std::move(g)) {}
    };

    struct ModulePack {
        Module card, base, tb;
        bool empty() const { return card.name.empty() && base.name.empty() && tb.name.empty(); }
    };

    inline std::ostream& operator<<(std::ostream& os, const Connection& c) {
        os << "Connection(connector=" << "[" << static_cast<int>(c.connector)
           << "]:" << magic_enum::enum_name(c.connector)
           << ", targetModule=" << (c.targetModule ? c.targetModule->name : "nullptr")
           << ", targetConnector=" << "[" << static_cast<int>(c.targetConnector)
           << "]:" << magic_enum::enum_name(c.targetConnector) << ")";
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
            if (i + 1 < c.parameters.size())
                os << ", ";
        }
        os << "])";
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Module& m) {
        os << "Module(name=" << m.name << ", type=[" << static_cast<int>(m.type)
           << "]:" << magic_enum::enum_name(m.type) << ", version=" << m.version
           << ", connections=[";
        for (size_t i = 0; i < m.connections.size(); ++i) {
            os << m.connections[i];
            if (i + 1 < m.connections.size())
                os << ", ";
        }
        os << "], parameters=[";
        for (size_t i = 0; i < m.parameters.size(); ++i) {
            os << m.parameters[i];
            if (i + 1 < m.parameters.size())
                os << ", ";
        }
        os << "], group=" << m.group;
        os << ")";
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os,
                                    const std::unordered_map<std::string, Module>& modules) {
        os << "{\n";
        size_t count = 0;
        for (const auto& [key, value] : modules) {
            os << "  \"" << key << "\": " << value;
            if (++count < modules.size())
                os << ",";
            os << "\n";
        }
        os << "}";
        return os;
    }

    namespace utils {

        constexpr bool isIoCard(cardType t) {
            return t == cardType::X20DO9322 || t == cardType::X20DI9372 ||
                   t == cardType::X20AI4622 || t == cardType::X20AI4632;
        }

        constexpr bool isBase(cardType t) {
            return t == cardType::X20BB52 || t == cardType::X20BB80 || t == cardType::X20BM11;
        }

        constexpr bool isCpu(cardType t) {
            return t == cardType::X20CP0484_1;
        }
        constexpr bool isIoCard(const Module& m) {
            return isIoCard(m.type);
        }
        constexpr bool isBase(const Module& m) {
            return isBase(m.type);
        }
        constexpr bool isCpu(const Module& m) {
            return isCpu(m.type);
        }
        constexpr std::string getBase(const Module& m) {
            if (isIoCard(m)) {
                assert(m.connections.size() == 2 &&
                       "Invalid ioCard connections. should have only 2.");
                return m.connections[1].targetModuleName;
            } else if (isBase(m)) {
                return m.name;
            } else if (isCpu(m)) {
                assert(false && "not implemented");
            }
            return std::string();
        }
        constexpr std::vector<std::string> getTargets(const Module& m) {
            std::vector<std::string> res;
            if (isIoCard(m)) {
                for (auto&& con : m.connections) {
                    res.push_back(con.targetModuleName);
                }

            } else if (isBase(m)) {
                assert(m.connections.size() == 1 &&
                       "Not valid module. If card is base it should have 1 connection");
                res.push_back(m.connections[0].targetModuleName);
            } else if (isCpu(m)) {
                assert(false && "not implemented");
            }

            return res;
        };

    }  // namespace utils

}  // namespace HwTool
