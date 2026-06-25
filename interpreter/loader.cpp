#include <dlfcn.h>
#include <fstream>
#include "loader.hpp"
#include "helpers.hpp"

namespace forte::loader {
    void loadFile(State* state, std::string filename) {
        std::string line;
        std::ifstream input(filename);

        if (!input.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        int lineNumber = 1;

        while (std::getline(input, line)) {
            try {
                tokens_t tokens = helpers::tokenize(line);
                state->eval(&tokens);

            } catch (const std::runtime_error& e) { // Handle error
                throw std::runtime_error(
                        filename + ":" + std::to_string(lineNumber) + ": " + e.what()
                        );
            }

            lineNumber++;
        }
    }

    void loadModule(State* state, const std::string path) {
        void* handle = dlopen(path.c_str(), RTLD_NOW);

        if (handle == nullptr) {
            throw std::runtime_error(std::string("Failed to import module: ") + dlerror());
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
