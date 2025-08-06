#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "IExample.h"

namespace Mir {

struct ExampleManager {
    std::vector<std::unique_ptr<IExample>> examples;
    size_t currentIndex = 0;
    IExample* currentExample = nullptr;


    void addExample(std::unique_ptr<IExample> example) {
        examples.push_back(std::move(example));
        if (examples.size() == 1) {
            selectExample(0);
        }
    }
    
    void selectExample(size_t index) {
        if (index < examples.size()) {
            // Clean up previous example
            if (currentExample) {
                currentExample->cleanup();
            }
            
            // Set new example and set it up
            currentIndex = index;
            currentExample = examples[index].get();
            currentExample->setup();
        }
    }
    
    // Helper methods
    size_t count() const { return examples.size(); }
    const char* getCurrentName() const { 
        return currentExample ? currentExample->getName() : "None"; 
    }
};

} // namespace Mir