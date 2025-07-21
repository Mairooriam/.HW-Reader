#pragma once
#include <unordered_map>
#include <string>
#include "types.h"
#include <iostream>
//TODO: Add better debug print
inline void resolveLinking(std::unordered_map<std::string, Module>& modules) {
    for (auto& [name, module] : modules) {
        for (auto& con : module.connections) {
            if (!con.targetModuleName.empty()) {
                auto it = modules.find(con.targetModuleName);
                if (it != modules.end()) {
                    con.targetModule = &it->second;
                } else {
                    std::cerr << "Warning: Target module '" << con.targetModuleName
                              << "' not found for connection in module '" << name << "'\n";
                    con.targetModule = nullptr;
                }
            }
        }
    }
}