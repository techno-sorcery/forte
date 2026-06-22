#pragma once

#include <stdexcept>
#include "types.hpp"

// Generally useful helper functions
namespace helpers {
    tokens_t tokenize(std::string input);
    void file(State* state, std::string filename);

    // Cast value types
    template <typename T>
        T cast(const val_t& val) {
            // Already the requested type
            if (std::holds_alternative<T>(val)) {
                return std::get<T>(val);
            }

            // num_t -> T
            if (std::holds_alternative<num_t>(val)) {
                if constexpr (std::is_arithmetic_v<T>) {
                    return static_cast<T>(std::get<num_t>(val));
                }
            }

            // char_t -> T
            if (std::holds_alternative<char_t>(val)) {
                if constexpr (std::is_arithmetic_v<T>) {
                    return static_cast<T>(std::get<char_t>(val));
                }
            }

            // ptr_t -> T
            if (std::holds_alternative<ptr_t>(val)) {
                if constexpr (std::is_arithmetic_v<T>) {
                    return static_cast<T>(std::get<ptr_t>(val));
                }
            }

            // word_t -> T
            if (std::holds_alternative<word_t>(val)) {
                if constexpr (std::is_same_v<T, word_t>) {
                    return std::get<word_t>(val);
                }
            }

            throw std::runtime_error("Couldn't cast");
        }
}
