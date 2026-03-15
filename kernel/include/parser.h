#pragma once
#include "memory.h"
#include "code.h"
#include "device.h"
#include "script.h"
#include "engine.h"

namespace qb {
    
    struct Bytecode {
        size_t length;
        qb::code_t* bytes;
    };
    
    /**
     * Parser
     */
    
    namespace parser {

        struct res_t {
            uint8_t code;
            code_addr_t error_addr;
            const qb::Script* script;
        };

        res_t parse(qb::Engine& engine, std::string name, std::string hex);
        res_t dump(qb::Script& script);
    }

}

/* Response Codes*/

#define QB_PARSER_R_OK 0

#define QB_PARSER_R_FAILED_UNEXPECTED_EOF 1
#define QB_PARSER_R_FAILED_INVALID_HEADER 2
#define QB_PARSER_R_FAILED_INVALID_OPCODE 3
#define QB_PARSER_R_FAILED_TARGET 4
#define QB_PARSER_R_FAILED_DATA 5
#define QB_PARSER_R_FAILED_DEVICE_INDEX 6
#define QB_PARSER_R_FAILED_UNBOUND_GOTO 7