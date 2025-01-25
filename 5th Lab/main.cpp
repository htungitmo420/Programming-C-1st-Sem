#include "Cmd.h"
#include <iostream>
#include <string>

int main() {
    CommandProcessor commandProcessor;
    std::string input;
    while (std::getline(std::cin, input)) {
        if (!input.empty()) {
            commandProcessor.ProcessCommand(input);
        }
    }
    
    return 0;
}