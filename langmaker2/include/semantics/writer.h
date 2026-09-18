#pragma once

#include "syntax/parser.h"
#include "semantics/parser.h"

namespace semantics {

    namespace writer {

        struct res_t {
            const enum Code {
                OK = 0x00,
                SEMANTICS_ERROR,
                NO_GRAMMAR_RULE
            } code;
            const uint8_t semantics_error_code;
            
            const semantics::Schema* schema;
            const std::string header;
            const std::string cpp;
        };
    
        struct Config {
            const enum OnError {
                FAIL = 0x00,
                STOP,
                CONTINUE
            } on_error = OnError::CONTINUE;
            const std::string header_path = "";
        };
    
    }

    writer::res_t write_parser(const std::string& langname, const syntax::Schema& syntax, std::string def, const writer::Config& config = {});

}