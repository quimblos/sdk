#pragma once
#include "instruction.h"

#define PARSE_INSTRUCTION(TYPE, LEN, CODE...) \
    qb::code_t code[] = CODE; \
    auto out = qb::Instruction::make(code, LEN, 0); \
    qb_assert(out.code == 0); \
    auto instr = (qb::instruction::TYPE*) out.instr;

#define ASSERT_TARGET() \
    qb_assert(instr->target.device == 0xFF); \
    qb_assert(instr->target.port == 0x12); \
    qb_assert(instr->target.flags == 0b00000000); \
    qb_assert(instr->target.slice == nullptr);


#define ASSERT_TARGET_DEREF() \
    qb_assert(instr->target.device == 0xFF); \
    qb_assert(instr->target.port == 0x12); \
    qb_assert(instr->target.flags == 0b00000001); \
    qb_assert(instr->target.slice == nullptr);

#define ASSERT_TARGET_SLICE() \
    qb_assert(instr->target.device == 0xFF); \
    qb_assert(instr->target.port == 0x12); \
    qb_assert(instr->target.flags == 0b00000010); \
    qb_assert(instr->target.slice->dims == 1); \
    qb_assert(instr->target.slice->start[0] == 0); \
    qb_assert(instr->target.slice->end[0] == 1);

#define ASSERT_TARGET_DEREF_SLICE() \
    qb_assert(instr->target.device == 0xFF); \
    qb_assert(instr->target.port == 0x12); \
    qb_assert(instr->target.flags == 0b00000011); \
    qb_assert(instr->target.slice->dims == 1); \
    qb_assert(instr->target.slice->start[0] == 0); \
    qb_assert(instr->target.slice->end[0] == 1);

