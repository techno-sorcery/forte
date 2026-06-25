#pragma once

#include "state.hpp"
#include <string>

/*
   This namespace contains functions related to module and library loading.
   Libraries consist of Forte code (or in the future bytecode) and are
   interpreted at runtime, whereas modules are written in C++ and
   compiled to shared objects before importation.
   */
namespace forte::loader {
    using module_init_t = void (*)(State*);

    void loadModule(State* state, const std::string path);
    void loadFile(State* state, const std::string path);
}
