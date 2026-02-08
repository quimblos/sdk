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
        USE_DEVICE = 0x01,           // (...name: const) Sets a value
        // 0x1* -> Register manipulation commands
        SET = 0x11,                // (dev: i8, reg: i8, ...data:i8[]) Sets a value
        // 0x2* -> Flow control commands 
        GOTO = 0x20,               // (goto: i16)
        IF_EQ = 0x21,              // (dev: i8, reg: i8, goto_true: i16, goto_false: i16, ...data: i8[]) Go to a position of the script if the value if equal, or other position if not
        IF_GT = 0x22,              // (dev: i8, reg: i8, goto_true: i16, goto_false: i16, ...data: i8[]) Go to a position of the script if the value if greater than, or other position if not
        IF_GTEQ = 0x23,            // (dev: i8, reg: i8, goto_true: i16, goto_false: i16, ...data: i8[]) Go to a position of the script if the value if equal, or other position if not
        // 0xD* -> Log commands
        LOG = 0xD0,                // (dev: i8, code: i8, ...data: str) Logs something
        // 0xE* -> Runner commands
        SLEEP = 0xE0,               // (data: i32) Waits a specified amount of time (in ms)
        STOP = 0xEE,                // (...data: str) Stops the runner with an ok message
        ERROR = 0xEF,               // (code: i8, ...data: str) Stops the runner with an error
        // 0xF* -> Engine commands
        RESET = 0xF0,              // () Clears the interpreter and resets the peripherals
        REBOOT = 0xFF,             // () Reboots the device
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