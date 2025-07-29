#include <iostream>
#include <HwTool.h>
#include <format>
#include "Types.h"
#include <memory>
#include "HardwareBuilder.h"
#include "DataAccess/ModuleXmlExporter.h"
using namespace HwTool;
int main(int argc, char const *argv[])
{
        

        V2::ModuleMap moduleCache;

        // Create modules
        V2::HardwareBuilder hwb;
        // Create 4 IO cards using the builder
        auto io1 = hwb.createModuleIoCard("AF100", V2::IoCardType::X20DO9322, "1.0", "X20TB12_1", "X20BM11_1");
        auto io2 = hwb.createModuleIoCard("AF200", V2::IoCardType::X20DI9372, "1.0", "X20TB12_2", "X20BM11_2");
        auto io3 = hwb.createModuleIoCard("AF300", V2::IoCardType::X20AI4622, "1.0", "X20TB12_3", "X20BM11_3");
        auto io4 = hwb.createModuleIoCard("AF400", V2::IoCardType::X20AI4632, "1.0", "X20TB12_4", "X20BM11_4");

        io1->next = io2;
        io2->previous = io1;
        io2->next = io3;
        io3->previous = io2;
        io3->next = io4;
        io4->previous = io3;

        // Store in cache
        moduleCache[io1->name] = io1;
        moduleCache[io2->name] = io2;
        moduleCache[io3->name] = io3;
        moduleCache[io4->name] = io4;

        V2::ModuleXmlExporter exp;
        exp.serialize(moduleCache, "testing.hw");

        // Access and use
        // for (const auto& [name, variant] : moduleCache) {
        //     std::cout << "Module name: " << name << " | ";
        //     std::visit([](auto&& arg){
        //     using T = std::decay_t<decltype(arg)>;
        //     if constexpr (std::is_same_v<T, V2::ModuleIO>){
        //         std::cout << "ModuleIO" << "\n";
        //     } else {
        //         assert(false && "Not handling all types yet.");
        //     }
        // },variant);
    
    
    return 0;
}
