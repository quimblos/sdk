#pragma once
#include "parser.h"
#include "i18n.h"

#define TEST_INSTRUCTION_OK(TYPE, CODE...) \
    qb::byte_t code[] = CODE; \
    auto res = qb::parser::instruction(code, sizeof(code)/sizeof(qb::byte_t)); \
    if (res.code != 0) { \
        std::cout << qb::i18n::parser.at(res.code) << std::endl; \
        qb_fail() \
    } \
    std::cout << res.out.instruction->to_str() << std::endl; \
    TYPE instruction = *(TYPE*) res.out.instruction; \
    delete res.out.instruction;

#define TEST_INSTRUCTION_FAIL(EXPECTED_ERROR, CODE...) \
    qb::byte_t code[] = CODE; \
    auto res = qb::parser::instruction(code, sizeof(code)/sizeof(qb::byte_t)); \
    auto res_code = res.code; \
    LOG(res_code) \
    delete res.out.instruction; \
    qb_assert(res_code == EXPECTED_ERROR)

qb_suite(test_parser, "parser", {

    qb_describe("Instruction: Node", {
    
        qb_test("USE_DRIVER", {
            TEST_INSTRUCTION_OK(qb::instruction::UseDriver, {
                qb::OpCode::USE_DRIVER,
                4,                  // str length
                't', 'e', 's', 't'  // str
            })
            qb_assert(instruction.type == qb::OpCode::USE_DRIVER)
            qb_assert(instruction.name == "test")
        })
    })

    qb_describe("Instruction: Parser (Type)", {

        qb_test("ADD_TYPE:VOID (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VOID       // kind
            })
        })
    
        qb_test("ADD_TYPE:BOOL (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::BOOL,      // kind
            })
        })
    
        qb_test("ADD_TYPE:INT (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::INT,      // kind
            })
        })
    
        qb_test("ADD_TYPE:FLOAT (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FLOAT,    // kind
            })
        })
    
        qb_test("ADD_TYPE:STRING (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::STRING,   // kind
            })
        })
    
        qb_test("ADD_TYPE:REF (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::REF,   // kind
            })
        })
        qb_test("ADD_TYPE:REF_SLICE (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::REF_SLICE,   // kind
            })
        })
        qb_test("ADD_TYPE:VECTOR <bool>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,   // kind
                false,                  // use
                B_TYPE_BOOL
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_BOOL)
        })
        qb_test("ADD_TYPE:VECTOR <u8>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,   // kind
                false,                  // use
                B_TYPE_U8
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:VECTOR <f32>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,   // kind
                false,                  // use
                B_TYPE_F32
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_F32)
        })
        qb_test("ADD_TYPE:VECTOR <str>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,   // kind
                false,                  // use
                B_TYPE_STR
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_STR)
        })
        qb_test("ADD_TYPE:VECTOR <ref>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,   // kind
                false,                  // use
                B_TYPE_REF
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_REF)
        })
        qb_test("ADD_TYPE:MAP <bool>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,   // kind
                false,                  // use
                B_TYPE_BOOL
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_BOOL)
        })
        qb_test("ADD_TYPE:MAP <u8>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,   // kind
                false,                  // use
                B_TYPE_U8
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:MAP <f32>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,   // kind
                false,                  // use
                B_TYPE_F32
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_F32)
        })
        qb_test("ADD_TYPE:MAP <str>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,   // kind
                false,                  // use
                B_TYPE_STR
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_STR)
        })
        qb_test("ADD_TYPE:MAP <ref>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,   // kind
                false,                  // use
                B_TYPE_REF
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_REF)
        })
        qb_test("ADD_TYPE:EVENT <bool>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,   // kind
                false,                  // use
                B_TYPE_BOOL
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_BOOL)
        })
        qb_test("ADD_TYPE:EVENT <u8>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,   // kind
                false,                  // use
                B_TYPE_U8
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:EVENT <f32>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,   // kind
                false,                  // use
                B_TYPE_F32
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_F32)
        })
        qb_test("ADD_TYPE:EVENT <str>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,   // kind
                false,                  // use
                B_TYPE_STR
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_STR)
        })
        qb_test("ADD_TYPE:EVENT <ref>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,   // kind
                false,                  // use
                B_TYPE_REF
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_REF)
        })
        qb_test("ADD_TYPE:FN <bool>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FN,   // kind
                false,                  // use
                B_TYPE_BOOL
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_BOOL)
        })
        qb_test("ADD_TYPE:FN <u8>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FN,   // kind
                false,                  // use
                B_TYPE_U8
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:FN <f32>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FN,   // kind
                false,                  // use
                B_TYPE_F32
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_F32)
        })
        qb_test("ADD_TYPE:FN <str>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FN,   // kind
                false,                  // use
                B_TYPE_STR
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_STR)
        })
        qb_test("ADD_TYPE:FN <ref>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FN,   // kind
                false,                  // use
                B_TYPE_REF
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_REF)
        })
        qb_test("ADD_TYPE:STRUCT", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::STRUCT,   // kind
                5,
                false, B_TYPE_BOOL,
                false, B_TYPE_U8,
                false, B_TYPE_F32,
                false, B_TYPE_STR,
                false, B_TYPE_REF,
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::STRUCT)
            qb_assert(instruction.type_def.add.children.size() == 5)
            qb_assert(instruction.type_def.add.children[0].use == B_TYPE_BOOL)
            qb_assert(instruction.type_def.add.children[1].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[2].use == B_TYPE_F32)
            qb_assert(instruction.type_def.add.children[3].use == B_TYPE_STR)
            qb_assert(instruction.type_def.add.children[4].use == B_TYPE_REF)
        })
        qb_test("ADD_TYPE:VECTOR <vector<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,       // kind
                true, qb::TypeKind::VECTOR, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:VECTOR <map<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,       // kind
                true, qb::TypeKind::MAP, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:VECTOR <event<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,       // kind
                true, qb::TypeKind::EVENT, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:VECTOR <fn<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,       // kind
                true, qb::TypeKind::FN, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:VECTOR <struct>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VECTOR,       // kind
                true, qb::TypeKind::STRUCT, // child
                    3,
                    false, B_TYPE_U8,       // subchild
                    false, B_TYPE_STR,      // subchild
                    false, B_TYPE_REF,      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::STRUCT)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 3)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[0].add.children[1].use == B_TYPE_STR)
            qb_assert(instruction.type_def.add.children[0].add.children[2].use == B_TYPE_REF)
        })
        qb_test("ADD_TYPE:MAP <vector<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,       // kind
                true, qb::TypeKind::VECTOR, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:MAP <map<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,       // kind
                true, qb::TypeKind::MAP, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:MAP <event<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,       // kind
                true, qb::TypeKind::EVENT, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:MAP <fn<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,       // kind
                true, qb::TypeKind::FN, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:MAP <struct>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::MAP,       // kind
                true, qb::TypeKind::STRUCT, // child
                    3,
                    false, B_TYPE_U8,       // subchild
                    false, B_TYPE_STR,      // subchild
                    false, B_TYPE_REF,      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::STRUCT)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 3)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[0].add.children[1].use == B_TYPE_STR)
            qb_assert(instruction.type_def.add.children[0].add.children[2].use == B_TYPE_REF)
        })
        qb_test("ADD_TYPE:EVENT <vector<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,       // kind
                true, qb::TypeKind::VECTOR, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:EVENT <map<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,       // kind
                true, qb::TypeKind::MAP, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:EVENT <event<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,       // kind
                true, qb::TypeKind::EVENT, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:EVENT <fn<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,       // kind
                true, qb::TypeKind::FN, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:EVENT <struct>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,       // kind
                true, qb::TypeKind::STRUCT, // child
                    3,
                    false, B_TYPE_U8,       // subchild
                    false, B_TYPE_STR,      // subchild
                    false, B_TYPE_REF,      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::STRUCT)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 3)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[0].add.children[1].use == B_TYPE_STR)
            qb_assert(instruction.type_def.add.children[0].add.children[2].use == B_TYPE_REF)
        })
        qb_test("ADD_TYPE:FN <vector<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FN,       // kind
                true, qb::TypeKind::VECTOR, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:FN <map<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FN,       // kind
                true, qb::TypeKind::MAP, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:FN <event<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FN,       // kind
                true, qb::TypeKind::EVENT, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:FN <fn<u8>>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FN,       // kind
                true, qb::TypeKind::FN, // child
                    false, B_TYPE_U8      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
        })
        qb_test("ADD_TYPE:EVENT <struct>", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::EVENT,       // kind
                true, qb::TypeKind::STRUCT, // child
                    3,
                    false, B_TYPE_U8,       // subchild
                    false, B_TYPE_STR,      // subchild
                    false, B_TYPE_REF,      // subchild
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::STRUCT)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 3)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[0].add.children[1].use == B_TYPE_STR)
            qb_assert(instruction.type_def.add.children[0].add.children[2].use == B_TYPE_REF)
        })
        qb_test("ADD_TYPE:STRUCT (complex)", {
            TEST_INSTRUCTION_OK(qb::instruction::AddType, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::STRUCT,       // kind
                5,
                true, qb::TypeKind::VECTOR, 
                    false, B_TYPE_U8,
                true, qb::TypeKind::MAP, 
                    false, B_TYPE_U8,
                true, qb::TypeKind::EVENT, 
                    false, B_TYPE_U8,
                true, qb::TypeKind::FN, 
                    false, B_TYPE_U8,
                true, qb::TypeKind::STRUCT, 
                    3,
                    false, B_TYPE_U8,
                    false, B_TYPE_STR,
                    false, B_TYPE_REF,
            })
            qb_assert(instruction.type == qb::OpCode::ADD_TYPE)
            qb_assert(instruction.type_def.add.kind == qb::TypeKind::STRUCT)
            qb_assert(instruction.type_def.add.children.size() == 5)
            qb_assert(instruction.type_def.add.children[0].add.kind == qb::TypeKind::VECTOR)
            qb_assert(instruction.type_def.add.children[0].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[0].add.children[0].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[1].add.kind == qb::TypeKind::MAP)
            qb_assert(instruction.type_def.add.children[1].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[1].add.children[0].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[2].add.kind == qb::TypeKind::EVENT)
            qb_assert(instruction.type_def.add.children[2].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[2].add.children[0].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[3].add.kind == qb::TypeKind::FN)
            qb_assert(instruction.type_def.add.children[3].add.children.size() == 1)
            qb_assert(instruction.type_def.add.children[3].add.children[0].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[4].add.kind == qb::TypeKind::STRUCT)
            qb_assert(instruction.type_def.add.children[4].add.children.size() == 3)
            qb_assert(instruction.type_def.add.children[4].add.children[0].use == B_TYPE_U8)
            qb_assert(instruction.type_def.add.children[4].add.children[1].use == B_TYPE_STR)
            qb_assert(instruction.type_def.add.children[4].add.children[2].use == B_TYPE_REF)
        })
    })

    qb_describe("Instruction: Parser (Const)", {

        qb_test("ADD_CONST:VOID", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_VOID,          // type
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_VOID)
            qb_assert(instruction.length == 0)
            qb_assert(instruction.bytes == nullptr)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:NULL", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_NULL,          // type
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_NULL)
            qb_assert(instruction.length == 0)
            qb_assert(instruction.bytes == nullptr)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:BOOL", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_BOOL,          // type
                true
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_BOOL)
            qb_assert(instruction.length == 1)
            qb_assert(instruction.bytes[0] == true)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:U8", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_U8,          // type
                123
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_U8)
            qb_assert(instruction.length == 1)
            qb_assert(instruction.bytes[0] == 123)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:I8", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_I8,          // type
                (qb::byte_t) -123
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_I8)
            qb_assert(instruction.length == 1)
            qb_assert(((int8_t*)instruction.bytes)[0] == -123)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:U16", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_U16,          // type
                0x12, 0x34
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_U16)
            qb_assert(instruction.length == 2)
            qb_assert(instruction.bytes[0] == 0x12)
            qb_assert(instruction.bytes[1] == 0x34)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:I16", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_I16,          // type
                0x12, 0x34
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_I16)
            qb_assert(instruction.length == 2)
            qb_assert(instruction.bytes[0] == 0x12)
            qb_assert(instruction.bytes[1] == 0x34)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:U32", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_U32,          // type
                0x12, 0x34, 0x56, 0x78
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_U32)
            qb_assert(instruction.length == 4)
            qb_assert(instruction.bytes[0] == 0x12)
            qb_assert(instruction.bytes[1] == 0x34)
            qb_assert(instruction.bytes[2] == 0x56)
            qb_assert(instruction.bytes[3] == 0x78)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:I32", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_I32,          // type
                0x12, 0x34, 0x56, 0x78
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_I32)
            qb_assert(instruction.length == 4)
            qb_assert(instruction.bytes[0] == 0x12)
            qb_assert(instruction.bytes[1] == 0x34)
            qb_assert(instruction.bytes[2] == 0x56)
            qb_assert(instruction.bytes[3] == 0x78)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:F32", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_F32,          // type
                0x12, 0x34, 0x56, 0x78
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_F32)
            qb_assert(instruction.length == 4)
            qb_assert(instruction.bytes[0] == 0x12)
            qb_assert(instruction.bytes[1] == 0x34)
            qb_assert(instruction.bytes[2] == 0x56)
            qb_assert(instruction.bytes[3] == 0x78)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:STR", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_STR,          // type
                0x00, 0x04,          // length
                't', 'e', 's', 't'
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_STR)
            qb_assert(instruction.length == 6)
            qb_assert(instruction.bytes[0] == 0x00)
            qb_assert(instruction.bytes[1] == 0x04)
            qb_assert(instruction.bytes[2] == 't')
            qb_assert(instruction.bytes[3] == 'e')
            qb_assert(instruction.bytes[4] == 's')
            qb_assert(instruction.bytes[5] == 't')
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:REF", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_REF,          // type
                BLOCK_ENGINE,
                PORT_CONST_TRUE
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_REF)
            qb_assert(instruction.length == 2)
            qb_assert(instruction.bytes[0] == BLOCK_ENGINE)
            qb_assert(instruction.bytes[1] == PORT_CONST_TRUE)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:REF_SLICE", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                B_TYPE_REF_SLICE,          // type
                BLOCK_ENGINE,
                PORT_CONST_TRUE,
                2,
                0x12, 0x34, 0x56, 0x78,
                0xAA, 0xBB, 0xCC, 0xDD,
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == B_TYPE_REF_SLICE)
            qb_assert(instruction.length == 11)
            qb_assert(instruction.bytes[0] == BLOCK_ENGINE)
            qb_assert(instruction.bytes[1] == PORT_CONST_TRUE)
            qb_assert(instruction.bytes[2] == 2)
            qb_assert(instruction.bytes[3] == 0x12)
            qb_assert(instruction.bytes[4] == 0x34)
            qb_assert(instruction.bytes[5] == 0x56)
            qb_assert(instruction.bytes[6] == 0x78)
            qb_assert(instruction.bytes[7] == 0xAA)
            qb_assert(instruction.bytes[8] == 0xBB)
            qb_assert(instruction.bytes[9] == 0xCC)
            qb_assert(instruction.bytes[10] == 0xDD)
            delete[] instruction.bytes;
        })

        qb_test("ADD_CONST:CUSTOM", {
            TEST_INSTRUCTION_OK(qb::instruction::AddConst, {
                qb::OpCode::ADD_CONST,
                0,          // type
                0x00, 0x06, // length
                0x12, 0x34, // item 0
                0x56, 0x78, // item 1
                0x9A, 0xBC, // item 2
            })
            qb_assert(instruction.type == qb::OpCode::ADD_CONST)
            qb_assert(instruction.tdx == 0)
            qb_assert(instruction.length == 8)
            qb_assert(instruction.bytes[0] == 0x00)
            qb_assert(instruction.bytes[1] == 0x06)
            qb_assert(instruction.bytes[2] == 0x12)
            qb_assert(instruction.bytes[3] == 0x34)
            qb_assert(instruction.bytes[4] == 0x56)
            qb_assert(instruction.bytes[5] == 0x78)
            qb_assert(instruction.bytes[6] == 0x9A)
            qb_assert(instruction.bytes[7] == 0xBC)
            delete[] instruction.bytes;
        })
    })

    qb_describe("Instruction: Parser (Arg/Var)", {

        qb_test("ADD_ARG", {
            TEST_INSTRUCTION_OK(qb::instruction::AddVar, {
                qb::OpCode::ADD_ARG,
                B_TYPE_U8,          // type
            })
            qb_assert(instruction.type == qb::OpCode::ADD_ARG)
            qb_assert(instruction.tdx == B_TYPE_U8)
        })

        qb_test("ADD_VAR", {
            TEST_INSTRUCTION_OK(qb::instruction::AddVar, {
                qb::OpCode::ADD_VAR,
                B_TYPE_U8,          // type
            })
            qb_assert(instruction.type == qb::OpCode::ADD_VAR)
            qb_assert(instruction.tdx == B_TYPE_U8)
        })

    })

    qb_describe("Instruction: Memory Manipulation", {
    
        qb_test("SET:const (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CONST_ASSIGNMENT, {
                qb::OpCode::SET,
                0b00000011,                 // flags
                BLOCK_THREAD, 0x00,   // target
                BLOCK_THREAD, 0x01,   // source
            })
        })
    
        qb_test("SET", {
            TEST_INSTRUCTION_OK(qb::instruction::Set, {
                qb::OpCode::SET,
                0b00000010,                 // flags
                BLOCK_CONTEXT, 0x00,         // target
                BLOCK_THREAD, 0x01,   // source
            })
            qb_assert(instruction.type == qb::OpCode::SET)
            qb_assert(instruction.flags.deref_target == false)
            qb_assert(instruction.flags.deref_source == true)
            qb_assert(instruction.target.block == BLOCK_CONTEXT)
            qb_assert(instruction.target.port == 0x00)
            qb_assert(instruction.source.block == BLOCK_THREAD)
            qb_assert(instruction.source.port == 0x01)
        })
    
        qb_test("HOLD:node", {
            TEST_INSTRUCTION_OK(qb::instruction::Hold, {
                qb::OpCode::HOLD,
                4,                                  // str length
                't', 'e', 's', 't'                  // driver
            })
            qb_assert(instruction.type == qb::OpCode::HOLD)
            qb_assert(instruction.driver == "test")
        })

        qb_test("HOLD:node", {
            TEST_INSTRUCTION_OK(qb::instruction::Hold, {
                qb::OpCode::HOLD,
                4,                                  // str length
                't', 'e', 's', 't'                  // driver
            })
            qb_assert(instruction.type == qb::OpCode::HOLD)
            qb_assert(instruction.driver == "test")
        })
    
        qb_test("RELEASE:node", {
            TEST_INSTRUCTION_OK(qb::instruction::Release, {
                qb::OpCode::RELEASE,
                4,                                  // str length
                't', 'e', 's', 't'                  // driver
            })
            qb_assert(instruction.type == qb::OpCode::RELEASE)
            qb_assert(instruction.driver == "test")
        })

        qb_test("RELEASE:node", {
            TEST_INSTRUCTION_OK(qb::instruction::Release, {
                qb::OpCode::RELEASE,
                4,                                  // str length
                't', 'e', 's', 't'                  // driver
            })
            qb_assert(instruction.type == qb::OpCode::RELEASE)
            qb_assert(instruction.driver == "test")
        })
    })

    qb_describe("Instruction: Flow Control", {

        qb_test("GOTO", {
            TEST_INSTRUCTION_OK(qb::instruction::Goto, {
                qb::OpCode::GOTO,
                0x12, 0x34      // addr
            })
            qb_assert(instruction.type == qb::OpCode::GOTO)
            qb_assert(instruction.addr == 0x1234)
        })

        qb_test("IF", {
            TEST_INSTRUCTION_OK(qb::instruction::If, {
                qb::OpCode::IF,
                0b00000001,                 // flags
                BLOCK_THREAD, 0x02,   // source
                0x12, 0x34,                 // addr true
                0x56, 0x78                  // addr false
            })
            qb_assert(instruction.type == qb::OpCode::IF)
            qb_assert(instruction.flags.deref_source == true)
            qb_assert(instruction.source.block == BLOCK_THREAD)
            qb_assert(instruction.source.port == 0x02)
            qb_assert(instruction.addr_true == 0x1234)
            qb_assert(instruction.addr_false == 0x5678)
        })
    })

    qb_describe("Instruction: Conditional data manipulation", {

        qb_test("SET_IF:const (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CONST_ASSIGNMENT, {
                qb::OpCode::SET_IF,
                0b00011111,                 // flags
                BLOCK_THREAD, 0x02,         // target
                BLOCK_CONTEXT, 0x01,         // left
                BLOCK_CONTEXT, 0x02,         // right
            })
        })

        qb_test("SET_IF:implicit-true-false", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b00001111,                 // flags
                BLOCK_CONTEXT, 0x01,         // target
                BLOCK_CONTEXT, 0x02,         // left
                BLOCK_CONTEXT, 0x03,         // right
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == false)
            qb_assert(instruction.flags.has_false == false)
            qb_assert(instruction.flags.op == qb::instruction::CompareOp::EQ)
            qb_assert(instruction.target.block == BLOCK_CONTEXT)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_CONTEXT)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_CONTEXT)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_ENGINE)
            qb_assert(instruction.data_true.port == PORT_CONST_TRUE)
            qb_assert(instruction.data_false.block == BLOCK_ENGINE)
            qb_assert(instruction.data_false.port == PORT_CONST_FALSE)
        })

        qb_test("SET_IF:implicit-false", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b00011111,                 // flags
                BLOCK_CONTEXT, 0x01,         // target
                BLOCK_CONTEXT, 0x02,         // left
                BLOCK_CONTEXT, 0x03,         // right
                BLOCK_CONTEXT, 0x04          // data true
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == true)
            qb_assert(instruction.flags.has_false == false)
            qb_assert(instruction.flags.op == qb::instruction::CompareOp::EQ)
            qb_assert(instruction.target.block == BLOCK_CONTEXT)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_CONTEXT)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_CONTEXT)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_CONTEXT)
            qb_assert(instruction.data_true.port == 0x04)
            qb_assert(instruction.data_false.block == BLOCK_ENGINE)
            qb_assert(instruction.data_false.port == PORT_CONST_FALSE)
        })

        qb_test("SET_IF:implicit-true", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b00101111,                 // flags
                BLOCK_CONTEXT, 0x01,         // target
                BLOCK_CONTEXT, 0x02,         // left
                BLOCK_CONTEXT, 0x03,         // right
                BLOCK_CONTEXT, 0x04          // data false
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == false)
            qb_assert(instruction.flags.has_false == true)
            qb_assert(instruction.flags.op == qb::instruction::CompareOp::EQ)
            qb_assert(instruction.target.block == BLOCK_CONTEXT)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_CONTEXT)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_CONTEXT)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_ENGINE)
            qb_assert(instruction.data_true.port == PORT_CONST_TRUE)
            qb_assert(instruction.data_false.block == BLOCK_CONTEXT)
            qb_assert(instruction.data_false.port == 0x04)
        })

        qb_test("SET_IF", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b00111111,                 // flags
                BLOCK_CONTEXT, 0x01,         // target
                BLOCK_CONTEXT, 0x02,         // left
                BLOCK_CONTEXT, 0x03,         // right
                BLOCK_CONTEXT, 0x04,          // data true
                BLOCK_CONTEXT, 0x05          // data false
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == true)
            qb_assert(instruction.flags.has_false == true)
            qb_assert(instruction.flags.op == qb::instruction::CompareOp::EQ)
            qb_assert(instruction.target.block == BLOCK_CONTEXT)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_CONTEXT)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_CONTEXT)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_CONTEXT)
            qb_assert(instruction.data_true.port == 0x04)
            qb_assert(instruction.data_false.block == BLOCK_CONTEXT)
            qb_assert(instruction.data_false.port == 0x05)
        })

        qb_test("SET_IF:lt", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b10111111,                 // flags
                BLOCK_CONTEXT, 0x01,         // target
                BLOCK_CONTEXT, 0x02,         // left
                BLOCK_CONTEXT, 0x03,         // right
                BLOCK_CONTEXT, 0x04,          // data true
                BLOCK_CONTEXT, 0x05          // data false
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == true)
            qb_assert(instruction.flags.has_false == true)
            qb_assert(instruction.flags.op == qb::instruction::CompareOp::LT)
            qb_assert(instruction.target.block == BLOCK_CONTEXT)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_CONTEXT)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_CONTEXT)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_CONTEXT)
            qb_assert(instruction.data_true.port == 0x04)
            qb_assert(instruction.data_false.block == BLOCK_CONTEXT)
            qb_assert(instruction.data_false.port == 0x05)
        })

    })

    qb_describe("Instruction: Math", {

        qb_test("MATH:const (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CONST_ASSIGNMENT, {
                qb::OpCode::MATH,
                0b01000111,                 // flags
                BLOCK_THREAD, 0x01,   // target
                BLOCK_CONTEXT, 0x02,         // source
            })
        })

        qb_test("MATH:? (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::MATH_OP, {
                qb::OpCode::MATH,
                0b11111111,                 // flags
                BLOCK_THREAD, 0x01,   // target
                BLOCK_CONTEXT, 0x02,         // source
            })
        })

        qb_test("MATH:add", {
            TEST_INSTRUCTION_OK(qb::instruction::Math, {
                qb::OpCode::MATH,
                0b01000011,                 // flags
                BLOCK_CONTEXT, 0x01,         // target
                BLOCK_CONTEXT, 0x02,         // source
            })
            qb_assert(instruction.type == qb::OpCode::MATH)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_source == true)
            qb_assert(instruction.flags.op == qb::instruction::Math::Flags::Op::ADD)
            qb_assert(instruction.target.block == BLOCK_CONTEXT)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.source.block == BLOCK_CONTEXT)
            qb_assert(instruction.source.port == 0x02)
        })

        qb_test("MATH:ln", {
            TEST_INSTRUCTION_OK(qb::instruction::Math, {
                qb::OpCode::MATH,
                0b01011011,                 // flags
                BLOCK_CONTEXT, 0x01,         // target
                BLOCK_CONTEXT, 0x02,         // source
            })
            qb_assert(instruction.type == qb::OpCode::MATH)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_source == true)
            qb_assert(instruction.flags.op == qb::instruction::Math::Flags::Op::LN)
            qb_assert(instruction.target.block == BLOCK_CONTEXT)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.source.block == BLOCK_CONTEXT)
            qb_assert(instruction.source.port == 0x02)
        })

    })

    qb_describe("Instruction: Context", {

        qb_test("RETURN", {
            TEST_INSTRUCTION_OK(qb::instruction::Return, {
                qb::OpCode::RETURN,
                BLOCK_CONTEXT, 0x01      // source
            })
            qb_assert(instruction.type == qb::OpCode::RETURN)
            qb_assert(instruction.source.block == BLOCK_CONTEXT)
            qb_assert(instruction.source.port == 0x01)
        })

    })

    qb_describe("Instruction: Thread", {

        qb_test("SLEEP", {
            TEST_INSTRUCTION_OK(qb::instruction::Sleep, {
                qb::OpCode::SLEEP,
                BLOCK_CONTEXT, 0x01      // time
            })
            qb_assert(instruction.type == qb::OpCode::SLEEP)
            qb_assert(instruction.time.block == BLOCK_CONTEXT)
            qb_assert(instruction.time.port == 0x01)
        })

    })

    qb_describe("Instruction: Engine", {

        qb_test("PUBLISH", {
            TEST_INSTRUCTION_OK(qb::instruction::Publish, {
                qb::OpCode::PUBLISH,
                4,                      // str length
                't', 'e', 's', 't',     // topic
                BLOCK_CONTEXT, 0x01      // source
            })
            qb_assert(instruction.type == qb::OpCode::PUBLISH)
            qb_assert(instruction.topic == "test")
            qb_assert(instruction.source.block == BLOCK_CONTEXT)
            qb_assert(instruction.source.port == 0x01)
        })

    })
})