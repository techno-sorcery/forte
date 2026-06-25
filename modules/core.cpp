#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include "../interpreter/state.hpp"
#include "../interpreter/helpers.hpp"
#include "../interpreter/types.hpp"

/*
   This module contains functions that are a core part of the Forte language. 
   In other words, you must implement these if you want your interpreter/
   compiler/whatever to be compatible with the base language.
   */
namespace forte::modules::core {
    template <typename A, typename B = A, typename Out = A, typename Op>
        void arithmetic(State* state, Op op) {
            val_t b = state->pop<val_t>();
            val_t a;

            try {
                a = state->pop<val_t>();
            } catch (const std::runtime_error&) {
                state->push(b);
                throw;
            }

            A av = helpers::cast<A>(a);
            B bv = helpers::cast<B>(b);

            state->push(static_cast<Out>(op(av, bv)));
        }

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

    void divide(State* state) {
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

        state->push(static_cast<word_t>(~word));
    }

    void shl(State* state) {
        arithmetic<word_t, num_t, word_t>(state, [](word_t a, num_t b) {
                if (b < 0) {
                throw std::runtime_error("Negative shift");
                }

                auto word = static_cast<std::uint64_t>(a);
                auto shift = static_cast<unsigned int>(b);

                if (shift >= 64) {
                throw std::runtime_error("Shift too large");
                }

                return static_cast<word_t>(word << shift);
                });
    }

    void shr(State* state) {
        arithmetic<word_t, num_t, word_t>(state, [](word_t a, num_t b) {
                if (b < 0) {
                throw std::runtime_error("Negative shift");
                }

                auto word = static_cast<std::uint64_t>(a);
                auto shift = static_cast<unsigned int>(b);

                if (shift >= 64) {
                throw std::runtime_error("Shift too large");
                }

                return static_cast<word_t>(word >> shift);
                });
    }

    // Comparison / Logic

    void land(State* state) {
        arithmetic<word_t, word_t, word_t>(state, [](word_t a, word_t b) {
                return static_cast<word_t>(a && b);
                });
    }

    void lor(State* state) {
        arithmetic<word_t, word_t, word_t>(state, [](word_t a, word_t b) {
                return static_cast<word_t>(a || b);
                });
    }

    void gt(State* state) {
        arithmetic<num_t, num_t, word_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a > b);
                });
    }

    void lt(State* state) {
        arithmetic<num_t, num_t, word_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a < b);
                });
    }

    void eq(State* state) {
        arithmetic<num_t, num_t, word_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a == b);
                });
    }

    void neq(State* state) {
        arithmetic<num_t, num_t, word_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a != b);
                });
    }

    void lte(State* state) {
        arithmetic<num_t, num_t, word_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a <= b);
                });
    }

    void gte(State* state) {
        arithmetic<num_t, num_t, word_t>(state, [](num_t a, num_t b) {
                return static_cast<word_t>(a >= b);
                });
    }

    void lnot(State* state) {
        word_t word = helpers::cast<word_t>(state->pop<val_t>());

        state->push(static_cast<word_t>(word == 0));
    }

    // Control

    void exitProgram(State* state) {
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

    void over(State* state) {
        val_t b = state->pop<val_t>();
        val_t a;

        try {
            a = state->pop<val_t>();
        } catch (const std::runtime_error&) {
            state->push(b);
            throw;
        }

        state->push(a);
        state->push(b);
        state->push(a);
    }

    void rot(State* state) {
        val_t c = state->pop<val_t>();
        val_t b;
        val_t a;

        try {
            b = state->pop<val_t>();
            a = state->pop<val_t>();
        } catch (const std::runtime_error&) {
            state->push(c);
            throw;
        }

        state->push(b);
        state->push(c);
        state->push(a);
    }

    void nip(State* state) {
        val_t b = state->pop<val_t>();
        val_t a;

        try {
            a = state->pop<val_t>();
        } catch (const std::runtime_error&) {
            state->push(b);
            throw;
        }

        (void)a;

        state->push(b);
    }

    void tuck(State* state) {
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
        state->push(b);
    }

    void twoDrop(State* state) {
        val_t b = state->pop<val_t>();

        try {
            state->pop<val_t>();
        } catch (const std::runtime_error&) {
            state->push(b);
            throw;
        }
    }

    void twoDup(State* state) {
        val_t b = state->pop<val_t>();
        val_t a;

        try {
            a = state->pop<val_t>();
        } catch (const std::runtime_error&) {
            state->push(b);
            throw;
        }

        state->push(a);
        state->push(b);
        state->push(a);
        state->push(b);
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

// Module init
extern "C" void forte_init_module(forte::State* state) {
    using namespace forte::modules::core;
    // Arithmetic
    state->newEntry("+", add);
    state->newEntry("-", sub);
    state->newEntry("*", mult);
    state->newEntry("/", divide);

    // Bitwise
    state->newEntry("&", band);
    state->newEntry("|", bor);
    state->newEntry("^", bxor);
    state->newEntry("~", bnot);
    state->newEntry("<<", shl);
    state->newEntry(">>", shr);

    // Comparison / logic
    state->newEntry("&&", land);
    state->newEntry("||", lor);
    state->newEntry(">", gt);
    state->newEntry("<", lt);
    state->newEntry("=", eq);
    state->newEntry("!=", neq);
    state->newEntry("<=", lte);
    state->newEntry(">=", gte);
    state->newEntry("!", lnot);

    // Control
    state->newEntry("exit", exitProgram);

    // Stack manipulation
    state->newEntry("drop", drop);
    state->newEntry("dup", dup);
    state->newEntry("<>", swap);
    state->newEntry("over", over);
    state->newEntry("rot", rot);
    state->newEntry("nip", nip);
    state->newEntry("tuck", tuck);
    state->newEntry("drop2", twoDrop);
    state->newEntry("dup2", twoDup);

    // Data
    state->newEntry("<-", set);
    state->newEntry("->", get);
}
