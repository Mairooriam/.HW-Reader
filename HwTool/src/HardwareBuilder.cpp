#include "HardwareBuilder.h"
#include "hwUtils.h"
HardwareBuilder::HardwareBuilder()
{
}

HardwareBuilder::~HardwareBuilder() {}

Module HardwareBuilder::X20BM11(const std::string &name, const std::string &version,
                                Module *targetModule, const std::string& targetModuleName)
{
    std::string resolvedTargetName = targetModuleName;
    if (resolvedTargetName.empty() && targetModule) {
        resolvedTargetName = targetModule->name;
    }
    return Module(name, cardType::X20BM11, version, {
        Connection(ConnectorType::X2X1,
                ConnectorType::X2X2,
                targetModule,
                resolvedTargetName
                )
    });
}

Module HardwareBuilder::X20TB12(const std::string &name)
{
    return Module(name, cardType::X20TB12, "1.0.0.0", {});
}

Module HardwareBuilder::IOCARD2(const std::string &name, cardType type, const std::string& targetTB12, const std::string& targetBM11, const std::string &version)
{
    Connection con1(ConnectorType::SS1, ConnectorType::SS, nullptr, targetTB12);
    Connection con2(ConnectorType::SL, ConnectorType::SL1, nullptr, targetBM11);
    Module m(name, type, version, std::vector<Connection>{con1,con2});
    return m;
}



ModulePack HardwareBuilder::IOCARD(const std::string &name, cardType type, const std::string& targetName, const std::string &version)
{
    Module base = X20BM11("X20BM11_" + name, "1.1.0.0", nullptr, targetName);
    Module tb = X20TB12("X20TB12_" + name);

    Connection con1(ConnectorType::SS1, ConnectorType::SS, &tb, tb.name);
    Connection con2(ConnectorType::SL, ConnectorType::SL1, &base, base.name);

    Module card(name, type, version, std::vector<Connection>{con1, con2});

    return ModulePack{card, base, tb};
}
ModulePack HardwareBuilder::createCard(const std::string& name, cardType type, const std::set<std::string>& currentModules){
    std::string nextX20BM11 = incrementStr("X20BM11", currentModules);
    Module newBM11 = X20BM11(nextX20BM11, "1.1.0.0", nullptr, std::string());
    Module newTB12 = X20TB12(incrementStr("X20TB12", currentModules));
    Module newIO = IOCARD2(name, cardType::X20AI4622, newTB12.name, newBM11.name);
    return ModulePack{newIO, newBM11, newTB12};
}
