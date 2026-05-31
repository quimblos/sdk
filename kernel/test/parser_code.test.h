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
    
        qb_test("Drivers", {
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

        qb_test("Types", {
            TEST_CODE_OK({
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_TYPE, qb::TypeKind::VECTOR, false, B_TYPE_U8,
                qb::OpCode::ADD_TYPE, qb::TypeKind::STRUCT, 3, false, B_TYPE_BOOL, false, 0, false, B_TYPE_U8,
            });
            qb_assert(code->types.size() == 2)
            qb_assert(code->types[0].add.kind == qb::TypeKind::VECTOR)
            qb_assert(code->types[0].add.children.size() == 1)
            qb_assert(code->types[0].add.children[0].use == B_TYPE_U8)
            qb_assert(code->types[1].add.kind == qb::TypeKind::STRUCT)
            qb_assert(code->types[1].add.children.size() == 3)
            qb_assert(code->types[1].add.children[0].use == B_TYPE_BOOL)
            qb_assert(code->types[1].add.children[1].use == 0)
            qb_assert(code->types[1].add.children[2].use == B_TYPE_U8)
            delete code;
        })

        qb_test("Consts", {
            TEST_CODE_OK({
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_CONST, B_TYPE_U16, 0x12, 0x34,
                qb::OpCode::ADD_CONST, B_TYPE_STR, 0x00, 0x04, 't', 'e', 's', 't',
                qb::OpCode::ADD_TYPE, qb::TypeKind::VECTOR, false, B_TYPE_U16,
                qb::OpCode::ADD_CONST, 0, 0x00, 0x09, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03,

            });
            qb_assert(code->consts.size() == 3)
            qb_assert(code->consts[0].tdx == B_TYPE_U16)
            qb_assert(code->consts[0].length == 2)
            qb_assert(code->consts[0].bytes[0] == 0x12)
            qb_assert(code->consts[0].bytes[1] == 0x34)
            qb_assert(code->consts[1].tdx == B_TYPE_STR)
            qb_assert(code->consts[1].length == 6)
            qb_assert(code->consts[1].bytes[0] == 0x00)
            qb_assert(code->consts[1].bytes[1] == 0x04)
            qb_assert(code->consts[1].bytes[2] == 't')
            qb_assert(code->consts[1].bytes[3] == 'e')
            qb_assert(code->consts[1].bytes[4] == 's')
            qb_assert(code->consts[1].bytes[5] == 't')
            qb_assert(code->consts[2].length == 11)
            qb_assert(code->consts[2].bytes[0] == 0x00)
            qb_assert(code->consts[2].bytes[1] == 0x09)
            qb_assert(code->consts[2].bytes[2] == 0x01)
            qb_assert(code->consts[2].bytes[3] == 0x00)
            qb_assert(code->consts[2].bytes[4] == 0x03)
            qb_assert(code->consts[2].bytes[5] == 0x00)
            qb_assert(code->consts[2].bytes[6] == 0x01)
            qb_assert(code->consts[2].bytes[7] == 0x00)
            qb_assert(code->consts[2].bytes[8] == 0x02)
            qb_assert(code->consts[2].bytes[9] == 0x00)
            qb_assert(code->consts[2].bytes[10] == 0x03)
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
                qb::OpCode::SLEEP, BLOCK_CONTEXT, 0x01,
                qb::OpCode::GOTO, 0x00, 0x00,
            });
            qb_assert(code->instructions.size() == 2)
            qb_assert(code->instructions[0]->type == qb::OpCode::SLEEP)
            qb_assert(code->instructions[1]->type == qb::OpCode::GOTO)
            delete code;
        })

    })
})