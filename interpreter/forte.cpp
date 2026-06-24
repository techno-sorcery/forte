#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

#include "helpers.hpp"
#include "types.hpp"
#include "state.hpp"
#include "loader.hpp"

#define INTERACTIVE_STRING "Forte Interpreter v1.0"
#define INTERACTIVE_PROMPT "> "

void interactive();
void file();

void interactive() {
    std::string line;
    forte::State state;

    std::cout << INTERACTIVE_STRING << "\n" << INTERACTIVE_PROMPT;

    // Loop until ctrl+c
    while (std::getline(std::cin, line)) {
        try {
            forte::tokens_t tokens = forte::helpers::tokenize(line);
            state.eval(&tokens);

        } catch (const std::runtime_error& e) { // Handle error
            std::cerr << "Error: " << e.what() << std::endl;
        }

        printf("%s", INTERACTIVE_PROMPT);
    }
}


// Main program function
int main(int argc, char *argv[]) {
    try {
        if (argc == 1) {
            interactive();

        } else if (argc == 2) {
            forte::State state;
            forte::loader::loadFile(&state, argv[1]);

        } else {
            std::cerr << "Usage: " << argv[0] << " [file]" << std::endl;
            return 1;
        }

    } catch (const std::runtime_error& e) { // Handle error
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
