#include <iostream>
#include <string_view>
#include <unordered_set>
#include "prefix.hpp"
#include "helpers.hpp"
#include "types.hpp"
#include "loader.hpp"

const std::unordered_set<std::string_view> prefixTokens = {
    "for", "def", "while", "if", "rep"
};

typedef struct TokenWrapper {
    tokens_t* tokens;
    int* depth;
} TokenWrapper;

namespace {
    void addToken(TokenWrapper wrapper, std::string token) {
        if (wrapper.tokens != NULL) {
            wrapper.tokens->push_back(token);
        }

        // Increase depth
        if (prefixTokens.count(token)) {

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

namespace prefix {
    bool cast(State* state, std::string token) {

        if (token == "") { // First iteration
            return false;
        } 

        // Pull val off stack, cast, and put it back
        val_t val = state->pop<val_t>();

        if (token == "#") { // Num
            num_t num = helpers::cast<num_t>(val);
            state->push(num);

        } else if (token == "$") { // Ptr
            ptr_t ptr = helpers::cast<ptr_t>(val);
            state->push(ptr);

        } else if (token == "@") { // Char
            char_t chr = helpers::cast<char_t>(val);
            state->push(chr);

        } else { // Invalid
            state->push(val);
            throw std::runtime_error("Invalid cast");
        }

        return true;
    }

    bool ifElse(State* state, std::string token) {
        PrefixContext* prefixContext = state->getPrefixContext();

        if (token == "") { // First iteration
            IfContext context;

            context.condition = helpers::cast<word_t>(state->pop<val_t>());
            prefixContext->data = context;

            return false;
        } 

        IfContext* context = &std::get<IfContext>(prefixContext->data);

        if (token == "done" && context->depth == 0) { // Last iteratiion

            // Only bother to eval if size > 0 
            if (context->body.size() > 0) {

                // Create child state
                State child(state->getStateContext(), state);

                // Evaluate body
                child.eval(&context->body);
            }

            return true;

        } else if (token == "else" && context->depth == 0) { // Else
            context->readElse = true;

        } else if  (context->condition) { // True

            if (!context->readElse) { // Read section before else
                addToken({&context->body, &context->depth}, token);

            } else { // Don't store, but keep track of depth
                addToken({NULL, &context->depth}, token);
            }

        } else if (!context->condition) { // False

            if (context->readElse) { // Read section after else
                addToken({&context->body, &context->depth}, token);

            } else { // Don't store, but keep track of depth
                addToken({NULL, &context->depth}, token); 
            }
        }

        return false;
    }

    bool rep(State* state, std::string token) {
        PrefixContext* prefixContext = state->getPrefixContext();

        if (token == "") { // First iteration
            RepContext context;

            context.count = state->pop<num_t>();
            prefixContext->data = context;

            return false;
        } 

        RepContext* context = &std::get<RepContext>(prefixContext->data);

        if (token == "done"
                && context->depth == 0) { // Last iteration

            // Create child state
            State child(state->getStateContext(), state);

            // Eval loop
            while (true) {

                // Check counter
                if (context->count <= 0) {
                    break;
                }

                // Evaluate body
                child.eval(&context->body);

                // Decrement count
                context->count--;
            }

            // Return
            return true;

        } else {
            addToken({&context->body, &context->depth}, token);
        }

        return false;
    }

    bool whileLoop(State* state, std::string token) {
        PrefixContext* prefixContext = state->getPrefixContext();

        if (token == "") { // First iteration
            prefixContext->data = WhileContext{};
            return false;
        } 

        WhileContext* context = &std::get<WhileContext>(prefixContext->data);

        if (context->state == whileState::COND) { // Condition
            if (token == "do") { // Next state
                context->state = whileState::BODY;

            } else {
                addToken({&context->cond, &context->depth}, token);
            }

        } else { // Body
            if (token == "done"
                    && context->depth == 0) { // Last iteration

                // Create child state
                State child(state->getStateContext(), state);

                // Eval loop
                while (true) {

                    // Evaluate condition
                    child.eval(&context->cond);
                    val_t val = child.pop<val_t>();

                    if (!helpers::cast<word_t>(val)) {
                        break;
                    }

                    // Evaluate body
                    child.eval(&context->body);
                }

                // Return
                return true;

            } else {
                addToken({&context->body, &context->depth}, token);
            }
        }

        return false;
    }

    bool forLoop(State* state, std::string token) {
        PrefixContext* prefixContext = state->getPrefixContext();

        if (token == "") { // First iteration
            prefixContext->data = ForContext{};
            return false;

        } 

        ForContext* context = &std::get<ForContext>(prefixContext->data);

        if (context->state == forState::INIT) { // Init
            if (token == "cond") { // Next state
                context->state = forState::COND;

            } else {
                addToken({&context->init, &context->depth}, token);
            }

        } else if (context->state == forState::COND) { // Condition
            if (token == "step") { // Next state
                context->state = forState::UPDATE;

            } else {
                addToken({&context->cond, &context->depth}, token);
            }
        } else if (context->state == forState::UPDATE) { // Condition
            if (token == "do") { // Next state
                context->state = forState::BODY;

            } else {
                addToken({&context->update, &context->depth}, token);
            }
        } else { // Body
            if (token == "done"
                    && context->depth == 0) { // Last iteration

                // Create child state
                State child(state->getStateContext(), state);

                // Init
                child.eval(&context->init);

                // Eval loop
                while (true) {

                    // Evaluate condition
                    child.eval(&context->cond);
                    val_t val = child.pop<val_t>();

                    if (!helpers::cast<word_t>(val)) {
                        break;
                    }

                    // Evaluate body
                    child.eval(&context->body);

                    // Evaluate update
                    child.eval(&context->update);
                }

                // Return
                return true;

            } else {
                addToken({&context->body, &context->depth}, token);
            }
        }

        return false;
    }

    bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size()
            && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    bool import(State* state, std::string token) {
        if (token == "") { // First iteration
            return false;

        }  

        // Second iteration
        if (endsWith(token, ".so")) {
            loader::loadModule(state, token);
            printf("Loaded\n");
            return true;
        }

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
        PrefixContext* prefixContext = state->getPrefixContext();

        if (token == "") {
            prefixContext->data = DefContext{};
            return false;
        }

        DefContext* context = &std::get<DefContext>(prefixContext->data);

        if (context->tokens == NULL) { // Second iteration, create symbol
            context->tokens = state->newFunction(token);

        } else if (token == "done" 
                && context->depth == 0) { // Last iteration
            return true;

        } else {
            addToken({context->tokens, &context->depth}, token);
        }

        return false;
    }
}
