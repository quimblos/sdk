#pragma once
#include "data.h"
#include "cmd.h"
#include "device.h"
#include "script.h"
#include "engine.h"

#define HEX(...) qb::parser::vectorToHex(std::vector<qb::code_t>(__VA_ARGS__));

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
            bool ok;
            std::string message;
            const qb::Script* script;
        };

        res_t dump(qb::Script& script);
        res_t parse(qb::Engine& engine, std::string name, std::string hex);

        qb::Bytecode hexToBytecode(std::string hex);
        std::string bytecodeToHex(qb::Bytecode hex);

        std::string vectorToHex(std::vector<qb::code_t> vec);
    }

}