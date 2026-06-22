#include <unordered_set>
#include "prefix.hpp"
#include "helpers.hpp"
#include "types.hpp"

namespace {
    void addToken(PrefixContext* context, std::string token) {
        context->tokens->push_back(token);

        // Increase depth
        if (token == "loop" ||
                token == "def") {

            context->tokenDepth++;

        } else if (token == "done") {
            context->tokenDepth--;
        }
    }
}

namespace prefix {
    bool loop(State* state, std::string token) {
        PrefixContext* context = state->getPrefixContext();

        if (token == "") { // First iteration
            context->tokenDepth = 0;
            context->tokens = new tokens_t;
            context->loopIndex = state->pop<num_t>();

        } else if (token == "done") { // Last iteration
            
            // Create child state
            State child(state->getStateContext(), state);

            // Eval loop

            // Clear context and return
            delete context->tokens;
            context->tokens = NULL;
            return true;

        } else {

        }

        return false;
    }

    bool import(State* state, std::string token) {
        if (token == "") { // First iteration
            return false;

        }  

        // Second iteration
        State importState(state->getStateContext(), state->getSymtable());

        helpers::file(&importState, token);
        return true;
    }

    bool var(State* state, std::string token) {
        if (token == "") { // First iteration
            return false;

        } else { // Second iteration

            // Create new entry and push data entry ptr to stack
            ptr_t ptr = state->newSymEntry(token, 1);
            state->push(ptr);

            return true;
        }
    }

    bool def(State* state, std::string token) {
        PrefixContext* context = state->getPrefixContext();

        if (token == "") { // First iteration
            context->tokenDepth = 0;
            context->tokens = NULL;

        } else if (context->tokens == NULL) { // Second iteration, create symbol
            context->tokens = state->newFunction(token);

        } else if (token == "done" && context->tokenDepth == 0) { // Last iteration
            return true;

        } else {
            addToken(context, token);
        }

        return false;
    }
}
