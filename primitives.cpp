#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <functional>

#include "primitives.hpp"
#include "helpers.hpp"
#include "types.hpp"

namespace {
    template <typename T>
    void arithmetic(State* state, std::function<T(T, T)> op) {
        val_t b = state->pop<val_t>();
        val_t a;

        try {
            a = state->pop<val_t>();

        } catch (const std::runtime_error&) {
            state->push(b);
            throw;
        }

        T av = helpers::cast<T>(a);
        T bv = helpers::cast<T>(b);

        state->push(op(av, bv));
    }

    template <typename T>
    void out(State* state) {
        val_t a = state->pop<val_t>();

        std::cout << helpers::cast<T>(a);
    }
}

namespace primitives {

    // Arithmetic
    void add(State* state) {
        arithmetic<num_t>(state, [](num_t a, num_t b) {
                return a + b;
                });
    }

    void sub(State* state) {
        arithmetic<num_t>(state, [](num_t a, num_t b) {
                return a - b;
                });
    }

    void mult(State* state) {
        arithmetic<num_t>(state, [](num_t a, num_t b) {
                return a * b;
                });
    }

    void div(State* state) {
        arithmetic<num_t>(state, [](num_t a, num_t b) {
                if (b == 0) {
                throw std::runtime_error("Divide-by-zero");
                }

                return a / b;
                });
    }

    // Bitwise
    void band(State* state) {
        arithmetic<word_t>(state, [](word_t a, word_t b) {
                return a & b;
                });
    }

    void bor(State* state) {
        arithmetic<word_t>(state, [](word_t a, word_t b) {
                return a | b;
                });
    }

    void bxor(State* state) {
        arithmetic<word_t>(state, [](word_t a, word_t b) {
                return a ^ b;
                });
    }

    void bnot(State* state) {
        word_t word = helpers::cast<word_t>(state->pop<val_t>());

        state->push(~word);
    }



    // Comparison
    void land(State* state) {
        arithmetic<num_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a && b);
                });
    }

    void lor(State* state) {
        arithmetic<num_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a || b);
                });
    }

    void gt(State* state) {
        arithmetic<num_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a > b);
                });
    }

    void lt(State* state) {
        arithmetic<num_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a < b);
                });
    }

    void eq(State* state) {
        arithmetic<num_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a == b);
                });
    }

    void lnot(State* state) {
        word_t word = helpers::cast<word_t>(state->pop<val_t>());

        state->push(static_cast<word_t>(word == 0));
    }


    // Control Flow

    void exit(State* state) {
        (void)state;
        std::exit(0);
    }


    // Stack manipulation
    void drop(State* state) {
        state->pop<val_t>();
    }

    void dup(State* state) {
        val_t a = state->pop<val_t>();

        state->push(a);
        state->push(a);
    }

    void swap(State* state) {
        val_t b = state->pop<val_t>();
        val_t a;

        try {
            a = state->pop<val_t>();
        } catch (const std::runtime_error&) {
            state->push(b);
            throw;
        }

        state->push(b);
        state->push(a);
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

    // Data
    void set(State* state) {
        ptr_t ptr = state->pop<ptr_t>();
        val_t val;

        try {
            val = state->pop<val_t>();

        } catch (const std::runtime_error&) {
            state->push(ptr);
            throw;
        }

        state->setData(ptr, val);
    }

    void get(State* state) {
        ptr_t ptr = state->pop<ptr_t>();
        val_t val;

        try {
            val = state->getData(ptr);

        } catch (const std::runtime_error&) {
            state->push(ptr);
            throw;
        }

        state->push(val);
    }
}
