#include <dlfcn.h>
#include "loader.hpp"

namespace loader {
    void loadModule(State* state, const std::string path) {
        void* handle = dlopen(path.c_str(), RTLD_NOW);

        if (handle == nullptr) {
            throw std::runtime_error(std::string("dlopen failed: ") + dlerror());
        }

        dlerror(); // clear old error

        auto init = reinterpret_cast<module_init_t>(
                dlsym(handle, "forte_init_module")
                );

        const char* error = dlerror();
        if (error != nullptr) {
            dlclose(handle);
            throw std::runtime_error(std::string("dlsym failed: ") + error);
        }

        init(state);
        state->addHandle(handle);
    }
}
