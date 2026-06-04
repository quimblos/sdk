#include <sstream>
#include "parser.h"

// #define DEBUG_PARSER
#ifdef DEBUG_PARSER
    #include <iostream>
#endif

//

bool parser::check_literal(const std::string input, uint32_t n, uint32_t i0, const std::string literal) {
    n = n-i0;
    uint32_t ln = literal.size();
    if (ln < n) n = ln;
    uint32_t i = 0;
    for (; i < n; i++) {
#ifdef DEBUG_PARSER
        std::cout << i << " " << input[i+i0] << " <-> " << literal[i] << " = " << (input[i+i0] != literal[i] ? "diff" : "match") << std::endl;
#endif
        if (input[i+i0] != literal[i]) return false;
  };
#ifdef DEBUG_PARSER
    std::cout << i << " <-> " << ln << " = " << (i == ln ? "match" : "diff") << std::endl;
#endif
    return i == ln;
};