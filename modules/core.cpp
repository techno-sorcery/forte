#include <cstdlib>
#include <stdexcept>
#include <functional>

#include "../interpreter/state.hpp"
#include "../interpreter/helpers.hpp"
#include "../interpreter/types.hpp"

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

extern "C" void forte_init_module(State* state) {
     // Arithmetic
    state->addFunct("+", add);
    state->addFunct("-", sub);
    state->addFunct("*", mult);
    state->addFunct("/", div);

    // Bitwise
    state->addFunct("&", band);
    state->addFunct("|", bor);
    state->addFunct("^", bxor);
    state->addFunct("~", bnot);

    // Comparison / logic
    state->addFunct("&&", land);
    state->addFunct("||", lor);
    state->addFunct(">",  gt);
    state->addFunct("<",  lt);
    state->addFunct("=",  eq);
    state->addFunct("!",  lnot);

    // Control
    state->addFunct("exit", exit);

    // Stack manipulation
    state->addFunct("drop", drop);
    state->addFunct("dup",  dup);
    state->addFunct("<>",   swap);

    // Data
    state->addFunct("<-", set);
    state->addFunct("->", get);
}
