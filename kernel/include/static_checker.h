#pragma once
#include "memory.h"
#include "script.h"
#include "engine.h"

namespace qb {
    
    /**
     * Static Checker
     */
    
    namespace static_checker {

        struct res_t {
            uint8_t code;
            code_addr_t error_addr;
        };

        res_t resolve_pointer(const Script* script, code_addr_t addr, device_t device_i, port_t* port, Node** target);
        res_t check(qb::Engine& engine, const Script* script);
    }

}

/* Response Codes*/

#define QB_STATIC_CHECKER_R_OK 0

#define QB_STATIC_CHECKER_R_INVALID_DEVICE 1
#define QB_STATIC_CHECKER_R_INVALID_NODE 2
#define QB_STATIC_CHECKER_R_INVALID_TARGET 3
#define QB_STATIC_CHECKER_R_TYPE_MISMATCH 4