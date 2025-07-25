#pragma once
#include <string>
#include "Types.h"
#include "ICommand.h"


namespace HwTool {
    class LinkToTargetCommand : public ICommand {
        std::string m_targetModule;
        std::string m_newCard;
    public:
        LinkToTargetCommand(const std::string& target);
        void execute(Hw& hw) override;
        void undo(Hw& hw) override;
    };
}