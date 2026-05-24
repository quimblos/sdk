#include "method.h"

// 0: output
// 1~x: args
// x+1~y: vars
qb::mem::Block qb::Method::make_block(const Code* code) {
    port_t ports = 0;
    for (code_addr_t i = 0; i < code->instructions.size(); i++) {
        auto instruction = code->instructions[i];
        if (instruction->type == qb::OpCode::ADD_VAR) {
            ports++;
        }
    }

    auto args = code->args.size();
    auto vars = code->vars.size();

    auto type_def = qb::TypeDef::block(
        std::vector<qb::TypeDef>(1 + args + vars)
    );
    type_def.add.children[0].use = B_TYPE_VOID;
    for (code_addr_t i = 0; i < args; i++) {
        type_def.add.children[i+1].use = code->args[i];
    }
    for (code_addr_t i = 0; i < vars; i++) {
        type_def.add.children[i+1+args].use = code->vars[i];
    }

    return qb::mem::Block(type_def);
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