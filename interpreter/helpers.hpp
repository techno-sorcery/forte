#pragma once

#include <stdexcept>
#include "types.hpp"

/*
   This namespace contains generally useful functions such as "tokenize" and
   "cast" which I couldn't put/didn't feel like putting in one of the classes.
   I might refactor it later if I find a better place for them.
   */
namespace forte::helpers {
    tokens_t tokenize(std::string input);

    // Cast value types
    template <typename T>
        T cast(const val_t& val) {
            static_assert(
                    std::is_same_v<T, num_t>  ||
                    std::is_same_v<T, ptr_t>  ||
                    std::is_same_v<T, char_t> ||
                    std::is_same_v<T, word_t>,
                    "helpers::cast<T>: invalid target type"
                    );

            // Already the requested type
            if (std::holds_alternative<T>(val)) {
                return std::get<T>(val);
            }

            // num_t -> T
            if (std::holds_alternative<num_t>(val)) {
                return static_cast<T>(std::get<num_t>(val));
            }

            // ptr_t -> T
            if (std::holds_alternative<ptr_t>(val)) {
                return static_cast<T>(std::get<ptr_t>(val));
            }

            // char_t -> T
            if (std::holds_alternative<char_t>(val)) {
                return static_cast<T>(std::get<char_t>(val));
            }

            // word_t -> T
            if (std::holds_alternative<word_t>(val)) {
                return static_cast<T>(std::get<word_t>(val));
            }

            throw std::runtime_error("Couldn't cast");
        }
}
