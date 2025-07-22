#include <iostream>
#include <HwTool.h>
int main(int argc, char const *argv[])
{
    std::cout << "Hello World" << "\n";
    HwTool::Hw hw;
    hw.render();
    hw.importHW("hardware.hw");
    printf("Import done\n");
    printf("Exporting\n");
    hw.exportHW("testing.hw");
    printf("export done\n");

    printf("exporting to mermaid.md\n");
    hw.exportMermaid("mermaid.md");
    printf("Export complete\n");
    return 0;
}
