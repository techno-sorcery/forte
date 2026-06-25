#pragma once

#include "types.hpp"
#include <stdexcept>

namespace forte {

    /*
       Runtime keeps track of globally-used interpreter data, such as the
       data stack, return stack, and loaded module handles. It is instantiated
       by the global state upon its own instantiation, and is passed as a
       shared pointer between child states
       */
    class Runtime {
        public:
            ~Runtime();

            // Safely pop from stack, check type, and return
            template <typename T>
                T pop() {
                    if (stack.size() < 1) {
                        throw std::runtime_error("Stack underflow");
                    }

                    val_t a = stack.top();

                    if constexpr (std::is_same_v<T, val_t>) {
                        stack.pop();
                        return a;
                    } else {
                        if (!std::holds_alternative<T>(a)) {
                            throw std::runtime_error("Invalid type");
                        }

                        stack.pop();
                        return std::get<T>(a);
                    }
                }

            void push(val_t val); // Push value to stack

            void addHandle(void* handle); // Add module handle

            val_t getData(ptr_t ptr); // Return data located at pointer
            void setData(ptr_t ptr, val_t val); // Set data at pointer location

            ptr_t alloc(int entries); // Allocate data entries

            ptr_t getDataBase() { // Return data base pointer
                return data.size();
            }

            void resizeData(ptr_t size) { // Resize data stack
                data.resize(size);
            }

        private:
            stack_t stack;
            data_t data;
            std::vector<void*> handles;

            void validatePtr(ptr_t ptr);
    };
}
