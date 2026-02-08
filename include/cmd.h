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
    
    enum CmdCode {
        // 0x0* -> Parser commands
        USE_DEVICE = 0x01,
        USE_REG = 0x02,
        // 0x1* -> Register manipulation commands
        SET = 0x11,
        // 0x2* -> Flow control commands 
        GOTO = 0x20,
        IF_EQ = 0x21,
        IF_GT = 0x22,
        IF_GTEQ = 0x23,
        // 0x3* -> Arithmetic commands 
        ADD = 0x30,
        SUB = 0x31,
        MULT = 0x32,
        DIV = 0x33,
        MOD = 0x34,
        // 0xD* -> Log commands
        LOG = 0xD0,
        // 0xE* -> Runner commands
        SLEEP = 0xE0,
        STOP = 0xEE,
        ERROR = 0xEF,
        // 0xF* -> Engine commands
        RESET = 0xF0,
        REBOOT = 0xFF,
    };
    
    struct Cmd { 
        CmdCode code;                 // The type of command
        qb::Device* device = 0;
        uint8_t reg_i = 0;                   
        Data* value = nullptr;                       // (optional) A pointer to the constant value on the engine
        qb::code_addr_t next = 0;                    // A pointer to the next command - if 0, it's the end
        qb::code_addr_t next_false = 0;              // (optional) A pointer to the next command, if 0 the condition fails
    };
    
}