#include "method.h"

// 0: output
// 1~x: args
// x+1~y: vars
qb::mem::Block qb::Method::make_block(const Code* code) {

    // Make 

    auto consts = code->consts.size();
    auto args = code->args.size();
    auto vars = code->vars.size();

    auto type_def = qb::TypeDef::block(
        std::vector<qb::TypeDef>(1 + consts + args + vars)
    );
    // Types
    
    // Return
    type_def.add.children[0].use = B_TYPE_VOID;
    // Constants
    for (code_addr_t i = 0; i < consts; i++) {
        type_def.add.children[i+1].use = code->consts[i].tdx;
    }
    // Arguments
    for (code_addr_t i = 0; i < args; i++) {
        type_def.add.children[i+1+consts].use = code->args[i];
    }
    // Variables
    for (code_addr_t i = 0; i < vars; i++) {
        type_def.add.children[i+1+consts+args].use = code->vars[i];
    }

    return qb::mem::Block(code->types, type_def);
}

bool qb::Method::init() {
    this->block.data.clear();
    auto consts = code->consts.size();
    for (code_addr_t i = 0; i < consts; i++) {
        auto data = code->consts[i];
        bool res = this->block.data.set_raw(i+1, data.bytes, data.length);
        if (!res) return false;
    }
    return true;
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