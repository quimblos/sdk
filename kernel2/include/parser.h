#pragma once
#include "number.h"
#include "instruction.h"

namespace qb {

    namespace parser {

        struct res_t {
            enum Code {
                OK = 0x00,
                UNEXPECTED_EOF = 0x01,
                CUSTOM_PRIMITIVE_TYPE = 0x02,
                CONST_ASSIGNMENT = 0x03,
                ENTITY_KIND = 0x04,
                COMPARE_OP = 0x05,
                MATH_OP = 0x06,
            } code;

            union {
                std::string* string;
                Instruction* instruction;
                Pointer* pointer;
            } out;

            // - OK  : next address
            // - !OK : error address
            code_addr_t addr;
        };

        const res_t instruction(const byte_t* bytes, code_addr_t length, code_addr_t addr = 0);
        const res_t string(const byte_t* bytes, code_addr_t length, code_addr_t addr = 0);
        const res_t pointer(const byte_t* bytes, code_addr_t length, code_addr_t addr = 0);

    }

}