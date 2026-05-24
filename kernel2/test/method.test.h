#pragma once
#include "method.h"

qb_suite(test_method, "method", {

    qb_describe("Method", {
    
        qb_test("Vars", {
            
            qb::byte_t code[] = {
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_ARG, B_TYPE_U8,
                qb::OpCode::ADD_VAR, B_TYPE_STR
            };
            auto res = qb::parser::code(code, sizeof(code)/sizeof(qb::byte_t));
            if (res.code != 0) {
                std::cout << qb::i18n::parser.at(res.code) << std::endl;
                qb_fail()
            }
            std::cout << res.out.code->to_str() << std::endl;

            auto method = qb::Method(nullptr, res.out.code);

            qb_assert(method.block.data.pos.size() == 3)

            delete res.out.code;
        })

    })
})