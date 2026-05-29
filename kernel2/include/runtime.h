#pragma once
#include "engine.h"
#include "operator.h"

namespace qb {

    namespace runtime {

        struct Reference {
            const Type* type;
            const data_t value;
        };
        
        struct Error {
            enum Code {
                OK = 0x00,
                UNRESOLVED_REF_BLOCK,
                UNRESOLVED_REF_PORT,
                FAILED_CAST_TO_TIME
            } code;
            code_addr_t addr;
            Instruction* instr;
        };

        mem::Block* resolve_block(Context* context, const mem::Reference& ref);
        code_addr_t run_instruction(Context* context, code_addr_t addr, Instruction* instr, Error* out_error);
        std::string block_to_str(const qb::mem::Block* block, std::string prefix = "");
        
    };
    
}