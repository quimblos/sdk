#pragma once

#include "syntax/parser.h"

namespace syntax {

    namespace writer {

        struct res_t {
            const enum Code {
                OK = 0x00,
                SYNTAX_ERROR,
                NO_GRAMMAR_RULE
            } code;
            const uint8_t syntax_error_code;
            
            const syntax::Schema* schema;
            const std::string header;
            const std::string cpp;
        };
    
        struct Config {
            const enum OnError {
                FAIL = 0x00,
                STOP,
                CONTINUE
            } on_error = OnError::CONTINUE;
            const std::string header_path;
        };
    
    }

    writer::res_t write_parser(std::string langname, std::string def, const writer::Config& config = {});

}