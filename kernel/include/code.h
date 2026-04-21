#pragma once
#include "memory.h"

#define HEADER_QUIMBLOS 'q', 'b', 0x00, 0x00

namespace qb {

    /**
     * Command
     */
    
    enum OpCode {
        // 0x0* -> Parser commands
        USE_DEVICE = 0x01,       // {name:str}
        USE_NODE = 0x02,         // {source:any}
        // 0x1* -> Node manipulation commands
        SET = 0x10,           // {target:ref} {source:str}
        HOLD = 0x1A,          // {device:u8}
        RELEASE = 0x1B,       // {device:u8}
        // 0x2* -> Flow control commands 
        GOTO = 0x20,          // {code:u16}
        BRANCH = 0x21,        // {source:any} {true_code:u16} {false_code:u16}
        // 0x3* -> Comparator commands 
        SET_IF_EQ = 0x30,     // {target:ref} {left:any} {right:any} {true:any} {false:any}
        SET_IF_LT = 0x31,     // {target:ref} {left:any} {right:any} {true:any} {false:any}
        SET_IF_GT = 0x32,     // {target:ref} {left:any} {right:any} {true:any} {false:any}
        // 0x4* -> Boolean Arithmetic commands 
        AND = 0x40,           // {target:reg} {source:any}
        OR = 0x41,            // {target:reg} {source:any}
        XOR = 0x42,           // {target:reg} {source:any}
        // 0x5* -> Arithmetic commands 
        ADD = 0x50,           // {target:reg} {source:any}
        SUB = 0x51,           // {target:reg} {source:any}
        MULT = 0x52,          // {target:reg} {source:any}
        DIV = 0x53,           // {target:reg} {source:any}
        MOD = 0x54,           // {target:reg} {source:any}
        POW = 0x55,           // {target:reg} {source:any}
        // 0xE* -> Runner commands
        LOG = 0xE0,           // {source:any}
        SLEEP = 0xEE,         // {time:any}
        RETURN = 0xEF,        // {source:any}
        // 0xF* -> Engine commands
        RESET = 0xF0,         // !
        REBOOT = 0xFF,        // !
    };
    
    struct Runner;
    struct Instruction { 
        OpCode code;                // The type of instruction
        code_addr_t next;
    
        Instruction(OpCode code) : code(code), next(0xFFFF) {}
        virtual ~Instruction() {};

        virtual code_addr_t run(qb::Runner& runner) {
            return this->next;
        };

        virtual void purge() {};
        virtual std::string to_str() const = 0;
    };
}