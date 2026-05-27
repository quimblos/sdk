#pragma once
#include "number.h"
#include "code.h"

namespace qb {

    namespace parser {

        struct res_t {
            enum Code {
                OK = 0x00,
                UNEXPECTED_EOF,
                CUSTOM_PRIMITIVE_TYPE,
                UNKNOWN_TYPE_KIND,
                CONST_ASSIGNMENT,
                ENTITY_KIND,
                COMPARE_OP,
                MATH_OP,
                UNKNOWN_OP_CODE,
                INVALID_HEADER
            } code;

            union {
                std::string* string;
                mem::Reference* ref;
                Instruction* instruction;
                TypeDef* type_def;
                qb::Code* code;
            } out;

            // - OK  : next address
            // - !OK : error address
            code_addr_t addr;
        };

        const res_t instruction(const byte_t* bytes, code_addr_t length, code_addr_t addr = 0);
        const res_t string(const byte_t* bytes, code_addr_t length, code_addr_t addr = 0);
        const res_t ref(const byte_t* bytes, code_addr_t length, code_addr_t addr = 0);
        const res_t type_def(const byte_t* bytes, code_addr_t length, code_addr_t addr = 0, bool force_add = false);

        const res_t code(const byte_t* bytes, code_addr_t length, code_addr_t addr = 0);

    }

}