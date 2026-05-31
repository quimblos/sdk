#pragma once
#include "engine.h"
#include "operator.h"

namespace qb {

    namespace runtime {

        struct Reference {
            const Type* type;
            const data_t value;
        };
        
        struct res_t {
            enum Code {
                OK = 0x00,
                UNRESOLVED_REF_BLOCK,
                UNRESOLVED_REF_PORT,
                UNRESOLVED_DRIVER,
                FAILED_ASSIGN,
                FAILED_MATH,
                FAILED_COMPARE,
                FAILED_RETURN,
                FAILED_CAST_TO_TIME,
                FAILED_CAST_TO_BOOL,
            } code;
            code_addr_t addr;
        };

        bool link_drivers(const Engine* engine, Thread* thread);

        mem::Block* resolve_block(Context* context, const mem::Reference& ref);
        res_t run_instruction(Context* context, code_addr_t addr, Instruction* instr);
        std::string block_to_str(const qb::mem::Block* block, std::string prefix = "");
    };
    
}