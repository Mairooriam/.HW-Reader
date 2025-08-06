#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "shader.h"

namespace Mir {

class IExample {
public:
    virtual ~IExample() = default;
    
    virtual void setup() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
    
    //virtual void processInput(GLFWwindow* window) { }
    
    virtual const char* getName() const = 0;

    virtual void renderUI() { }
    protected:
        GLFWwindow* m_window = nullptr;  // Store window handle
};

} // namespace Mir