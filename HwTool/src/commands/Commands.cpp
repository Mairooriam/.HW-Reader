#include "Commands.h"
#include "HwTool.h"

namespace HwTool {
    LinkToTargetCommand::LinkToTargetCommand(const std::string& target) 
        : m_targetModule(target){};

    void LinkToTargetCommand::execute(Hw& hw) {
        m_newCard = hw.m_addCardBuffer.card.name;
        hw.LinkToTargetInternal(m_targetModule);
    }
    void LinkToTargetCommand::undo(Hw& hw) {
        //TODO: needs hw to implement removeCard.
    }
}