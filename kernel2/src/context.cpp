#include "context.h"
#include "runtime.h"

#define QB_CONTEXT_DEBUG

// 0: output
// 1~x: args
// x+1~y: vars
qb::mem::Block qb::Context::make_block(const Code* code) {

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

bool qb::Context::init() {
    this->block.data.clear();
    auto consts = code->consts.size();
    for (code_addr_t i = 0; i < consts; i++) {
        auto data = code->consts[i];
        bool res = this->block.data.set_raw(i+1, data.bytes, data.length);
        if (!res) return false;
    }
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

    qb::code_addr_t next = this->run_instruction(instr);

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

qb::code_addr_t qb::Context::run_instruction(qb::Instruction* instr) {
    std::cout << instr->to_str() << std::endl;
    switch (instr->type) {
        case qb::OpCode::SET:
            qb::runtime::resolve_ref(*this, ((qb::instruction::Set*)instr)->target);
            std::cout << "SET" << std::endl;
            break;
        case qb::OpCode::HOLD:
            std::cout << "HOLD" << std::endl;
            break;
        case qb::OpCode::RELEASE:
            std::cout << "RELEASE" << std::endl;
            break;
        case qb::OpCode::GOTO:
            std::cout << "GOTO" << std::endl;
            break;
        case qb::OpCode::IF:
            std::cout << "IF" << std::endl;
            break;
        case qb::OpCode::SET_IF:
            std::cout << "SET_IF" << std::endl;
            break;
        case qb::OpCode::MATH:
            std::cout << "MATH" << std::endl;
            break;
        case qb::OpCode::RETURN:
            std::cout << "RETURN" << std::endl;
            break;
        case qb::OpCode::SLEEP:
            std::cout << "SLEEP" << std::endl;
            break;
        case qb::OpCode::PUBLISH:
            std::cout << "PUBLISH" << std::endl;
            break;
        case qb::OpCode::REBOOT:
            std::cout << "REBOOT" << std::endl;
            break;
    }
    return this->cursor + 1;
}