#include <fstream>

#include "helpers.hpp"
#include "state.hpp"

// Helper function to tokenize a string
namespace helpers {
    void file(State* state, std::string filename) {
        std::string line;
        std::ifstream input(filename);

        if (!input.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        int lineNumber = 1;

        while (std::getline(input, line)) {
            try {
                tokens_t tokens = helpers::tokenize(line);
                state->eval(&tokens);

            } catch (const std::runtime_error& e) { // Handle error
                throw std::runtime_error(
                        filename + ":" + std::to_string(lineNumber) + ": " + e.what()
                        );
            }

            lineNumber++;
        }
    }
    tokens_t tokenize(std::string input) {
        tokens_t tokens;
        std::string current;

        bool in_quotes = false;
        char quote_char = '\0';

        for (char c : input) {

            // Character in quotes
            if (c == '"' || c == '\'') {

                if (!in_quotes) {
                    in_quotes = true;
                    quote_char = c;

                } else if (c == quote_char) {
                    in_quotes = false;
                    quote_char = '\0';

                } 

                current += c;

                // Blank character/space/tab (not in quotes)
            } else if ((c == ' ' || c == '\t') && !in_quotes) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }

                // Character not in quotes
            } else {
                current += c;
            }
        }

        // Throw error if quotes not closed
        if (in_quotes) {
            throw std::runtime_error("Unclosed quote in input");
        }

        if (!current.empty()) {
            tokens.push_back(current);
        }

        return tokens;
    }
}
