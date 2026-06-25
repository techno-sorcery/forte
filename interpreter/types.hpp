#pragma once

#include <any>
#include <cstddef>
#include <memory>
#include <unordered_map>
#include <variant>
#include <stack>
#include <string>
#include <vector>


/*
   This namespace contains a bunch of definitions for types which are used
   throughout the codebase.
   */
namespace forte {
    class State;
    class Runtime;
    class Scope;

    using runtime_t = std::shared_ptr<Runtime>;
    using scope_t = std::shared_ptr<Scope>;

    // Value types
    using num_t = double; // Numeric type
    using ptr_t = std::size_t; // Pointer type
    using char_t = char; // Character type
    using word_t = unsigned long long; // Raw (untyped) type
    using val_t = std::variant<num_t, ptr_t, word_t, char_t>; // Generic value type

    using label_t = std::string;

    using tokens_t = std::vector<label_t>; // Token vector (defined functions)
    using stack_t = std::stack<val_t>; // Return stack
    using data_t = std::vector<val_t>; // Data vector

    // Function types
    using funct_t = void(*)(State*); // Primitive postfix functions
    using prefix_t = bool(*)(State*, label_t); // Primitive prefix functions

    // Symtable types
    /* 
       Each entry holds either a primitive function pointer, prefix function 
       pointer, primitive value, or token vector (user defined function)
       */
    using entry_t = std::variant<funct_t, prefix_t, ptr_t, tokens_t>;
    using symtable_t = std::unordered_map<label_t, entry_t>; // Symtable map

    // Struct for holding state class context
    struct PrefixContext {
        prefix_t prefix = NULL;
        std::any data;
    };

    struct TokenWrapper {
        tokens_t* tokens;
        int* depth;
    };
}
