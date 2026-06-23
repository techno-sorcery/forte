#pragma once

#include <stdexcept>
#include <string>

#include "prefix.hpp"
#include "types.hpp"

// Class to keep track of interpreter state
class State {
    public:
        State(); // Global scope
        // Local scope
        State(StateContext context);
        // Local scope + pass in symtable
        State(StateContext context, symtable_t* globalTable);
        // Parent scope (conditionals/loops)
        State(StateContext context, State* parentState);
        ~State(); // Destructor

        // Safely pop from stack, check type, and return
        template <typename T>
            T pop() {
                stack_t* stack = stateContext.stack;

                if (stack->size() < 1) {
                    throw std::runtime_error("Stack underflow");
                }

                val_t a = stack->top();

                if constexpr (std::is_same_v<T, val_t>) {
                    stack->pop();
                    return a;

                } else {
                    if (!std::holds_alternative<T>(a)) {
                        throw std::runtime_error("Invalid type");
                    }

                    stack->pop();
                    return std::get<T>(a);
                }
            }


        ptr_t alloc(int entries); // Allocate blank entries

        bool inSymtable(std::string label); // Check if label in symtable
        entry_t getSymEntry(std::string label); // Return symtable entry

        // Create new symtable entry and allocate data entries
        ptr_t newSymEntry(std::string label, int entries);
        tokens_t* newFunction(std::string label);

        val_t getData(ptr_t ptr); // Return data located at pointer
        void setData(ptr_t ptr, val_t val); // Set data at pointer location

        void eval(tokens_t* tokens); // Evaluate expression
        void addFunct(std::string label, funct_t funct); // Add module funct

        void push(val_t val) { // Push value to stack
            stateContext.stack->push(val);
        }

        void addHandle(void* handle) {
            handles.push_back(handle);
        }

        StateContext getStateContext() {
            return stateContext;
        }

        symtable_t* getSymtable() {
            return symtable;
        }

        PrefixContext* getPrefixContext() {
            return &prefixContext;
        }


    private:
        StateContext stateContext;
        PrefixContext prefixContext;

        symtable_t* symtable = NULL;
        ptr_t dataBase;
        bool passedSym = false;
        State* parent = NULL;
        std::vector<void*> handles;

        // Validate label
        void validateSym(std::string label, bool checkExists); 

        // Validate ptr
        void validatePtr(ptr_t ptr);
};
