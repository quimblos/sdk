#include <sstream>
#include <iostream>
#include <iomanip>
#include "hex.h"

using namespace qb;

Bytecode qb::hex_to_bytecode(std::string hex) {
    size_t length = hex.length()/2;
    std::string s2;
    std::istringstream ss (hex);

    // std::cout << "length: " << length << std::endl;
    code_t* bytes = new code_t[length];
    for (size_t i = 0; (ss >> std::setw(2) >> s2); i++) {
        uint16_t u;                       
        std::istringstream ss2 (s2);    
        ss2 >> std::setbase(16) >> u;   
        bytes[i] = (code_t) static_cast<uint8_t>(u & 0xFF);
        // std::cout << "s2: " << s2 << ", u: " << u << ", b: " << unsigned(bytes[i]) << ", bb: " << ((uint16_t) bytes[i]) << std::endl;
    }
    
    return {
        .length = length,
        .bytes = bytes
    };
}

std::string qb::bytecode_to_hex(Bytecode) {
    // TODO
    return "";
}

std::string qb::vector_to_hex(std::vector<code_t> vec) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    std::vector<uint8_t>::const_iterator it;

    for (it = vec.begin(); it != vec.end(); it++) {
        ss << std::setw(2) << static_cast<unsigned>(*it);
    }

    return ss.str();
}
