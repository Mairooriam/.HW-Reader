#include <gtest/gtest.h>
#include "HwTool.h"
#include <string>
#include <fstream>
#include <tinyxml2.h>
#include "Types.h"
#include "DataAccess/ModuleXmlImporter.h"
#include <iostream>
using namespace HwTool;

TEST(BasicTestV2, Sanity) {
    EXPECT_TRUE(true);
    EXPECT_EQ(2 + 2, 4);
}

TEST(TypesV2, creation){
 // auto io = std::make_shared<V2::IO_Module>("name", V2::IoCardType::X20DO9322, "1.0", "tb", "base");
  //auto io2 = std::make_shared<V2::IO_Module>("name2", V2::IoCardType::X20DI9372, "1.1", "tb2", "base2", io);
}
