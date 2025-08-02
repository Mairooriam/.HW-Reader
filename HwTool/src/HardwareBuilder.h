#pragma once
#include <memory>
#include <set>

#include "Types.h"
#include "hwUtils.h"

namespace HwTool {
    class HardwareBuilder {
    private:
    public:
        HardwareBuilder(/* args */);
        ~HardwareBuilder();
        Module X20BM11(const std::string& name, const std::string& version = "1.1.0.0",
                Module* targetModule = nullptr, const std::string& targetModuleName = "");
        Module X20TB12(const std::string& name);

        // version will cause problems in Br
        ModulePack IOCARD(const std::string& name, cardType type, const std::string& targetTB12,
                const std::string& version = "1.0.0.0");
        Module IOCARD2(const std::string& name, cardType type, const std::string& targetTB12,
                const std::string& targetBM11, const std::string& version = "1.0.0.0");

        template <typename StringContainer>
            requires HasContains<StringContainer, std::string>
        ModulePack createCard(
                const std::string& name, cardType type, const StringContainer& currentModules) {
            std::string nextX20BM11 = incrementStr("X20BM11", currentModules);
            Module newBM11 = X20BM11(nextX20BM11, "1.1.0.0", nullptr, std::string());
            Module newTB12 = X20TB12(incrementStr("X20TB12", currentModules));
            Module newIO = IOCARD2(name, cardType::X20AI4622, newTB12.name, newBM11.name);
            return ModulePack{newIO, newBM11, newTB12};
        }
    };

    namespace V2 {

        class HardwareBuilder {
        private:
            /* data */
        public:
            HardwareBuilder(/* args */);
            ModuleIO createModuleIoCard(const std::string& name,
                    IoCardType type, const std::string& version, const std::string& tbName,
                    const std::string& bmName);
            ModuleCPU createModuleCpu(const std::string& name, CpuType type, const std::string& bmName,
                    const std::string& version = "1.0");
            ModuleBUS createModuleBus(const std::string& name,
                    BusModuleType type, const std::string& version, const std::string& bmName,
                    int nodeID);

            ~HardwareBuilder();
        };

    }  // namespace V2
}  // namespace HwTool
