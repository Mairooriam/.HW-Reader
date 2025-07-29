#include "HardwareBuilder.h"
namespace HwTool{
    HardwareBuilder::HardwareBuilder()
    {
    }
    namespace V2 {
        std::shared_ptr<V2::ModuleIO> V2::HardwareBuilder::createModuleIoCard(
            const std::string &name, IoCardType type, const std::string &version,
            const std::string &tbName, const std::string &bmName) {
            return std::make_shared<ModuleIO>(name, type, version, tbName, bmName);
        }
        HardwareBuilder::HardwareBuilder(){}
        HardwareBuilder::~HardwareBuilder(){}
        // std::shared_ptr<ModuleCPU> HardwareBuilder::createModuleCpu(const std::string &name,
        //                                                                 cardType type,
        //                                                                 const std::string &version,
        //                                                                 const std::string &tbName,
        //                                                                 const std::string &bmName) {
        //     return std::shared_ptr<ModuleCPU>(name, type, version, tbName, bmName);
        // }

        // std::shared_ptr<:ModuleBUS> HardwareBuilder::createModuleBus(const std::string &name,
        //                                                                 cardType type,
        //                                                                 const std::string &version,
        //                                                                 const std::string &tbName,
        //                                                                 const std::string &bmName) {
        //     return std::make_shared<ModuleBUS>(name, type, version, tbName, bmName);
        // }

    }  // namespace V2

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
    


}    
