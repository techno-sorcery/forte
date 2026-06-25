#pragma once

#include "types.hpp"
#include <cstdint>
#include <string>
#include <vector>

/*
   WIP bytecode interpreter
   */
namespace forte::bytecode {
    using operand_t = uint64_t;

    enum class Op : uint8_t {
        PushVal,
        CallFunct,
        CallCode,
        ResolveLabel,
        Jump,
        JumpIfZero
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

