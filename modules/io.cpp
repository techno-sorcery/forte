#include "../interpreter/state.hpp"
#include "../interpreter/helpers.hpp"
#include <iostream>
#include <algorithm>

/*
   This module contains functions that deal with basic I/O - think printing
   strings, reading keyboard input (WIP), and printing primitive values.
   */
namespace forte::modules::io {
    template <typename T>
        void out(State* state) {
            val_t a = state->pop<val_t>();

            std::cout << helpers::cast<T>(a);
        }

    // IO
    void in(State* state) {
        char_t c;

        if (!std::cin.get(c)) {
            throw std::runtime_error("Character not read");
        }

        state->push(c);
    }


    void outChar(State* state) {
        out<char_t>(state);
    }

    void outNum(State* state) {
        out<num_t>(state);
    }

    void outPtr(State* state) {
        out<ptr_t>(state);
    }

    void outWord(State* state) {
        out<word_t>(state);
    }

    void print(State* state) {
        char_t c;

        do {
            val_t val = state->pop<val_t>();
            c = helpers::cast<char_t>(val);

            if (c != '\0') {
                std::cout << c;
            }

        } while (c != '\0');
    }

    void newline(State* state) {
        std::cout << '\n';
    }

    void tab(State* state) {
        std::cout << '\t';
    }

    void concat(State* state) {
        std::string a;
        std::string b;

        char_t c;

        do {
            val_t val = state->pop<val_t>();
            c = helpers::cast<char_t>(val);

            if (c != '\0') {
                a += c;
            }

        } while (c != '\0');

        do {
            val_t val = state->pop<val_t>();
            c = helpers::cast<char_t>(val);

            if (c != '\0') {
                b += c;
            }

        } while (c != '\0');

        std::string ab = a + b;
        std::reverse(ab.begin(), ab.end());

        state->push('\0');

        for (char chr : ab) {
            state->push(chr);
        }
    }

    void printLine(State* state) {
        print(state);
        std::cout << '\n';
    }

    void flush(State* state) {
        (void)state;
        std::cout << std::flush;
    }

}

extern "C" void forte_init_module(forte::State* state) {
    using namespace forte::modules::io;

    state->newEntry("in", in);
    state->newEntry(".#", outNum);
    state->newEntry(".@", outChar);
    state->newEntry(".$", outPtr);
    state->newEntry(".%", outWord);
    state->newEntry("print", print);
    state->newEntry("println", printLine);
    state->newEntry("\\n", newline);
    state->newEntry("\\t", tab);
    state->newEntry("cat", concat);
    state->newEntry("flush", flush);
}
