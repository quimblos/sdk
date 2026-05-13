#include "stack.h"

qb::mem::Block qb::exec::Routine::make_block(TypeSolver& solver, const Code* code) {
    port_t ports = 0;
    for (code_addr_t i = 0; i < code->instructions.size(); i++) {
        auto instruction = code->instructions[i];
        if (instruction->type == qb::OpCode::ADD_VAR) {
            ports++;
        }
    }

    auto args = code->args.size();
    auto vars = code->vars.size();

    std::vector<type_t> tdxs(args + vars);
    for (code_addr_t i = 0; i < args; i++) {
        tdxs[i] = code->args[i];
    }
    for (code_addr_t i = 0; i < vars; i++) {
        tdxs[i+args] = code->vars[i];
    }

    return qb::mem::Block(solver, tdxs);
}

qb::exec::Routine* qb::exec::Stack::push(const Code* code){
    auto parent = this->root;
    auto routine = new Routine(*this->solver, parent, code);
    this->root = routine;
    return routine;
}