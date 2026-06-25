#pragma once

#include "types.hpp"
#include <unordered_set>
#include <utility>

namespace forte {
    enum class ValidateMode : bool {
        CheckExists,
        CheckDoesntExist
    };

    /*
       Scope keeps track of data relevant to a particular scope, chiefly the
       symtable. Each state has its own scope, except for those explicitly
       instantiated with their parent's scope. Each scope object points to
       the scope of its parent so they can be recursively searched when 
       resolving symbols. 

       It similarly keeps a set of "block" prefix labels (ie those of a 
       variable length, terminated with "done") so the parser can keep track
       of function depth.
       */
    class Scope {

        public:
            Scope() // Global
                : parent(nullptr) {};
            Scope(scope_t parentScope) // Local
                : parent(std::move(parentScope)) {};

            bool inSymtable(label_t label); // Check if label in symtable
            entry_t getEntry(label_t label); // Return symtable entry

            // Add entry to symtable, and return pointer
            entry_t* addEntry(label_t label, entry_t item);

            bool blockPrefixExists(label_t label); // Check if label is prefix tok
            void addBlockPrefix(label_t label); // Add to prefix token table

        private:
            symtable_t symtable;
            scope_t parent;
            std::unordered_set<label_t> blockPrefixes;

            // Validate label
            void validateLabel(label_t label, ValidateMode mode); 
    };
}
