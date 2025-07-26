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
        hw.deleteCardInternal(m_newCard);
    }

    deleteCardCommand::deleteCardCommand(const std::string& target): m_targetModule(target) {}

    void deleteCardCommand::execute(Hw& hw) {
        hw.deleteCardInternal(m_targetModule);
    }

    void deleteCardCommand::undo(Hw& hw) {}

}  // namespace HwTool