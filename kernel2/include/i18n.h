#pragma once

#define LOG(X) std::cout << #X " = " << X << std::endl;

namespace qb {

    namespace i18n {

        namespace error {

            const std::map<uint8_t, std::string> PARSER = {
                { qb::parser::res_t::Code::OK,
                    "[parser] OK" },
                { qb::parser::res_t::Code::UNEXPECTED_EOF,
                    "[parser] Unexpected EOF" },
                { qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE,
                    "[parser] Creating custom primitive types is not allowed" },
                { qb::parser::res_t::Code::CONST_ASSIGNMENT,
                    "[parser] Assignment to constant block is not allowed" },
                { qb::parser::res_t::Code::ENTITY_KIND,
                    "[parser] Invalid kind of entity" },
                { qb::parser::res_t::Code::COMPARE_OP,
                    "[parser] Invalid comparison operation" },
                { qb::parser::res_t::Code::MATH_OP,
                    "[parser] Invalid arithmetic operation" },
            };

        }

    }

}