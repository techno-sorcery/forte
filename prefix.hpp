#pragma once

#include <string>
#include "types.hpp"

typedef struct PrefixContext {
    prefix_t prefix = NULL;
    tokens_t* tokens = NULL;
    int loopIndex = 0;
    int tokenDepth = 0;

} PrefixContext;

#include "state.hpp"

// Built-in prefix directives
namespace prefix {
    bool import(State* state, std::string token); // Define a function
    bool var(State* state, std::string token); // Declare number
    bool def(State* state, std::string token); // Define a function

    // Control flow
    bool loop(State* state, std::string token); // Loop 
}
