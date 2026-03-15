#pragma once
#include "memory.h"

#define HEADER_QUIMBLOS 'q', 'b', 0x00, 0x00

// USE_DEVICE 0x00
// USE_NODE PTR 0x00 0x00 0x00 0x00
//
// SET SH_REF_NODE 0xFF 0x00 UINT8 0x01
// ADD SH_NODE_NODE 0xFF 0x00 UINT8 0x01
// IF_GTEQ SH_NODE_REF 0xFF 0x00 PTR_SHORT 0x00 0x01
//     GOTO 0xFFFF                                  
//     GOTO 0x0002

// #use ledstrip
// var it = ledstrip.buffer
//
// @loop
//     *it = 1
//     it += 1
//     if it < ledstrip.length @loop else @end


namespace qb {

    /**
     * Command
     */
    
    enum OpCode {
        // 0x0* -> Parser commands
        USE_DEVICE = 0x01,       // <str>
        USE_NODE = 0x02,         // TYPE[?]
        USE_NODE_ALIASED = 0x03, // TYPE[?] <str>
        // 0x1* -> Register manipulation commands
        SET = 0x10,           // TARGET <any>
        HOLD = 0x1A,          // DEV
        RELEASE = 0x1B,       // DEV
        // 0x2* -> Flow control commands 
        GOTO = 0x20,          // CODE[2]
        IF_EQ = 0x21,         // TARGET <any> CODE[2] CODE_[2]
        IF_GT = 0x23,         // TARGET <any> CODE[2] CODE_[2]
        IF_GTEQ = 0x25,       // TARGET <any> CODE[2] CODE_[2]
        // 0x3* -> Arithmetic commands 
        ADD = 0x30,           // TARGET <any>
        SUB = 0x31,           // TARGET <any>
        MULT = 0x32,          // TARGET <any>
        DIV = 0x33,           // TARGET <any>
        MOD = 0x34,           // TARGET <any>
        POW = 0x35,           // TARGET <any>
        FLOOR = 0x36,         // TARGET <any>
        CEIL = 0x37,          // TARGET <any>
        // 0xD* -> Log commands
        LOG = 0xD0,           // DEV <str>
        // 0xE* -> Runner commands
        SLEEP = 0xE0,         // MS[4]
        RETURN = 0xEE,        // <any>
        // 0xF* -> Engine commands
        RESET = 0xF0,         // !
        REBOOT = 0xFF,        // !
    };
    
    // The type of data binding of an instruction
    enum OpBind {
        NODE_NODE = 0x00,           // Target: Node, Source: Node
        NODE_REF = 0x01,            // Target: Node, Source: Referenced Node
        REF_NODE = 0x02,            // Target: Referenced Node, Source: Node
        REF_REF = 0x03,             // Target: Referenced Node, Source: Referenced Node
        
        SHORT_NODE_NODE = 0x10,     // Target: Node, Source: Node
        SHORT_NODE_REF = 0x11,      // Target: Node, Source: Referenced Node
        SHORT_REF_NODE = 0x12,      // Target: Referenced Node, Source: Node
        SHORT_REF_REF = 0x13        // Target: Referenced Node, Source: Referenced Node
    };

    struct Runner;
    struct Instruction { 
        OpCode code;                // The type of instruction
        code_addr_t next;
    
        Instruction(OpCode code) : code(code), next(0xFFFF) {}

        virtual code_addr_t run(qb::Runner& runner) {
            return this->next;
        };

        virtual void purge() {};
        virtual std::string to_str() const = 0;
    };
}