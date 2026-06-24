#include "state.hpp"
#include "loader.hpp"
#include "types.hpp"
#include <dlfcn.h>
#include <string>

namespace forte {
    // Global
    State::State() {
        stateContext.global = this;
        stateContext.stack = new stack_t;
        stateContext.data = new data_t;
        symtable = new symtable_t;
        passedSym = false;
        dataBase = 0;
        parent = NULL;
        handles = {};

        // Fill symtable with built-in functions
        loader::loadModule(this, "modules/prefix.so");
        loader::loadModule(this, "modules/core.so");
        loader::loadModule(this, "modules/io.so");
    }


    // Local
    State::State(StateContext context) {
        stateContext = context;
        symtable = new symtable_t;
        passedSym = false;
        dataBase = stateContext.data->size();
        parent = NULL;
        handles = {};
    }


    // Local + passed in symtable
    State::State(StateContext context, symtable_t* globalTable) {
        symtable = globalTable;
        stateContext = context;
        passedSym = true;
        dataBase = stateContext.data->size();
        parent = NULL;
        handles = {};
    }

    // Loop/if statement
    State::State(StateContext context, State* parentState) {
        stateContext = context;
        symtable = new symtable_t;
        passedSym = false;
        dataBase = stateContext.data->size();
        parent = parentState;
    }


    // Destructor
    State::~State() {
        if (stateContext.global != this) { // Local scope
            stateContext.data->resize(dataBase);

        } else { // Global scope
            delete stateContext.stack;
            delete stateContext.data;
        }

        // Handles
        // for (void* handle : handles) {
        //     dlclose(handle);
        // }

        // Symtable
        if (!passedSym) {
            delete symtable;
        }
    }

    void State::addFunct(std::string label, funct_t funct) {
        validateSym(label, 1);

        (*symtable)[label] = funct;
    }

    void State::addPrefix(std::string label, prefix_t prefix, bool insertToken) {
        validateSym(label, 1);

        (*symtable)[label] = prefix;

        if (insertToken) {
            stateContext.prefixTokens.insert(label);
        }
    }

    void State::validateSym(std::string label, bool checkExists) {
        if (label.empty()) { // Empty
            throw std::runtime_error("Blank label");
        }

        if (std::isdigit(static_cast<char>(label[0]))) { // First char not alpha
            throw std::runtime_error("First label character can't be a digit");
        }

        if (checkExists && inSymtable(label)) { // Already exists
            throw std::runtime_error("Label already exists");

        } else if (!checkExists && !inSymtable(label)) { // Doesn't exist
            throw std::runtime_error("Label doesn't exist");
        }
    }

    bool State::inSymtable(std::string label) {
        auto it = symtable->find(label);

        if (it != symtable->end()) {
            return true;
        }

        if (parent != NULL) {
            return parent->inSymtable(label);
        }

        if (stateContext.global != NULL && stateContext.global != this) {
            return stateContext.global->inSymtable(label);
        }

        return false;
    }


    entry_t State::getSymEntry(std::string label) {
        auto it = symtable->find(label);

        if (it != symtable->end()) {
            return it->second;
        }

        if (parent != NULL) {
            if (parent->inSymtable(label)) {
                return parent->getSymEntry(label);
            }
        }

        if (stateContext.global != NULL && stateContext.global != this) {
            if (stateContext.global->inSymtable(label)) {
                return stateContext.global->getSymEntry(label);
            }
        }

        throw std::runtime_error("Label doesn't exist");
    }


    ptr_t State::newSymEntry(std::string label, int entries) {

        // Check validity of label
        validateSym(label, 1);

        // Allocate new data entries
        ptr_t ptr = alloc(entries);

        // Set new symtable entry
        (*symtable)[label] = ptr;

        return ptr;
    }

    tokens_t* State::newFunction(std::string label) {

        // Check validity of label
        validateSym(label, 1);

        tokens_t tokens;
        (*symtable)[label] = tokens;

        return &std::get<tokens_t>((*symtable)[label]);
    }

    val_t State::getData(ptr_t ptr) {
        data_t* data = stateContext.data;

        validatePtr(ptr);
        val_t val = (*data)[ptr];

        return (*data)[ptr];
    }

    void State::setData(ptr_t ptr, val_t val) {
        data_t* data = stateContext.data;

        validatePtr(ptr);

        (*data)[ptr] = val;

    }

    void State::validatePtr(ptr_t ptr) {
        data_t* data = stateContext.data;

        if (ptr >= data->size()) { 
            throw std::runtime_error("Invalid pointer");
        }
    }

    ptr_t State::alloc(int entries) {
        data_t* data = stateContext.data;

        // Check # entries
        if (entries <= 0) {
            throw std::runtime_error("Invalid number of entries");
        }

        // Store pointer to last element
        ptr_t ptr = data->size();

        // Allocate new entries
        for (int i = 0; i < entries; i++) {
            val_t entry;
            data->push_back(entry);
        }

        // Return pointer to first entry
        return ptr;
    }


    // Interpret a line of code
    void State::eval(tokens_t* tokens) {
        State* global = stateContext.global;

        // Evaluate all tokens
        for (std::string token : *tokens) {
            std::string lower = token;

            // Convert token to lowercase
            for (char &c : lower) {
                c = std::tolower(static_cast<unsigned char>(c));
            }

            // Parse token
            if (prefixContext.prefix != NULL) { // Prefix command

                // Execute and check if finished
                try {

                    // Exit prefix mode if done
                    if (prefixContext.prefix(this, token)) {
                        prefixContext.prefix = NULL;
                    }

                } catch (const std::runtime_error& e) { // Handle error

                    // Exit prefix mode and throw
                    prefixContext.prefix = NULL;
                    throw;
                }

            } else if (inSymtable(lower)) { // Check if token in symtable
                entry_t entry = getSymEntry(lower);

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
                        prefixContext.prefix = NULL;
                        throw;
                    }
                } else if (std::holds_alternative<ptr_t>(entry)) { // Pointer
                    ptr_t ptr = std::get<ptr_t>(entry);

                    // Push pointer to stack
                    push(ptr);

                } else if (std::holds_alternative<tokens_t>(entry)) { // User funct
                    tokens_t* tokens = &std::get<tokens_t>(entry);

                    // Create new scope
                    State state(stateContext);

                    // Evaluate tokens
                    state.eval(tokens);
                }

            } else { // Otherwise, push value

                if (token.size() >= 2 && token.front() == '"' && token.back() == '"') { // String
                    std::string str = token.substr(1, token.size() - 2);

                    // Push null terminator first
                    push(char_t{'\0'});

                    // Push characters in reverse order
                    for (auto it = str.rbegin(); it != str.rend(); ++it) {
                        push(char_t{*it});
                    }

                } else if (token.size() == 3 && token.front() == '\'' && token.back() == '\'') { // Char
                    push(char_t{token[1]});

                } else { // Number
                    try {
                        std::size_t pos = 0;
                        num_t num = std::stod(token, &pos);

                        if (pos != token.size()) {
                            throw std::runtime_error("Invalid token");
                        }

                        push(num);

                    } catch (...) {
                        throw std::runtime_error("Unknown token: " + token);
                    }
                }
            }
        }
    }

    void State::addToken(TokenWrapper wrapper, std::string token) {
        if (wrapper.tokens != NULL) {
            wrapper.tokens->push_back(token);
        }

        auto& prefixTokens = stateContext.prefixTokens;

        // Increase depth
        if (prefixTokens.count(token) > 0) {

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

