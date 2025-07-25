#include <iostream>
#include <HwTool.h>
int main(int argc, char const *argv[])
{
    std::cout << "Hello World" << "\n";
    HwTool::Hw hw;
    hw.importHW("hardware.hw");
    printf("Import done\n");
    printf("Exporting\n");
    hw.exportHW("testing.hw");
    printf("export done\n");

    printf("exporting to mermaid.md\n");
    hw.exportMermaid("mermaid.md");
    printf("Export complete\n");

    printf("adding card\n");


    
    hw.createCard("AF111", cardType::X20DO9322);
    auto validCards = hw.getAvailableCards();
    int selected = 4;
    hw.LinkToTarget(validCards[selected]);



    hw.exportHW("afterAdding.hw");
    hw.exportMermaid("afterAddingWithlINKING.md");

    //hw.render();
    

    return 0;
}
