#pragma once
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include "types.h"

#define HEX(...) qb::vector_to_hex(std::vector<qb::code_t>(__VA_ARGS__))

namespace qb {
    
    struct Bytecode {
        code_addr_t length;
        code_t* bytes;
    };

    Bytecode hex_to_bytecode(std::string hex);
    std::string bytecode_to_hex(Bytecode);
    std::string vector_to_hex(std::vector<code_t> vec);

}
