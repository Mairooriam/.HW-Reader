
#pragma once

namespace HwTool { struct RenderData; }

namespace HwTool {
    class IRenderer {
    public:
        virtual ~IRenderer() = default;
        virtual void render(const HwTool::RenderData& modules) = 0;
    };
}