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
    delete res.out.instruction; \
    qb_assert(res_code == EXPECTED_ERROR)

qb_suite(test_parser, "parser", {

    qb_describe("Instruction: Parser", {
    
        qb_test("USE_BLOCK", {
            TEST_INSTRUCTION_OK(qb::instruction::UseBlock, {
                qb::OpCode::USE_BLOCK,
                4,                  // str length
                't', 'e', 's', 't'  // str
            })
            qb_assert(instruction.type == qb::OpCode::USE_BLOCK)
            qb_assert(instruction.name == "test")
        })
    
        qb_test("ADD_VAR", {
            TEST_INSTRUCTION_OK(qb::instruction::AddVar, {
                qb::OpCode::ADD_VAR,
                B_TYPE_U8,          // type
            })
            qb_assert(instruction.type == qb::OpCode::ADD_VAR)
            qb_assert(instruction.tdx == B_TYPE_U8)
        })
    
        qb_test("ADD_TYPE:VOID (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::VOID,      // kind
                0b00000000               // flags
            })
        })
    
        qb_test("ADD_TYPE:BOOL (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::BOOL,      // kind
                0b000000000              // flags
            })
        })
    
        qb_test("ADD_TYPE:INT (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::INT,      // kind
                0b00000011              // flags
            })
        })
    
        qb_test("ADD_TYPE:FLOAT (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::FLOAT,    // kind
                0b00000000              // flags
            })
        })
    
        qb_test("ADD_TYPE:STRING (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::STRING,   // kind
                0b00000000              // flags
            })
        })
    
        qb_test("ADD_TYPE:REF (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE, {
                qb::OpCode::ADD_TYPE,
                qb::TypeKind::REF,   // kind
                0b00000000           // flags
            })
        })
    })

    qb_describe("Instruction: Data Manipulation", {
    
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
                BLOCK_ROUTINE, 0x00,         // target
                BLOCK_THREAD, 0x01,   // source
            })
            qb_assert(instruction.type == qb::OpCode::SET)
            qb_assert(instruction.flags.deref_target == false)
            qb_assert(instruction.flags.deref_source == true)
            qb_assert(instruction.target.block == BLOCK_ROUTINE)
            qb_assert(instruction.target.port == 0x00)
            qb_assert(instruction.source.block == BLOCK_THREAD)
            qb_assert(instruction.source.port == 0x01)
        })
    
        qb_test("HOLD:? (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::ENTITY_KIND, {
                qb::OpCode::HOLD,
                0xFF,                               // kind                 // flags
                0
            })
        })

        qb_test("HOLD:node", {
            TEST_INSTRUCTION_OK(qb::instruction::Hold, {
                qb::OpCode::HOLD,
                qb::instruction::Hold::Kind::NODE,  // kind                 // flags
                4,                                  // str length
                't', 'e', 's', 't'                  // entity
            })
            qb_assert(instruction.type == qb::OpCode::HOLD)
            qb_assert(instruction.kind == qb::instruction::Hold::Kind::NODE)
            qb_assert(instruction.entity == "test")
        })

        qb_test("HOLD:node", {
            TEST_INSTRUCTION_OK(qb::instruction::Hold, {
                qb::OpCode::HOLD,
                qb::instruction::Hold::Kind::NODE,  // kind                 // flags
                4,                                  // str length
                't', 'e', 's', 't'                  // entity
            })
            qb_assert(instruction.type == qb::OpCode::HOLD)
            qb_assert(instruction.kind == qb::instruction::Hold::Kind::NODE)
            qb_assert(instruction.entity == "test")
        })
    
        qb_test("RELEASE:? (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::ENTITY_KIND, {
                qb::OpCode::RELEASE,
                0xFF,                               // kind                 // flags
                0
            })
        })

        qb_test("RELEASE:node", {
            TEST_INSTRUCTION_OK(qb::instruction::Release, {
                qb::OpCode::RELEASE,
                qb::instruction::Release::Kind::NODE,  // kind                 // flags
                4,                                  // str length
                't', 'e', 's', 't'                  // entity
            })
            qb_assert(instruction.type == qb::OpCode::RELEASE)
            qb_assert(instruction.kind == qb::instruction::Release::Kind::NODE)
            qb_assert(instruction.entity == "test")
        })

        qb_test("RELEASE:node", {
            TEST_INSTRUCTION_OK(qb::instruction::Release, {
                qb::OpCode::RELEASE,
                qb::instruction::Release::Kind::NODE,  // kind                 // flags
                4,                                  // str length
                't', 'e', 's', 't'                  // entity
            })
            qb_assert(instruction.type == qb::OpCode::RELEASE)
            qb_assert(instruction.kind == qb::instruction::Release::Kind::NODE)
            qb_assert(instruction.entity == "test")
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

        qb_test("SET_IF:const (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CONST_ASSIGNMENT, {
                qb::OpCode::SET_IF,
                0b00011111,                 // flags
                BLOCK_THREAD, 0x02,   // target
                BLOCK_ROUTINE, 0x01,         // left
                BLOCK_ROUTINE, 0x02,         // right
            })
        })

        qb_test("SET_IF:implicit-true-false", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b00001111,                 // flags
                BLOCK_ROUTINE, 0x01,         // target
                BLOCK_ROUTINE, 0x02,         // left
                BLOCK_ROUTINE, 0x03,         // right
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == false)
            qb_assert(instruction.flags.has_false == false)
            qb_assert(instruction.flags.op == qb::instruction::SetIf::Flags::Op::EQ)
            qb_assert(instruction.target.block == BLOCK_ROUTINE)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_ROUTINE)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_ROUTINE)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_KERNEL)
            qb_assert(instruction.data_true.port == PORT_CONST_TRUE)
            qb_assert(instruction.data_false.block == BLOCK_KERNEL)
            qb_assert(instruction.data_false.port == PORT_CONST_FALSE)
        })

        qb_test("SET_IF:implicit-false", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b00011111,                 // flags
                BLOCK_ROUTINE, 0x01,         // target
                BLOCK_ROUTINE, 0x02,         // left
                BLOCK_ROUTINE, 0x03,         // right
                BLOCK_ROUTINE, 0x04          // data true
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == true)
            qb_assert(instruction.flags.has_false == false)
            qb_assert(instruction.flags.op == qb::instruction::SetIf::Flags::Op::EQ)
            qb_assert(instruction.target.block == BLOCK_ROUTINE)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_ROUTINE)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_ROUTINE)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_ROUTINE)
            qb_assert(instruction.data_true.port == 0x04)
            qb_assert(instruction.data_false.block == BLOCK_KERNEL)
            qb_assert(instruction.data_false.port == PORT_CONST_FALSE)
        })

        qb_test("SET_IF:implicit-true", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b00101111,                 // flags
                BLOCK_ROUTINE, 0x01,         // target
                BLOCK_ROUTINE, 0x02,         // left
                BLOCK_ROUTINE, 0x03,         // right
                BLOCK_ROUTINE, 0x04          // data false
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == false)
            qb_assert(instruction.flags.has_false == true)
            qb_assert(instruction.flags.op == qb::instruction::SetIf::Flags::Op::EQ)
            qb_assert(instruction.target.block == BLOCK_ROUTINE)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_ROUTINE)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_ROUTINE)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_KERNEL)
            qb_assert(instruction.data_true.port == PORT_CONST_TRUE)
            qb_assert(instruction.data_false.block == BLOCK_ROUTINE)
            qb_assert(instruction.data_false.port == 0x04)
        })

        qb_test("SET_IF", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b00111111,                 // flags
                BLOCK_ROUTINE, 0x01,         // target
                BLOCK_ROUTINE, 0x02,         // left
                BLOCK_ROUTINE, 0x03,         // right
                BLOCK_ROUTINE, 0x04,          // data true
                BLOCK_ROUTINE, 0x05          // data false
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == true)
            qb_assert(instruction.flags.has_false == true)
            qb_assert(instruction.flags.op == qb::instruction::SetIf::Flags::Op::EQ)
            qb_assert(instruction.target.block == BLOCK_ROUTINE)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_ROUTINE)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_ROUTINE)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_ROUTINE)
            qb_assert(instruction.data_true.port == 0x04)
            qb_assert(instruction.data_false.block == BLOCK_ROUTINE)
            qb_assert(instruction.data_false.port == 0x05)
        })

        qb_test("SET_IF:lt", {
            TEST_INSTRUCTION_OK(qb::instruction::SetIf, {
                qb::OpCode::SET_IF,
                0b10111111,                 // flags
                BLOCK_ROUTINE, 0x01,         // target
                BLOCK_ROUTINE, 0x02,         // left
                BLOCK_ROUTINE, 0x03,         // right
                BLOCK_ROUTINE, 0x04,          // data true
                BLOCK_ROUTINE, 0x05          // data false
            })
            qb_assert(instruction.type == qb::OpCode::SET_IF)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_left == true)
            qb_assert(instruction.flags.deref_right == true)
            qb_assert(instruction.flags.deref_data == true)
            qb_assert(instruction.flags.has_true == true)
            qb_assert(instruction.flags.has_false == true)
            qb_assert(instruction.flags.op == qb::instruction::SetIf::Flags::Op::LT)
            qb_assert(instruction.target.block == BLOCK_ROUTINE)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.left.block == BLOCK_ROUTINE)
            qb_assert(instruction.left.port == 0x02)
            qb_assert(instruction.right.block == BLOCK_ROUTINE)
            qb_assert(instruction.right.port == 0x03)
            qb_assert(instruction.data_true.block == BLOCK_ROUTINE)
            qb_assert(instruction.data_true.port == 0x04)
            qb_assert(instruction.data_false.block == BLOCK_ROUTINE)
            qb_assert(instruction.data_false.port == 0x05)
        })

    })

    qb_describe("Instruction: Math", {

        qb_test("MATH:const (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::CONST_ASSIGNMENT, {
                qb::OpCode::MATH,
                0b01000111,                 // flags
                BLOCK_THREAD, 0x01,   // target
                BLOCK_ROUTINE, 0x02,         // source
            })
        })

        qb_test("MATH:? (should fail)", {
            TEST_INSTRUCTION_FAIL(qb::parser::res_t::Code::MATH_OP, {
                qb::OpCode::MATH,
                0b11111111,                 // flags
                BLOCK_THREAD, 0x01,   // target
                BLOCK_ROUTINE, 0x02,         // source
            })
        })

        qb_test("MATH:add", {
            TEST_INSTRUCTION_OK(qb::instruction::Math, {
                qb::OpCode::MATH,
                0b01000011,                 // flags
                BLOCK_ROUTINE, 0x01,         // target
                BLOCK_ROUTINE, 0x02,         // source
            })
            qb_assert(instruction.type == qb::OpCode::MATH)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_source == true)
            qb_assert(instruction.flags.op == qb::instruction::Math::Flags::Op::ADD)
            qb_assert(instruction.target.block == BLOCK_ROUTINE)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.source.block == BLOCK_ROUTINE)
            qb_assert(instruction.source.port == 0x02)
        })

        qb_test("MATH:ln", {
            TEST_INSTRUCTION_OK(qb::instruction::Math, {
                qb::OpCode::MATH,
                0b01011011,                 // flags
                BLOCK_ROUTINE, 0x01,         // target
                BLOCK_ROUTINE, 0x02,         // source
            })
            qb_assert(instruction.type == qb::OpCode::MATH)
            qb_assert(instruction.flags.deref_target == true)
            qb_assert(instruction.flags.deref_source == true)
            qb_assert(instruction.flags.op == qb::instruction::Math::Flags::Op::LN)
            qb_assert(instruction.target.block == BLOCK_ROUTINE)
            qb_assert(instruction.target.port == 0x01)
            qb_assert(instruction.source.block == BLOCK_ROUTINE)
            qb_assert(instruction.source.port == 0x02)
        })

    })

    qb_describe("Instruction: Thread", {

        qb_test("SLEEP", {
            TEST_INSTRUCTION_OK(qb::instruction::Sleep, {
                qb::OpCode::SLEEP,
                BLOCK_ROUTINE, 0x01      // time
            })
            qb_assert(instruction.type == qb::OpCode::SLEEP)
            qb_assert(instruction.time.block == BLOCK_ROUTINE)
            qb_assert(instruction.time.port == 0x01)
        })

        qb_test("PUBLISH", {
            TEST_INSTRUCTION_OK(qb::instruction::Publish, {
                qb::OpCode::PUBLISH,
                4,                      // str length
                't', 'e', 's', 't',     // topic
                BLOCK_ROUTINE, 0x01      // source
            })
            qb_assert(instruction.type == qb::OpCode::PUBLISH)
            qb_assert(instruction.topic == "test")
            qb_assert(instruction.source.block == BLOCK_ROUTINE)
            qb_assert(instruction.source.port == 0x01)
        })

        qb_test("RETURN", {
            TEST_INSTRUCTION_OK(qb::instruction::Return, {
                qb::OpCode::RETURN,
                BLOCK_ROUTINE, 0x01      // source
            })
            qb_assert(instruction.type == qb::OpCode::RETURN)
            qb_assert(instruction.source.block == BLOCK_ROUTINE)
            qb_assert(instruction.source.port == 0x01)
        })

    })
})