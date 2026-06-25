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
            case Op::PushVal: // Push val to stack
                state->push(code->vals[inst->operand]);
                break;

            case Op::CallFunct:
                code->functs[inst->operand](state);
                break;

            case Op::CallCode:
                break;

            case Op::ResolveLabel:
                break;;

            case Op::Jump:
                break;

            case Op::JumpIfZero: // Jump if stack val == 0
                word_t cond = state->pop<word_t>();

                break;
        }
    }
}
