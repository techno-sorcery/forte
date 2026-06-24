#include "helpers.hpp"

// Helper function to tokenize a string
namespace forte::helpers {

    tokens_t tokenize(std::string input) {
        tokens_t tokens;
        std::string current;

        bool in_quotes = false;
        bool in_comment = false;
        bool escaping = false;
        char quote_char = '\0';

        auto flushCurrent = [&]() {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        };

        auto escapeChar = [](char c) -> char {
            switch (c) {
                case 'n':  return '\n';
                case 't':  return '\t';
                case '0':  return '\0';
                case 'r':  return '\r';
                case 'b':  return '\b';
                case 'f':  return '\f';
                case 'v':  return '\v';
                case '\\': return '\\';
                case '\'': return '\'';
                case '"':  return '"';
                default:
                           throw std::runtime_error(std::string("Invalid escape sequence: \\") + c);
            }
        };

        for (char c : input) {
            if (in_comment) {
                if (c == ')') {
                    in_comment = false;
                }
                continue;
            }

            if (escaping) {
                current += escapeChar(c);
                escaping = false;
                continue;
            }

            if (in_quotes && c == '\\') {
                escaping = true;
                continue;
            }

            if (c == ';' && !in_quotes) {
                flushCurrent();
                break;
            }

            if (c == '(' && !in_quotes) {
                flushCurrent();
                in_comment = true;
                continue;
            }

            if (c == '"' || c == '\'') {
                if (!in_quotes) {
                    in_quotes = true;
                    quote_char = c;

                } else if (c == quote_char) {
                    in_quotes = false;
                    quote_char = '\0';
                }

                current += c;

            } else if ((c == ' ' || c == '\t') && !in_quotes) {
                flushCurrent();

            } else {
                current += c;
            }
        }

        if (escaping) {
            throw std::runtime_error("Unfinished escape sequence");
        }

        if (in_quotes) {
            throw std::runtime_error("Unclosed quote in input");
        }

        if (in_comment) {
            throw std::runtime_error("Unclosed comment in input");
        }

        flushCurrent();

        return tokens;
    }

}
