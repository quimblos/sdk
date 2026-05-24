#pragma once
#include "code.h"

namespace qb {

    struct Method {
        const Method* parent;
        const Code* code;
        mem::Block block;
        code_addr_t length;
        code_addr_t cursor;

        Method(
            const Method* parent,
            const Code* code
        ):
            parent(parent),
            code(code),
            block(Method::make_block(code)),
            length(code->instructions.size()),
            cursor(0)
        {}

        bool tick();

        static mem::Block make_block(const Code* code);
    };
    
}