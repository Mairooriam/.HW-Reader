#include "Application.h"
int main(int argc, char const *argv[])
{
    try {
        Application app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}