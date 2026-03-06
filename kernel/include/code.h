#pragma once
#include <stdint.h>
#include <memory>
#include <iostream>
#include "device.h"

#define HEADER_QUIMBLOS 'q', 'b', 0x00, 0x00

namespace qb {

    /**
     * Command
     */
    
    enum OpCode {
        // 0x0* -> Parser commands
        USE_DEVICE = 0x01,  // {str}
        USE_PORT = 0x02,    // 0xTYPE
        // 0x1* -> Register manipulation commands
        SET = 0x10,         // 0xDEV 0xPORT {?}
        SET_AT = 0x11,      // 0xDEV 0xPORT IDX{?} {?}
        HOLD = 0x1A,        // 0xDEV
        RELEASE = 0x1B,     // 0xDEV
        // 0x2* -> Flow control commands 
        GOTO = 0x20,        // 0xCODE 0xCODE
        IF_EQ = 0x21,       // 0xDEV 0xPORT 0xCODE 0xCODE 0xCODE_ 0xCODE_ {?}
        IF_GT = 0x22,       // 0xDEV 0xPORT 0xCODE 0xCODE 0xCODE_ 0xCODE_ {?}
        IF_GTEQ = 0x23,     // 0xDEV 0xPORT 0xCODE 0xCODE 0xCODE_ 0xCODE_ {?}
        // 0x3* -> Arithmetic commands 
        ADD = 0x30,         // 0xDEV 0xPORT {?}
        SUB = 0x31,         // 0xDEV 0xPORT {?}
        MULT = 0x32,        // 0xDEV 0xPORT {?}
        DIV = 0x33,         // 0xDEV 0xPORT {?}
        MOD = 0x34,         // 0xDEV 0xPORT {?}
        // 0xD* -> Log commands
        LOG = 0xD0,         // 0xDEV {?}
        // 0xE* -> Runner commands
        SLEEP = 0xE0,       // 0xMS 0xMS 0xMS 0xMS
        STOP = 0xEE,        // {str}
        ERROR = 0xEF,       // {str}
        // 0xF* -> Engine commands
        RESET = 0xF0,       // {void}
        REBOOT = 0xFF,      // {void}
    };
    
    struct Runner;
    struct Instruction { 
        OpCode code;                // The type of instruction
        code_addr_t next;
    
        Instruction(OpCode code) : code(code), next(0xFFFF) {}

        virtual code_addr_t run(qb::Runner& runner) {
            return this->next;
        };
        virtual void purge() {
            
        };
    };
}