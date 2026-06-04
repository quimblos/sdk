#pragma once

#include "ebnf.h"

namespace meta {

    struct res_t {
        enum Code {
            OK = 0x00,
            EBNF_ERROR,
            NO_GRAMMAR_RULE
        } code;
        uint8_t ebnf_code;
        
        std::string header;
        std::string cpp;
    };

    struct Config {
        enum OnError {
            FAIL = 0x00,
            STOP,
            CONTINUE
        } on_error = OnError::CONTINUE;
    };

    res_t generate_cst_parser(std::string langname, std::string ebnf, const Config& config = {});
}