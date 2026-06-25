#include "bytecode.hpp"
#include "state.hpp"

namespace forte::bytecode {
    void execInst(State* state, Code* code, Inst* inst);

    void execCode(State* state, Code* code) {

        // Iterate through instructions
        for (auto& inst : code->insts) {
            execInst(state, code, &inst);
        }
    }

    void execInst(State* state, Code* code, Inst* inst) {

        // Parse opcode
        switch (inst->op) {
            case Op::PUSH_VAL: // Push val to stack
                state->push(code->vals[inst->operand]);
                break;

            case Op::CALL_PREFIX: // Call prefix funct
                code->prefixes[inst->operand](state, NULL);
                break;

            case Op::CALL_FUNCT:
                code->functs[inst->operand](state);
                break;

            case Op::CALL_CODE:
                break;

            case Op::CALL_LABEL:
                break;
        }
    }
}
