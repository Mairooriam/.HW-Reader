#pragma once
#include "Types.h"
#include <set>
#include "hwUtils.h"
namespace HwTool{

    class HardwareBuilder
    {
    private:
    Module X20BM11(const std::string& name, const std::string& version = "1.1.0.0",
        Module* targetModule = nullptr, const std::string& targetModuleName = "");
        Module X20TB12(const std::string& name);
        
        // version will cause problems in Br
        ModulePack IOCARD(const std::string& name, cardType type, const std::string& targetTB12, const std::string& version = "1.0.0.0");
        Module IOCARD2(const std::string& name, cardType type, const std::string& targetTB12, const std::string& targetBM11, const std::string& version = "1.0.0.0");
        Module BusCard(const std::string& name, const std::string& baseName, cardType type, int nodeNumber, const std::string& version);
        
    
    public:
    HardwareBuilder(/* args */);
    ~HardwareBuilder();
        template<typename StringContainer>
        requires HasContains<StringContainer, std::string>
        ModulePack createCard(const std::string& name, cardType type, const StringContainer& currentModules){
            std::string nextX20BM11 = incrementStr("X20BM11", currentModules);
            Module newBM11 = X20BM11(nextX20BM11, "1.1.0.0", nullptr, std::string());
            Module newTB12 = X20TB12(incrementStr("X20TB12", currentModules));
            Module newIO = IOCARD2(name, cardType::X20AI4622, newTB12.name, newBM11.name);
            return ModulePack{newIO, newBM11, newTB12};
        }

        template<typename StringContainer>
        requires HasContains<StringContainer, std::string>
        ModulePack createBC0083(const std::string& name, const StringContainer& currentMdoules);


        ModulePack createBusModule(const std::string& name, cardType type, const std::string& plk1Target, const std::string& plk2Target, const ModuleMap& currentModules){
            Connection con1;
            Connection con2;
            
            auto it = currentModules.find(plk1Target);
            if (it != currentModules.end())
            {
                if(Utils::isCpu(it->second)){
                    con1.connector = ConnectorType::PLK1;
                    con1.targetConnector = ConnectorType::IF3;
                    con1.targetModuleName = plk1Target;
                }else if (Utils::isBus(it->second)){
                    con1.connector = ConnectorType::PLK1;
                    con1.targetConnector = ConnectorType::PLK2; // THis might cause bug. it might not always be PLK2 on the target. GL finding this bug :)
                    con1.targetModuleName = plk1Target;
                }else{
                    assert(false && "Not implemented");
                };
            }
            


            it = currentModules.find(plk2Target);
            if (it != currentModules.end())
            {
                if(Utils::isCpu(it->second)){
                    assert(false && "Shouldnt probably happen. usually CPU is in PLK1?");
                    con2.connector = ConnectorType::PLK2;
                    con2.targetConnector = ConnectorType::IF3;
                    con2.targetModuleName = plk2Target;
                }else if (Utils::isBus(it->second)){
                    con2.connector = ConnectorType::PLK2;
                    con2.targetConnector = ConnectorType::PLK1; // THis might cause bug. it might not always be PLK2 on the target. GL finding this bug :)
                    con2.targetModuleName = plk2Target;
                }else{
                    assert(false && "Not implemented");
                };
            }
            Module base = Module(incrementStr("X20BB80", currentModules), cardType::X20BB80, "1.0.0"); // Make dynamic so it chooses correct base accoriding to cardType
            Module bus = BusCard(name, base.name, type, 1, "2.16.1.0"); // Node number from currentModules available node numbers
            
            if (!con1.targetModuleName.empty())
            {
                bus.connections.push_back(con1);
            }
              if (!con2.targetModuleName.empty())
            {
                bus.connections.push_back(con2);
            }
            return {bus, base};
        };


    };
    

}    
