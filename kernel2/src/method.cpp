#include "method.h"

#define QB_METHOD_DEBUG

// 0: output
// 1~x: args
// x+1~y: vars
qb::mem::Block qb::Method::make_block(TypeSolver& solver, const Code* code) {
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

bool qb::Method::tick() {
    if (this->cursor >= this->length) {
        #ifdef QB_METHOD_DEBUG
            std::cout << "[done]" << std::endl;
        #endif
        return false;
    }

    qb::Instruction* instr = this->code->instructions.at(this->cursor);
    
    #ifdef QB_METHOD_DEBUG
        std::cout << "[tick] " << this->cursor << "/" << this->length << "\t" << instr->to_str() << std::endl;
    #endif

    qb::code_addr_t next = this->cursor + 1;
    // qb::code_addr_t next = this->run_instruction(instr);

    // // Program is ending
    // if (next >= this->length) {
    //     // Was running, end with success, no output
    //     if (this->state == runner::State::RUNNING) {
    //         this->state = runner::State::OK;
    //     }
    //     // Started sleeping, advance and wait
    //     else if (this->state == runner::State::SLEEPING) {
    //         this->cursor = next;
    //         return true;
    //     }
    //     return false;
    // }

    this->cursor = next;
    return true;
}