qb_suite(test_instruction, "instruction", {

    qb_describe("constructor", {

        qb_test("UseDevice", {
            auto instr = qb::instruction::UseDevice("led");
            qb_assert(instr.type == qb::InstructionType::USE_DEVICE);
            qb_assert(instr.name == "led");
        })
        qb_test("UseVar", {
            auto data = qb::data::u16(0xABCD);
            auto instr = qb::instruction::UseVar(data);
            qb_assert(instr.type == qb::InstructionType::USE_VAR);
            qb_assert(instr.data->type == qb::DataType::UINT16);
        })

        qb_test("Set", {
            auto data = qb::data::u8(0xAB);
            qb::data::Reference target(
                0xFF,
                0x12,
                0b00000011,
                qb::data::Slice::init({{0,1}})
            );
            auto instr = qb::instruction::Set(target, data);
            qb_assert(instr.type == qb::InstructionType::SET);
            qb_assert(instr.target.device == 0xFF);
            qb_assert(instr.target.port == 0x12);
            qb_assert(instr.target.flags == 0b00000011);
            qb_assert(instr.target.slice->dims == 1);
            qb_assert(instr.target.slice->start[0] == 0);
            qb_assert(instr.target.slice->end[0] == 1);
            qb_assert(instr.data->type == qb::DataType::UINT8);
        })
        qb_test("SetSlice", {
            qb::data::Reference target(
                0xFF,
                0x12,
                0b00000000,
                nullptr
            );
            auto shape = new const qb::Data*[2];
            shape[0] = qb::data::u8(0);
            shape[1] = qb::data::u8(1);
            auto instr = qb::instruction::SetSlice(target, 1, shape);
            qb_assert(instr.type == qb::InstructionType::SET_SLICE);
            qb_assert(instr.target.device == 0xFF);
            qb_assert(instr.target.port == 0x12);
            qb_assert(instr.target.flags == 0b00000000);
            qb_assert(instr.dims == 1);
            qb_assert(instr.shape[0]->type == qb::DataType::UINT8);
            qb_assert(instr.shape[1]->type == qb::DataType::UINT8);
        })
        qb_test("Hold", {
            auto instr = qb::instruction::Hold(0xFF);
            qb_assert(instr.type == qb::InstructionType::HOLD);
            qb_assert(instr.device == 0xFF);
        })
        qb_test("Release", {
            auto instr = qb::instruction::Release(0xFF);
            qb_assert(instr.type == qb::InstructionType::RELEASE);
            qb_assert(instr.device == 0xFF);
        })

        qb_test("Goto", {
            auto instr = qb::instruction::Goto(0x0001);
            qb_assert(instr.type == qb::InstructionType::GOTO);
            qb_assert(instr.addr == 0x0001);
        })
        qb_test("Branch", {
            auto data = qb::data::_bool(true);
            auto instr = qb::instruction::Branch(data, 0x0001, 0x0002);
            qb_assert(instr.type == qb::InstructionType::BRANCH);
            qb_assert(instr.data->type == qb::DataType::BOOL);
            qb_assert(instr.addr_true == 0x0001);
            qb_assert(instr.addr_false == 0x0002);
        })

        qb_test("ConditionalSet", {
            auto left = qb::data::_bool(false);
            auto right = qb::data::_bool(true);
            auto data_true = qb::data::u16(0xABCD);
            auto data_false = qb::data::u16(0x1234);
            qb::data::Reference target(
                0xFF,
                0x12,
                0b000000011,
                qb::data::Slice::init({{0,1}})
            );
            auto instr = qb::instruction::ConditionalSet(qb::InstructionType::SET_IF_EQ, &target, left, right, data_true, data_false);
            qb_assert(instr.type == qb::InstructionType::SET_IF_EQ);
            qb_assert(instr.target.device == 0xFF);
            qb_assert(instr.target.port == 0x12);
            qb_assert(instr.target.flags == 0b00000011);
            qb_assert(instr.target.slice->dims == 1);
            qb_assert(instr.target.slice->start[0] == 0);
            qb_assert(instr.target.slice->end[0] == 1);
            qb_assert(instr.left->type == qb::DataType::BOOL);
            qb_assert(instr.right->type == qb::DataType::BOOL);
            qb_assert(instr.data_true->type == qb::DataType::UINT16);
            qb_assert(instr.data_false->type == qb::DataType::UINT16);
        })
        
        qb_test("Arithmetic", {
            auto data = qb::data::u16(0xABCD);
            qb::data::Reference target(
                0xFF,
                0x12,
                0b00000011,
                qb::data::Slice::init({{0,1}})
            );
            auto instr = qb::instruction::Arithmetic(qb::InstructionType::ADD, &target, data);
            qb_assert(instr.type == qb::InstructionType::ADD);
            qb_assert(instr.target.device == 0xFF);
            qb_assert(instr.target.port == 0x12);
            qb_assert(instr.target.flags == 0b00000011);
            qb_assert(instr.target.slice->dims == 1);
            qb_assert(instr.target.slice->start[0] == 0);
            qb_assert(instr.target.slice->end[0] == 1);
            qb_assert(instr.data->type == qb::DataType::UINT16);
        })

        qb_test("Sleep", {
            auto time = qb::data::u32(0xFFFFFFFF);
            auto instr = qb::instruction::Sleep(time);
            qb_assert(instr.type == qb::InstructionType::SLEEP);
            auto data = instr.time->copy();
            qb_assert(data->type == qb::DataType::UINT32);
            qb_assert(qb::data::as_u32(data)->value == 0xFFFFFFFF);
            delete data;
        })
        qb_test("Log", {
            auto data = qb::data::str("Hello!");
            auto instr = qb::instruction::Log(data);
            qb_assert(instr.type == qb::InstructionType::LOG);
            qb_assert(instr.data->type == qb::DataType::STRING);
        })
        qb_test("Return", {
            auto data = qb::data::u16(0xFEDC);
            auto instr = qb::instruction::Return(data);
            qb_assert(instr.type == qb::InstructionType::RETURN);
            qb_assert(instr.data->type == qb::DataType::UINT16);
        })

    })

    qb_describe("parse", {

        qb_test("USE_DEVICE", {
            PARSE_INSTRUCTION(UseDevice, 5, {
                qb::InstructionType::USE_DEVICE,
                    0x03,           // name length
                    'l', 'e', 'd'   // name
                })
            qb_assert(instr->type == qb::InstructionType::USE_DEVICE);
            qb_assert(instr->name == "led");
            delete instr;
        })

        qb_test("USE_VAR", {
            PARSE_INSTRUCTION(UseVar, 5, {
                qb::InstructionType::USE_VAR,
                qb::DataType::UINT16, // type
                0x01, 0x02            // initial value
            })
            qb_assert(instr->type == qb::InstructionType::USE_VAR);
            qb_assert(instr->data->type == qb::DataType::UINT16);
            delete instr;
        })

        qb_test("SET", {
            PARSE_INSTRUCTION(Set, 6, {
                qb::InstructionType::SET,
                0xFF,                // target device
                0x12,                // target port
                0b00000000,          // target flags
                qb::DataType::UINT8, // data type
                0x01                 // data value
            })
            qb_assert(instr->type == qb::InstructionType::SET);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })

        qb_test("SET (deref)", {
            PARSE_INSTRUCTION(Set, 6, {
                qb::InstructionType::SET,
                0xFF,                // target device
                0x12,                // target port
                0b00000001,          // target flags
                qb::DataType::UINT8, // data type
                0x01                 // data value
            })
            qb_assert(instr->type == qb::InstructionType::SET);
            ASSERT_TARGET_DEREF()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })

        qb_test("SET (slice)", {
            PARSE_INSTRUCTION(Set, 11, {
                qb::InstructionType::SET,
                0xFF,                   // target device
                0x12,                   // target port
                0b00000010,             // target flags
                0x01,                   // target slice dims
                0x00, 0x00, 0x00, 0x01, // target slice start/end (u16)
                qb::DataType::UINT8,    // data type
                0x01                    // data value
            })
            qb_assert(instr->type == qb::InstructionType::SET);
            ASSERT_TARGET_SLICE()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })

        qb_test("SET (deref+slice)", {
            PARSE_INSTRUCTION(Set, 11, {
                qb::InstructionType::SET,
                0xFF,                   // target device
                0x12,                   // target port
                0b00000011,             // target flags
                0x01,                   // target slice dims
                0x00, 0x00, 0x00, 0x01, // target slice start/end (u16)
                qb::DataType::UINT8,    // data type
                0x01                    // data value
            })
            qb_assert(instr->type == qb::InstructionType::SET);
            ASSERT_TARGET_DEREF_SLICE()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })

        qb_test("SET_SLICE", {
            PARSE_INSTRUCTION(SetSlice, 11, {
                qb::InstructionType::SET_SLICE,
                0xFF,                   // target device
                0x12,                   // target port
                0b00000000,             // target flags
                0x01,                   // target slice dims
                qb::DataType::UINT16,   // slice start type
                0x00, 0x00,             // slice start value
                qb::DataType::UINT16,   // slice end type
                0x00, 0x01,             // slice end value
            })
            qb_assert(instr->type == qb::InstructionType::SET_SLICE);
            ASSERT_TARGET()
            qb_assert(instr->dims == 1);
            qb_assert(instr->shape[0]->type == qb::DataType::UINT16);
            qb_assert(instr->shape[1]->type == qb::DataType::UINT16);
            delete instr;
        })
        
        qb_test("HOLD", {
            PARSE_INSTRUCTION(Hold, 2, {
                qb::InstructionType::HOLD,
                0xFF // device
            })
            qb_assert(instr->type == qb::InstructionType::HOLD);
            qb_assert(instr->device == 0xFF);
            delete instr;
        })
        qb_test("RELEASE", {
            PARSE_INSTRUCTION(Release, 2, {
                qb::InstructionType::RELEASE,
                0xFF // device
            })
            qb_assert(instr->type == qb::InstructionType::RELEASE);
            qb_assert(instr->device == 0xFF);
            delete instr;
        })

        qb_test("GOTO", {
            PARSE_INSTRUCTION(Goto, 3, {
                qb::InstructionType::GOTO,
                0x00, 0x01 // code addr
            })
            qb_assert(instr->type == qb::InstructionType::GOTO);
            qb_assert(instr->addr == 0x0001);
            delete instr;
        })
        qb_test("BRANCH", {
            PARSE_INSTRUCTION(Branch, 7, {
                qb::InstructionType::BRANCH,
                qb::DataType::BOOL, // data type
                true,               // data value
                0x00, 0x01,         // code addr (true)
                0x00, 0x02          // code addr (false)
            })
            qb_assert(instr->type == qb::InstructionType::BRANCH);
            qb_assert(instr->data->type == qb::DataType::BOOL);
            qb_assert(instr->addr_true == 0x0001);
            qb_assert(instr->addr_false == 0x0002);
            delete instr;
        })

        qb_test("SET_IF_EQ", {
            PARSE_INSTRUCTION(ConditionalSet, 14, {
                qb::InstructionType::SET_IF_EQ,
                0xFF,                   // target device
                0x12,                   // target port
                0b00000000,             // target flags
                qb::DataType::BOOL,     // left type
                false,                  // left value
                qb::DataType::BOOL,     // right type
                true,                   // right value
                qb::DataType::UINT16,   // source1 type
                0xAB, 0xCD,             // source1 value
                qb::DataType::UINT16,   // source2 type
                0x12, 0x34              // source2 value
            })
            qb_assert(instr->type == qb::InstructionType::SET_IF_EQ);
            ASSERT_TARGET()
            qb_assert(instr->left->type == qb::DataType::BOOL);
            qb_assert(instr->right->type == qb::DataType::BOOL);
            qb_assert(instr->data_true->type == qb::DataType::UINT16);
            qb_assert(instr->data_false->type == qb::DataType::UINT16);
            delete instr;
        })
    
        qb_test("SET_IF_GT", {
            PARSE_INSTRUCTION(ConditionalSet, 14, {
                qb::InstructionType::SET_IF_GT,
                0xFF,                   // target device
                0x12,                   // target port
                0b00000000,             // target flags
                qb::DataType::UINT8,    // left type
                12,                     // left value
                qb::DataType::UINT8,    // right type
                13,                     // right value
                qb::DataType::UINT16,   // source1 type
                0xAB, 0xCD,             // source1 value
                qb::DataType::UINT16,   // source2 type
                0x12, 0x34              // source2 value
            })
            qb_assert(instr->type == qb::InstructionType::SET_IF_GT);
            ASSERT_TARGET()
            qb_assert(instr->left->type == qb::DataType::UINT8);
            qb_assert(instr->right->type == qb::DataType::UINT8);
            qb_assert(instr->data_true->type == qb::DataType::UINT16);
            qb_assert(instr->data_false->type == qb::DataType::UINT16);
            delete instr;
        })
    
        qb_test("SET_IF_LT", {
            PARSE_INSTRUCTION(ConditionalSet, 14, {
                qb::InstructionType::SET_IF_LT,
                0xFF,                   // target device
                0x12,                   // target port
                0b00000000,             // target flags
                qb::DataType::UINT8,    // left type
                12,                     // left value
                qb::DataType::UINT8,    // right type
                13,                     // right value
                qb::DataType::UINT16,   // source1 type
                0xAB, 0xCD,             // source1 value
                qb::DataType::UINT16,   // source2 type
                0x12, 0x34              // source2 value
            })
            qb_assert(instr->type == qb::InstructionType::SET_IF_LT);
            ASSERT_TARGET()
            qb_assert(instr->left->type == qb::DataType::UINT8);
            qb_assert(instr->right->type == qb::DataType::UINT8);
            qb_assert(instr->data_true->type == qb::DataType::UINT16);
            qb_assert(instr->data_false->type == qb::DataType::UINT16);
            delete instr;
        })

        qb_test("NOT", {
            PARSE_INSTRUCTION(Arithmetic, 6, {
                qb::InstructionType::NOT,
                0xFF,               // target device
                0x12,               // target port
                0b00000000,         // target flags
                qb::DataType::BOOL, // data type
                true                // data value
            })
            qb_assert(instr->type == qb::InstructionType::NOT);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::BOOL);
            delete instr;
        })
        qb_test("AND", {
            PARSE_INSTRUCTION(Arithmetic, 6, {
                qb::InstructionType::AND,
                0xFF,               // target device
                0x12,               // target port
                0b00000000,         // target flags
                qb::DataType::BOOL, // data type
                true                // data value
            })
            qb_assert(instr->type == qb::InstructionType::AND);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::BOOL);
            delete instr;
        })
        qb_test("OR", {
            PARSE_INSTRUCTION(Arithmetic, 6, {
                qb::InstructionType::OR,
                0xFF,               // target device
                0x12,               // target port
                0b00000000,         // target flags
                qb::DataType::BOOL, // data type
                true                // data value
            })
            qb_assert(instr->type == qb::InstructionType::OR);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::BOOL);
            delete instr;
        })

        qb_test("ADD", {
            PARSE_INSTRUCTION(Arithmetic, 6, {
                qb::InstructionType::ADD,
                0xFF,                // target device
                0x12,                // target port
                0b00000000,          // target flags
                qb::DataType::UINT8, // data type
                12                   // data value
            })
            qb_assert(instr->type == qb::InstructionType::ADD);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })
        qb_test("SUB", {
            PARSE_INSTRUCTION(Arithmetic, 6, {
                qb::InstructionType::SUB,
                0xFF,                // target device
                0x12,                // target port
                0b00000000,          // target flags
                qb::DataType::UINT8, // data type
                12                   // data value
            })
            qb_assert(instr->type == qb::InstructionType::SUB);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })
        qb_test("MULT", {
            PARSE_INSTRUCTION(Arithmetic, 6, {
                qb::InstructionType::MULT,
                0xFF,                // target device
                0x12,                // target port
                0b00000000,          // target flags
                qb::DataType::UINT8, // data type
                12                   // data value
            })
            qb_assert(instr->type == qb::InstructionType::MULT);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })
        qb_test("DIV", {
            PARSE_INSTRUCTION(Arithmetic, 6, {
                qb::InstructionType::DIV,
                0xFF,                // target device
                0x12,                // target port
                0b00000000,          // target flags
                qb::DataType::UINT8, // data type
                12                   // data value
            })
            qb_assert(instr->type == qb::InstructionType::DIV);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })
        qb_test("MOD", {
            PARSE_INSTRUCTION(Arithmetic, 6, {
                qb::InstructionType::MOD,
                0xFF,                // target device
                0x12,                // target port
                0b00000000,          // target flags
                qb::DataType::UINT8, // data type
                12                   // data value
            })
            qb_assert(instr->type == qb::InstructionType::MOD);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })
        qb_test("POW", {
            PARSE_INSTRUCTION(Arithmetic, 6, {
                qb::InstructionType::POW,
                0xFF,                // target device
                0x12,                // target port
                0b00000000,          // target flags
                qb::DataType::UINT8, // data type
                12                   // data value
            })
            qb_assert(instr->type == qb::InstructionType::POW);
            ASSERT_TARGET()
            qb_assert(instr->data->type == qb::DataType::UINT8);
            delete instr;
        })

        qb_test("SLEEP", {
            PARSE_INSTRUCTION(Sleep, 6, {
                qb::InstructionType::SLEEP,
                qb::DataType::UINT32,       // data type
                0x01, 0x23, 0x45, 0x67      // data value
            })
            qb_assert(instr->type == qb::InstructionType::SLEEP);
            qb_assert(instr->time->type == qb::DataType::UINT32);
            auto time = instr->time->copy();
            qb_assert(qb::data::as_u32(time)->value == 0x01234567);
            delete time;
            delete instr;
        })
        qb_test("LOG", {
            PARSE_INSTRUCTION(Log, 10, {
                qb::InstructionType::LOG,
                qb::DataType::STRING,           // data type
                0x00, 0x06,                     // string length
                'H', 'e', 'l', 'l', 'o', '!'    // string data
            })
            qb_assert(instr->type == qb::InstructionType::LOG);
            qb_assert(instr->data->type == qb::DataType::STRING);
            delete instr;
        })
        qb_test("RETURN", {
            PARSE_INSTRUCTION(Return, 4, {
                qb::InstructionType::RETURN,
                qb::DataType::UINT16,   // data type
                0xFE, 0xDC              // data value
            })
            qb_assert(instr->type == qb::InstructionType::RETURN);
            qb_assert(instr->data->type == qb::DataType::UINT16);
            delete instr;
        })
    })
})