#pragma once

#include <string>
#include <variant>
#include "types.hpp"

enum class forState {
    INIT,
    COND,
    UPDATE,
    BODY
};

enum class whileState {
    COND,
    BODY
};

typedef struct RepContext {
    int count = 0;
    tokens_t body;
    int depth = 0;
} RepContext;

typedef struct ForContext {
    forState state = forState::INIT;
    tokens_t init;
    tokens_t cond;
    tokens_t update;
    tokens_t body;
    int depth = 0;
} ForContext;

typedef struct WhileContext {
    whileState state = whileState::COND;
    tokens_t cond;
    tokens_t body;
    int depth = 0;
} WhileContext;

typedef struct DefContext {
    tokens_t* tokens = NULL;
    int depth = 0;
} DefContext;

using PrefixData = std::variant<DefContext, 
      ForContext, 
      WhileContext, 
      RepContext>;

typedef struct PrefixContext {
    prefix_t prefix = NULL;
    PrefixData data;
} PrefixContext;

#include "state.hpp"

// Built-in prefix directives
namespace prefix {
    // Data
    bool cast(State* state, std::string token); // Typecasting 

    // Conditions

    // Loops
    bool forLoop(State* state, std::string token); // For 
    bool whileLoop(State* state, std::string token); // While 
    bool rep(State* state, std::string token); // Repeat 

    // Definitions
    bool import(State* state, std::string token); // Import a file
    bool var(State* state, std::string token); // Declare number
    bool def(State* state, std::string token); // Define a function
}
