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
    for (const auto& card : validCards) {
        hw.importHW("hardware.hw");
        std::cout << "Linking to: " << card << std::endl;
        hw.LinkToTarget(card);
        // Optionally, export after each link to see the result
        hw.exportHW("after_linking_" + card + ".hw");
        hw.exportMermaid("after_linking_" + card + ".md");
    }

    //hw.render();
    

    return 0;
}
