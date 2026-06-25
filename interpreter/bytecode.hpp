#pragma once

#include "types.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace forte::bytecode {
    using operand_t = uint64_t;

    enum class Op : uint8_t {
        PUSH_VAL,
        CALL_PREFIX,
        CALL_FUNCT,
        CALL_CODE,
        CALL_LABEL
    };

    struct Inst {
        Op op;
        operand_t operand;
    };

    struct Code {
        std::vector<Inst> insts;

        std::vector<val_t> vals;
        std::vector<prefix_t> prefixes;
        std::vector<funct_t> functs;
        std::vector<std::string> labels;
    };

    void execCode(State* state, Code* code);
}

