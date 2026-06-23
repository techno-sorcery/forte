#include "state.hpp"
#include <string>

using module_init_t = void (*)(State*);

namespace loader {

    void loadModule(State* state, const std::string path);
}
