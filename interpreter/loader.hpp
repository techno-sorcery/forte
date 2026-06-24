#pragma once

#include "state.hpp"
#include <string>


namespace forte::loader {
    using module_init_t = void (*)(State*);

    void loadModule(State* state, const std::string path);
    void loadFile(State* state, const std::string path);
}
