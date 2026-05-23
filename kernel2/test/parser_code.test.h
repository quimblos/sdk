#pragma once
#include "parser.h"
#include "i18n.h"

#define TEST_CODE_OK(CODE...) \
    qb::byte_t bytecode[] = CODE; \
    auto res = qb::parser::code(bytecode, sizeof(bytecode)/sizeof(qb::byte_t)); \
    if (res.code != 0) { \
        std::cout << qb::i18n::parser.at(res.code) << std::endl; \
        qb_fail() \
    } \
    std::cout << res.out.code->to_str(); \
    qb::Code* code = res.out.code;

qb_suite(test_parser_code, "parser_code", {

    qb_describe("Code", {
    
        qb_test("Blocks", {
            TEST_CODE_OK({
                HEADER_QUIMBLOS,
                qb::OpCode::USE_DRIVER, 3, 'l', 'e', 'd',
                qb::OpCode::USE_DRIVER, 5, 'c', 'l', 'o', 'c', 'k',
            });
            qb_assert(code->drivers.size() == 2)
            qb_assert(code->drivers[0] == "led")
            qb_assert(code->drivers[1] == "clock")
            delete code;
        })

        qb_test("Args", {
            TEST_CODE_OK({
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_ARG, B_TYPE_U8,
                qb::OpCode::ADD_ARG, B_TYPE_STR,
            });
            qb_assert(code->args.size() == 2)
            qb_assert(code->args[0] == B_TYPE_U8)
            qb_assert(code->args[1] == B_TYPE_STR)
            delete code;
        })

        qb_test("Vars", {
            TEST_CODE_OK({
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_VAR, B_TYPE_F32,
                qb::OpCode::ADD_VAR, B_TYPE_REF,
            });
            qb_assert(code->vars.size() == 2)
            qb_assert(code->vars[0] == B_TYPE_F32)
            qb_assert(code->vars[1] == B_TYPE_REF)
            delete code;
        })

        qb_test("Instructions", {
            TEST_CODE_OK({
                HEADER_QUIMBLOS,
                qb::OpCode::SLEEP, BLOCK_METHOD, 0x01,
                qb::OpCode::GOTO, 0x00, 0x00,
            });
            qb_assert(code->instructions.size() == 2)
            qb_assert(code->instructions[0]->type == qb::OpCode::SLEEP)
            qb_assert(code->instructions[1]->type == qb::OpCode::GOTO)
            delete code;
        })

    })
})