#pragma once

#include <memory>
#include <string>
#include "types.hpp"
#include "runtime.hpp"
#include "scope.hpp"

namespace forte {
    enum class StateMode : bool {
        NewScope,
        SharedScope
    };

    enum class PrefixEntryMode : bool {
        NoToken,
        AddToken
    };

    /*
       State acts as a container for the scope and runtime objects, as well as 
       an interface/wrapper for their functions. With the exception of prefix
       functions passing scope/runtime objects to child states, modules 
       interact with the interpreter through passed state pointers.

       Each separate lexical scope has its own state object, which holds a 
       pointer to the global runtime object + the global scope, its own local
       scope, or the scope of its parent depending on whether it it's global,
       local, or module scope.
       */
    class State {
        public:
            State(); // Global scope
                     // Local scope
            State(runtime_t parentRuntime, scope_t parentScope, 
                    State* parentState, StateMode mode);
            // Local scope + pass in scope
            State(Runtime* parentRuntime, State* parentState, Scope* parentScope);
            ~State(); // Destructor

            void addToken(TokenWrapper wrapper, std::string token);

            // Create new symtable entry and allocate data entries
            ptr_t newEntry(std::string label, int entries);
            void newEntry(std::string label, funct_t funct); // Add module funct

            // Add module prefix
            void newEntry(std::string label, prefix_t prefix, 
                    PrefixEntryMode mode);

            tokens_t* newEntry(std::string label); // Add used defined function

            void eval(tokens_t* tokens); // Evaluate expression

            PrefixContext* getPrefixContext() {
                return &prefixContext;
            }

            void push(val_t val) { // Push to runtime stack
                runtime->push(val);
            }

            // Pop from runtime stack
            template <typename T>
                T pop() {
                    return runtime->pop<T>();
                }

            void addHandle(void* handle) { // Add runtime handle
                runtime->addHandle(handle);
            }

            void setData(ptr_t ptr, val_t val) { // Set runtime data at pointer
                runtime->setData(ptr, val);
            } 

            val_t getData(ptr_t ptr) { // Return data located at pointer
                return runtime->getData(ptr);
            }

            std::shared_ptr<Runtime> getRuntime() {
                return runtime;
            }

            std::shared_ptr<Scope> getScope() {
                return scope;
            }

        private:
            runtime_t runtime = nullptr;
            scope_t scope = nullptr;

            State* parent = nullptr;
            ptr_t dataBase;

            // StateContext stateContext;
            PrefixContext prefixContext;

            // Validate ptr
            void validatePtr(ptr_t ptr);
    };
}

