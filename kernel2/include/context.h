#pragma once
#include "code.h"

namespace qb {

    struct Thread;
    struct Context {
        const Thread* thread;
        const Context* parent;
        const Code* code;
        mem::Block block;
        code_addr_t length;
        code_addr_t cursor;

        Context(
            Thread* thread,
            const Context* parent,
            const Code* code
        ):
            thread(thread),
            parent(parent),
            code(code),
            block(Context::make_block(code)),
            length(code->instructions.size()),
            cursor(0)
        {
            this->init();
        }

        bool init();
        bool tick();

    protected:
        code_addr_t run_instruction(qb::Instruction* instruction);
        
        static mem::Block make_block(const Code* code);
        void print_debug() const;
    };
    
}