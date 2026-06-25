#include "scope.hpp"
#include <stdexcept>

namespace forte {
    bool Scope::inSymtable(label_t label) {
        auto it = symtable.find(label);

        if (it != symtable.end()) {
            return true;
        }

        if (parent != nullptr) { // Local scope, check parent
            return parent->inSymtable(label);

        } 

        return false;
    }

    entry_t Scope::getEntry(label_t label) {
        auto it = symtable.find(label);

        if (it != symtable.end()) {
            return it->second;
        }

        if (parent != nullptr) {
            if (parent->inSymtable(label)) {
                return parent->getEntry(label);
            }
        }

        throw std::runtime_error("Label doesn't exist");
    }

    void Scope::validateLabel(label_t label, ValidateMode mode) {
        if (label.empty()) { // Empty
            throw std::runtime_error("Blank label");
        }

        if (mode == ValidateMode::CheckExists && 
                inSymtable(label)) { // Already exists
            throw std::runtime_error("Label already exists");

        } else if (mode == ValidateMode::CheckDoesntExist && 
                !inSymtable(label)) { // Doesn't exist
            throw std::runtime_error("Label doesn't exist");
        }
    }

    entry_t* Scope::addEntry(label_t label, entry_t item) {
        validateLabel(label, ValidateMode::CheckExists);
        symtable[label] = item;

        return &symtable[label];
    }

    bool Scope::blockPrefixExists(label_t label) {
        if (blockPrefixes.count(label) > 0) {
            return true;
        }

        if (parent != nullptr) { // Check parent scope
            return parent->blockPrefixExists(label);
        }

        // Global + not found
        return false;
    }

    void Scope::addBlockPrefix(label_t label) {
        if (blockPrefixExists(label)) {
            throw std::runtime_error("Prefix token already exists: " + label);
        }

        blockPrefixes.insert(label);

    }
}

