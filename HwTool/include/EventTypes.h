// EventTypes.h
#pragma once
#include <string>
#include "types.h"
namespace Event{
    struct ImportHW {
        std::string filename;
    };
    
    struct ExportHW {
        std::string filename;
    };
    
    struct ExportMermaid {
        std::string filename;
    };
    
    struct CreateCard {
        std::string name;
        HwTool::cardType type;
        std::string target;
    };
    
    struct Undo {
    };
    
    struct ImportCSV {
        std::string filename;
        std::string target;
    };

}