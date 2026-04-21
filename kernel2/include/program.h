#pragma once
#include <vector>
#include <string>
#include "types.h"
#include "data.h"
#include "instruction.h"

/*
    Program
*/

namespace qb {
    
    /*
        Response
    */

    struct Program;
    namespace program {
        struct res_t {
            uint8_t code;
            Program* program;
            code_addr_t error_addr;
        };
    }

    
    namespace program {
        struct Port {
            std::string name;
            Data* data;
        };
    }
    
    struct Program {
        std::string name;
    
        std::vector<std::string> devices;
        std::vector<Data*> variables;
        std::vector<Instruction*> instructions;
 
        std::vector<program::Port> ports;

        Program(
            std::string name,
            std::vector<Instruction*> instructions,
            std::vector<program::Port> ports
        );
        ~Program();

        // Builds a program from a byte sequence
        static program::res_t make(std::string name, code_t* bytes, code_addr_t code_len);
    };

}