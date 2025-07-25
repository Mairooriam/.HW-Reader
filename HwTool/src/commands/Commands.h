#pragma once
#include <string>
#include "Types.h"
#include "ICommand.h"

namespace HwTool{

    class CreateCardCommand : public ICommand {
        std::string name;
        cardType type;
    public:
        CreateCardCommand(const std::string& n, cardType t) : name(n), type(t) {}
        void execute() override {  }
        void undo() override { /* implement undo logic */ }
    };

}