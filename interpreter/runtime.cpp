#include "runtime.hpp"
#include <dlfcn.h>

namespace forte {

    Runtime::~Runtime() {
        // Handles
        for (void* handle : handles) {
            dlclose(handle);
        }
    }

    void Runtime::push(val_t val) { 
        stack.push(val);
    }

    void Runtime::addHandle(void* handle) {
        handles.push_back(handle);
    }

    val_t Runtime::getData(ptr_t ptr) {
        validatePtr(ptr);
        return data[ptr];
    }

    void Runtime::setData(ptr_t ptr, val_t val) {
        validatePtr(ptr);
        data[ptr] = val;
    }

    ptr_t Runtime::alloc(int entries) {

        // Check # entries
        if (entries <= 0) {
            throw std::runtime_error("Invalid number of entries");
        }

        // Store pointer to last element
        ptr_t ptr = data.size();

        // Allocate new entries
        for (int i = 0; i < entries; i++) {
            val_t entry;
            data.push_back(entry);
        }

        // Return pointer to first entry
        return ptr;
    }

    void Runtime::validatePtr(ptr_t ptr) {
        if (ptr >= data.size()) { 
            throw std::runtime_error("Invalid pointer");
        }
    }
}
