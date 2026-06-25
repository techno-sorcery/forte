#include "state.hpp"
#include "loader.hpp"
#include "types.hpp"
#include <dlfcn.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

namespace forte {

    // Global
    State::State() 
        : runtime(std::make_shared<Runtime>()), 
        scope(std::make_shared<Scope>()),
        parent(nullptr),
        dataBase(0){

            // Fill symtable with built-in functions
            loader::loadModule(this, "modules/prefix.so");
            loader::loadModule(this, "modules/core.so");
            loader::loadModule(this, "modules/io.so");
        }

    // Local
    State::State(runtime_t parentRuntime, scope_t parentScope, 
            State* parentState, StateMode mode) 
        : runtime(parentRuntime),
        parent(parentState) {

            switch (mode) {
                case StateMode::NewScope:
                    scope = std::make_shared<Scope>(parentScope);
                    break;

                case StateMode::SharedScope:
                    scope = std::move(parentScope);
                    break;
            }

            dataBase = runtime->getDataBase();
        }

    // Destructor
    State::~State() {
        if (parent != nullptr) { // Local scope
            runtime->resizeData(dataBase);
        } 

        prefixContext.data.reset();
    }

    ptr_t State::newEntry(std::string label, int entries) {

        // Allocate new data entries
        ptr_t ptr = runtime->alloc(entries);

        // Set new symtable entry
        scope->addEntry(label, ptr);

        return ptr;
    }

    void State::newEntry(std::string label, funct_t funct) {

        // Set new symtable entry
        scope->addEntry(label, funct);
    }


    // Add module prefix
    void State::newEntry(std::string label, prefix_t prefix, 
            PrefixEntryMode mode) {

        // Set new entry
        scope->addEntry(label, prefix);

        // Add token if block prefix
        if (mode == PrefixEntryMode::AddToken) {
            scope->addBlockPrefix(label);
        }
    }

    // Add used defined function
    tokens_t* State::newEntry(std::string label) {

        tokens_t tokens;
        entry_t* entry = scope->addEntry(label, tokens);

        return std::get_if<tokens_t>(entry);
    }

    // Interpret a line of code
    void State::eval(tokens_t* tokens) {
        // State* global = stateContext.global;

        // Evaluate all tokens
        for (std::string token : *tokens) {

            // Parse token
            if (prefixContext.prefix != nullptr) { // Prefix command

                // Execute and check if finished
                try {

                    // Exit prefix mode if done
                    if (prefixContext.prefix(this, token)) {
                        prefixContext.prefix = nullptr;
                        prefixContext.data.reset();
                    }

                } catch (const std::runtime_error& e) { // Handle error

                    // Exit prefix mode and throw
                    prefixContext.prefix = nullptr;
                    prefixContext.data.reset();
                    throw;
                }

            } else if (scope->inSymtable(token)) { // Check if token in symtable
                entry_t entry = scope->getEntry(token);

                if (std::holds_alternative<funct_t>(entry)) { // Postfix
                    funct_t funct = std::get<funct_t>(entry);

                    // Execute function from symtable
                    funct(this);

                } else if (std::holds_alternative<prefix_t>(entry)) { // Prefix

                    // Enter prefix mode
                    prefixContext.prefix = std::get<prefix_t>(entry);

                    // Execute prefix function from symtable
                    try {
                        prefixContext.prefix(this, "");

                    } catch (const std::runtime_error& e) { // Handle error

                        // Exit prefix mode and throw
                        prefixContext.prefix = nullptr;
                        prefixContext.data.reset();
                        throw;
                    }
                } else if (std::holds_alternative<ptr_t>(entry)) { // Pointer
                    ptr_t ptr = std::get<ptr_t>(entry);

                    // Push pointer to stack
                    runtime->push(ptr);

                } else if (std::holds_alternative<tokens_t>(entry)) { // User funct
                    tokens_t* tokens = &std::get<tokens_t>(entry);

                    // Create new scope
                    State state(runtime, scope, this, StateMode::NewScope);

                    // Evaluate tokens
                    state.eval(tokens);
                }

            } else { // Otherwise, push value

                if (token.size() >= 2 && token.front() == '"' && token.back() == '"') { // String
                    std::string str = token.substr(1, token.size() - 2);

                    // Push null terminator first
                    runtime->push(char_t{'\0'});

                    // Push characters in reverse order
                    for (auto it = str.rbegin(); it != str.rend(); ++it) {
                        runtime->push(char_t{*it});
                    }

                } else if (token.size() == 3 && token.front() == '\'' && token.back() == '\'') { // Char
                    runtime->push(char_t{token[1]});

                } else { // Number
                    try {
                        std::size_t pos = 0;
                        num_t num = std::stod(token, &pos);

                        if (pos != token.size()) {
                            throw std::runtime_error("Invalid token");
                        }

                        runtime->push(num);

                    } catch (...) {
                        throw std::runtime_error("Unknown token: " + token);
                    }
                }
            }
        }
    }

    void State::addToken(TokenWrapper wrapper, std::string token) {
        if (wrapper.tokens != nullptr) {
            wrapper.tokens->push_back(token);
        }

        // Check if depth int exists
        if (wrapper.depth != nullptr) {

            // Increase depth
            if (scope->blockPrefixExists(token)) {

                (*wrapper.depth)++;

            } else if (token == "done") { // Decrease depth
                (*wrapper.depth)--;

                // Throw if invalid depth
                if (*wrapper.depth < 0) {
                    throw std::runtime_error("Unbalanced prefix statements");
                }
            }

        }
    }
}

