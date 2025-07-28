#include <iostream>
#include <HwTool.h>
#include <format>
#include "Types.h"
#include <memory>
using namespace HwTool;
int main(int argc, char const *argv[])
{
        using namespace HwTool::V2;

        ModuleMap moduleCache;

        // Create modules
        auto io = std::make_shared<ModuleIO>("io1", IoCardType::X20DO9322, "1.0", "tb1", "base1");
        auto io2 = std::make_shared<ModuleIO>("io2", IoCardType::X20DO9322, "1.0", "tb2", "base2",io);
        io->next = io2;
        auto bus = std::make_shared<ModuleBUS>(/* ... */);
        auto cpu = std::make_shared<ModuleCPU>(/* ... */);

        // Store in cache
        moduleCache[io->name] = io;
        moduleCache["bus1"] = bus;
        moduleCache["cpu1"] = cpu;

        // Access and use
        for (const auto& [name, variant] : moduleCache) {
            std::cout << "Module name: " << name << " | ";
            std::visit([](auto&& ptr) {
                using T = std::decay_t<decltype(ptr)>;
                if constexpr (std::is_same_v<T, std::shared_ptr<ModuleIO>>) {
                    std::cout << "Type: IO, version: " << ptr->version << "\n";
                } else if constexpr (std::is_same_v<T, std::shared_ptr<ModuleBUS>>) {
                    std::cout << "Type: BUS\n";
                } else if constexpr (std::is_same_v<T, std::shared_ptr<ModuleCPU>>) {
                    std::cout << "Type: CPU\n";
                }
        }, variant);
    }
    
    return 0;
}
