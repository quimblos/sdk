#pragma once
#include "parser.h"

#define HEX(...) qb::vector_to_hex(std::vector<qb::code_t>(__VA_ARGS__))

namespace qb {

    Bytecode hex_to_bytecode(std::string hex);
    std::string bytecode_to_hex(Bytecode);
    std::string vector_to_hex(std::vector<code_t> vec);

}
