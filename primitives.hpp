#pragma once

#include "state.hpp"

namespace primitives {

    // Arithmetic
    void add(State* state);
    void sub(State* state);
    void mult(State* state);
    void div(State* state);
    
    // Bitwise
    void band(State* state);
    void bor(State* state);
    void bxor(State* state);

    // Comparison
    void land(State* state);
    void lor(State* state);
    void gt(State* state);
    void lt(State* state);
    void eq(State* state);

    // Control Flow
    void exit(State* state);

    // Stack manipulation
    void drop(State* state);
    void dup(State* state);
    void swap(State* state);

    // IO
    void out(State* state); // Print char
    void in(State* state); // Get char from keyboard

    // Data
    void toNum(State* state);
    void toPtr(State* state);

    void set(State* state);
    void get(State* state);

    void setArr(State* state);
    void getArr(State* state);
}
