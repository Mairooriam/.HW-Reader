#pragma once
#include <memory>
#include <vector>

namespace HwTool{

    class Hw;

    class ICommand {
    public:
        virtual ~ICommand() = default;
        virtual void execute(Hw& hw) = 0;
        virtual void undo(Hw& hw) = 0; 
    };



    class CommandManager {
        Hw& hw;
        std::vector<std::unique_ptr<ICommand>> history;
        int current = -1;
    public:
        CommandManager(Hw& hwRef) : hw(hwRef) {}

        void execute(std::unique_ptr<ICommand> cmd) {
            cmd->execute(hw);
            history.resize(current + 1);
            history.push_back(std::move(cmd));
            ++current;
        }

        void undo() {
            if (current >= 0) {
                history[current]->undo(hw);
                --current;
            }
        }

        void redo() {
            if (current + 1 < history.size()) {
                ++current;
                history[current]->execute(hw);
            }
        }
    };
}