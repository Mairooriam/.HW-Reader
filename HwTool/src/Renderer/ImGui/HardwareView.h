#pragma once
#include "HwTool.h"
#include <string>
#include "RenderData.h"
namespace Mir::HardwareView {
    void render(const HwTool::RenderData& hw, std::string& selectedCard);
}