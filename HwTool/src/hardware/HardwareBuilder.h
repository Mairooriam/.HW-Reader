#pragma once
#include "types.h"
class HardwareBuilder
{
private:

public:
HardwareBuilder(/* args */);
~HardwareBuilder();
Module X20BM11(const std::string& name, const std::string& version = "1.1.0.0",
    Module* targetModule = nullptr, const std::string& targetModuleName = "");
    Module X20TB12(const std::string& name);
    
    // version will cause problems in Br
    std::vector<Module> IOCARD(const std::string& name, cardType type, const std::string& targetTB12, const std::string& version = "1.0.0.0");
    Module IOCARD2(const std::string& name, cardType type, const std::string& targetTB12, const std::string& targetBM11, const std::string& version = "1.0.0.0");
};


