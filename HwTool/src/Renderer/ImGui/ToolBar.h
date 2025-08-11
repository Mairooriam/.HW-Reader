#pragma once
#include "EventDispatcher.h"
#include <string>

namespace Mir::ToolBar {
    void render(EventDispatcher* dispatcher, const std::string& selectedCard);
}