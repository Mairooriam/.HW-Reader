
#pragma once
#include <unordered_map>
#include "Types.h"

namespace HwTool {
    class IRenderer {
    public:
        virtual ~IRenderer() = default;
        virtual void render(const ModuleMap& modules) = 0;
    };
}