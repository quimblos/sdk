#pragma once
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include "config.h"

#define HEX(...) qb::vector_to_hex(std::vector<qb::byte_t>(__VA_ARGS__))

namespace qb {
    
    struct Bytecode {
        code_addr_t length;
        byte_t* bytes;
    };

    Bytecode hex_to_bytecode(std::string hex);
    std::string bytecode_to_hex(Bytecode);
    std::string vector_to_hex(std::vector<byte_t> vec);

}
