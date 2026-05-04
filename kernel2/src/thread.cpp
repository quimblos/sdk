#include "thread.h"

const qb::mem::Block& qb::Thread::make_const(const TypeSolver& solver, const Code* code) {
    std::vector<type_t> tdxs;
    for (code_addr_t i = 0; i < code->size; i++) {
        auto instruction = code->instructions[i];
        if (instruction.type == qb::OpCode::ADD_CONST) {
            // ...
        }
    }
    return qb::mem::Block(solver, tdxs);
}

qb::mem::Block& qb::Thread::make_var(const TypeSolver& solver, const Code* code) {
    port_t ports = 0;
    for (code_addr_t i = 0; i < code->size; i++) {
        auto instruction = code->instructions[i];
        if (instruction.type == qb::OpCode::ADD_VAR) {
            ports++;
        }
    }

    std::vector<type_t> tdxs(ports);
    for (code_addr_t i = 0; i < code->size; i++) {
        auto instruction = code->instructions[i];
        if (instruction.type == qb::OpCode::ADD_VAR) {
            tdxs[i] = instruction.tdx;
        }
    }

    return qb::mem::Block(solver, tdxs);
}