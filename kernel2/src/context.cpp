#include "context.h"
#include "runtime.h"

#define QB_CONTEXT_DEBUG

// 0: output
// 1~x: args
// x+1~y: consts
// y+1~z: vars
qb::mem::Block qb::Context::make_block(const Code* code) {

    // Make 

    auto args = code->args.size();
    auto consts = code->consts.size();
    auto vars = code->vars.size();

    auto type_def = qb::TypeDef::block(
        std::vector<qb::TypeDef>(1 + args + consts + vars)
    );

    // Return
    type_def.add.children[0].use = B_TYPE_VOID;
    // Arguments
    for (code_addr_t i = 0; i < args; i++) {
        type_def.add.children[i+1].use = code->args[i];
    }
    // Constants
    for (code_addr_t i = 0; i < consts; i++) {
        type_def.add.children[i+1+args].use = code->consts[i].tdx;
        type_def.add.children[i+1+args].is_const = true;
    }
    // Variables
    for (code_addr_t i = 0; i < vars; i++) {
        type_def.add.children[i+1+args+consts].use = code->vars[i];
    }

    return qb::mem::Block(code->types, type_def);
}

bool qb::Context::init() {
    this->block.data.clear();
    auto consts = code->consts.size();
    for (code_addr_t i = 0; i < consts; i++) {
        auto data = code->consts[i];
        bool res = this->block.data.set_raw(i+1, data.bytes, data.length);
        if (!res) return false;
    }
    #ifdef QB_CONTEXT_DEBUG
        this->print_debug();
        this->thread->print_debug();
    #endif
    return true;
}

bool qb::Context::tick() {
    if (this->cursor >= this->length) {
        #ifdef QB_CONTEXT_DEBUG
            std::cout << "[done]" << std::endl;
        #endif
        return false;
    }

    qb::Instruction* instr = this->code->instructions.at(this->cursor);
    
    #ifdef QB_CONTEXT_DEBUG
        std::cout << "[tick] " << this->cursor << "/" << this->length << "\t" << instr->to_str() << std::endl;
    #endif

    qb::runtime::Error* error;
    qb::code_addr_t next = qb::runtime::run_instruction(this, this->cursor, instr, error);

    #ifdef QB_CONTEXT_DEBUG
        this->print_debug();
        this->thread->print_debug();
    #endif

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

void qb::Context::print_debug() const {
    std::cout << "┌─── context ───" << std::endl;
    std::cout << "│ cursor: " << this->cursor << '/' << this->length << std::endl;
    if (this->block.data.size()) {
        std::cout << "│ #memory" << std::endl;
        std::cout << qb::runtime::block_to_str(&this->block, "│ ") << std::endl;
    }
    std::cout << "└───────────────" << std::endl;
}