
#pragma once
#include <unordered_map>
#include "Types.h"

namespace HwTool {
    class IRenderer {
    public:
        virtual ~IRenderer() = default;
        virtual void render(const std::unordered_map<std::string, Module>& modules) = 0;
    };
}