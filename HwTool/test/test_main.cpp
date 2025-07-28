#include <gtest/gtest.h>
#include "HwTool.h"
#include <string>
#include <fstream>
#include <tinyxml2.h>
#include "Types.h"
#include "DataAccess/ModuleXmlImporter.h"
#include <iostream>
using namespace HwTool;
TEST(BasicTest, Sanity) {
    EXPECT_TRUE(true);
    EXPECT_EQ(2 + 2, 4);
}
std::string createTestHwFile(const std::string& filename) {
    const char* xml =
        R"(<?xml version="1.0" encoding="utf-8"?>
<?AutomationStudio Version=6.0.2.177 FileVersion="4.9"?>
<Hardware xmlns="http://br-automation.co.at/AS/Hardware">
  <Module Name="X20BM11" Type="X20BM11" Version="1.1.0.0">
    <Connection Connector="X2X1" TargetModule="X20BB52" TargetConnector="IF6" />
  </Module>
  <Module Name="X20TB12a" Type="X20TB12" Version="1.0.0.0" />
  <Module Name="initialcard1" Type="X20DO9322" Version="1.6.2.0">
    <Connection Connector="SS1" TargetModule="X20TB12a" TargetConnector="SS" />
    <Connection Connector="SL" TargetModule="X20BM11" TargetConnector="SL1" />
  </Module>
</Hardware>
)";
    std::ofstream ofs(filename);
    ofs << xml;
    ofs.close();
    return filename;
}
TEST(ModuleXmlImporter, basic){
    std::string filename = "test_temp.hw";
    createTestHwFile(filename);

    ModuleXmlImporter hwimp(filename);
    hwimp.mapModules(); 
    auto modules = hwimp.getModules();

    
    ASSERT_EQ(hwimp.getErrors().size(), 0);
    ASSERT_EQ(modules.size(), 3);
    std::remove(filename.c_str());
}

TEST(TypeUtils, getters){
    std::string filename = "test_temp.hw";
    createTestHwFile(filename);

    ModuleXmlImporter hwimp(filename);
    hwimp.mapModules(); 
    auto modules = hwimp.getModules();
    EXPECT_EQ(utils::getTargets(modules["X20BM11"]).size(), 1);
    EXPECT_EQ(utils::getTargets(modules["X20TB12a"]).size(), 0);
    EXPECT_EQ(utils::getTargets(modules["initialcard1"]).size(), 2);


    EXPECT_EQ(utils::getBase(modules["X20BM11"]), "X20BM11"); // Not sure if i want this to return itself or nothing
    EXPECT_EQ(utils::getBase(modules["X20TB12a"]), "");
    EXPECT_EQ(utils::getBase(modules["initialcard1"]), "X20BM11");

    // test same after invalid data
    //modules["initialcard1"].connections.emplace_back(ConnectorType::IF6,ConnectorType::PS1,nullptr,"Testing");
     //EXPECT_NE(utils::getBase(modules["initialcard1"]), "X20BM11");
   
    //TODO: Test with CPU
    


    std::remove(filename.c_str());
}
