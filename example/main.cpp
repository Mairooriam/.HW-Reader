#include <iostream>
#include <HwTool.h>
#include <format>
int main(int argc, char const *argv[])
{
    using namespace HwTool;
    std::cout << "Hello World" << "\n";
    HwTool::Hw hw;
    // hw.importHW("hardware.hw");
    // hw.exportHW("testing.hw");
    // hw.exportMermaid("mermaid.md");

    // hw.importHW("hardware.hw");
    // hw.exportMermaid("adding_single_card_before.md");
    // hw.createCard("SingleCard", cardType::X20DI9372);
    // auto validCards3 = hw.getAvailableCards();
    // hw.linkToTarget(validCards3[2]);
    // hw.exportMermaid("adding_single_card_after.md");

    // printf("adding card\n");
    // hw.createCard("AF111", cardType::X20DO9322);
    // hw.importHW("hardware.hw");
    // auto validCards = hw.getAvailableCards();
    // for (const auto& card : validCards) {
    //     hw.importHW("hardware.hw");
    //     std::cout << "Linking to: " << card << std::endl;
    //     hw.linkToTarget(card);
    //     hw.exportHW("after_linking_" + card + ".hw");
    //     hw.exportMermaid("after_linking_" + card + ".md");
    // }
    
    hw.importHW("hardware.hw");
    hw.exportMermaid("before_adding_cards.md");
    std::string previousCard = "";
    for (size_t i = 0; i < 4; i++)
    {
        std::string newCardName = "AF10" + std::to_string(i);
        hw.createCard(newCardName, cardType::X20AI4622);
        if (previousCard.empty()){
            auto validCards2 = hw.getAvailableCards();
            previousCard = validCards2[2];
        }

        hw.linkToTarget(previousCard);
        previousCard = newCardName;
    }
    hw.exportMermaid("after_adding_cards.md");

    hw.undo();
    hw.undo();
    hw.undo();

    hw.exportMermaid("after_undo_cards.md");

    hw.importHW("hardware.hw");
    hw.createCard("AF111", cardType::X20DO9322);
    hw.linkToTarget("AF300");
    hw.undo();
    hw.exportMermaid("testingUndo2.md");

    hw.importHW("hardware.hw");
    std::vector<std::string> validcards5 = hw.getAvailableCards();

    for (size_t i = 0; i < validcards5.size(); ++i) {
        std::cout << std::format("[{:>2}] - {}\n", i, validcards5[i]);
    }
    
    auto modulesCsv = hw.importCSV("HW_version-1.0.0.csv");
    hw.combineToExisting(modulesCsv, validcards5[3]);
    hw.exportMermaid("HW_version-1.0.0.md");
    hw.exportHW("Hw_version1.0.0.0.hw");
    printf("Example done");


    for (size_t i = 0; i < validcards5.size(); ++i) {
        std::cout << std::format("[{:>2}] - {}\n", i, validcards5[i]);
        hw.importHW("hardware.hw");
        auto modulesCsv = hw.importCSV("HW_version-1.0.0.csv");
        hw.combineToExisting(modulesCsv, validcards5[i]);
        hw.exportMermaid("HW_version-1.0.0_" + std::to_string(i) + ".md");
        hw.exportHW("Hw_version1.0.0.0_" + std::to_string(i) + ".hw");
    }
    printf("Example done\n");


    return 0;
}
