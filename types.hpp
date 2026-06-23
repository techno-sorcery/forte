#pragma once

#include <cstddef>
#include <variant>
#include <stack>
#include <string>
#include <map>
#include <vector>

class State;


// Value types
using num_t = double; // Numeric type
using ptr_t = std::size_t; // Pointer type
using char_t = char; // Character type
using word_t = unsigned long long; // Raw (untyped) type
using val_t = std::variant<std::monostate, ptr_t, num_t, word_t, char_t>; // Generic value type

using tokens_t = std::vector<std::string>; // Token vector (defined functions)
using stack_t = std::stack<val_t>; // Return stack
using data_t = std::vector<val_t>; // Data vector

// Function types
using funct_t = void(*)(State*); // Primitive postfix functions
using prefix_t = bool(*)(State*, std::string); // Primitive prefix functions

// Symtable types
/* 
   Each entry holds a primitive function pointer, prefix function pointer,
   primitive value, array of primitive values, or array of tokens (user defined
   function)
*/
using entry_t = std::variant<funct_t, prefix_t, ptr_t, tokens_t>;
using symtable_t = std::map<std::string, entry_t>; // Symtable map

// Struct for holding state class context
typedef struct StateContext {
    stack_t* stack = NULL;
    data_t* data = NULL;
    State* global = NULL;
} StateContext;
