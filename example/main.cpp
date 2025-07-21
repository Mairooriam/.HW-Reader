#include <iostream>
#include <HwTool.h>
int main(int argc, char const *argv[])
{
    std::cout << "Hello World" << "\n";
    HwTool::Hw hw;
    hw.render();
    hw.importHW("hardware.hw");

    printf("Example done.");
    return 0;
}
