#pragma once
#include "program.h"

#define MAKE_PROGRAM(CODE...) \
    std::vector<qb::Instruction*> instructions = { \
        CODE \
    }; \
    auto program = qb::Program("test", instructions, {});

#define PARSE_PROGRAM(LEN, CODE...) \
    qb::code_t bytes[LEN] = CODE; \
    auto res = qb::Program::make("test", bytes, LEN); \
    if (res.code > 0) { \
        std::cout << "Erorr parsing program: " << +res.code << std::endl; \
        ok = false; \
        break; \
    } \
    qb::Program* program = res.program;

qb_suite(test_program, "program", {

    qb_describe("constructor", {

        qb_test("devices", {
            MAKE_PROGRAM(
                new qb::instruction::UseDevice("led"),
                new qb::instruction::UseDevice("fan")
            )
            qb_assert(program.devices.size() == 2);
            qb_assert(program.devices[0] == "led");
            qb_assert(program.devices[1] == "fan");
        })

        qb_test("variables", {
            MAKE_PROGRAM(
                new qb::instruction::UseVar(qb::data::u8(0xAB)),
                new qb::instruction::UseVar(qb::data::u16(0xABCD))
            )
            qb_assert(program.variables.size() == 2);
            qb_assert(program.variables[0]->type == qb::DataType::UINT8);
            qb_assert(program.variables[1]->type == qb::DataType::UINT16);
        })

        qb_test("instructions", {
            qb::data::Reference target(false, 0b01111111, 0, 0);
            MAKE_PROGRAM(
                new qb::instruction::UseDevice("led"),
                new qb::instruction::Set(&target, qb::data::u8(0x12)),
                new qb::instruction::UseVar(qb::data::u8(0xAB)),
                new qb::instruction::Log(qb::data::u8(0x12)),
                new qb::instruction::UseVar(qb::data::u16(0xABCD)),
                new qb::instruction::Sleep(0x01234567),
            )
            qb_assert(program.instructions.size() == 3);
            qb_assert(program.instructions[0]->type == qb::InstructionType::SET);
            qb_assert(program.instructions[1]->type == qb::InstructionType::LOG);
            qb_assert(program.instructions[2]->type == qb::InstructionType::SLEEP);
        })

    })

    qb_describe("parse", {

        qb_test("devices", {
            PARSE_PROGRAM(10, {
                qb::InstructionType::USE_DEVICE, 0x03, 'l', 'e', 'd',
                qb::InstructionType::USE_DEVICE, 0x03, 'f', 'a', 'n',
            })
            qb_assert(program->devices.size() == 2);
            qb_assert(program->devices[0] == "led");
            qb_assert(program->devices[1] == "fan");
            delete program;
        })

        qb_test("variables", {
            PARSE_PROGRAM(17, {
                qb::InstructionType::USE_VAR, qb::DataType::INT16, 0xCF, 0xC7,
                qb::InstructionType::USE_VAR, qb::DataType::STRING, 0x00, 0x09, 's','o','m','e',' ','t','e','x','t'
            })
            qb_assert(program->variables.size() == 2);

            auto var0 = (qb::data::Numeric<int16_t>*) program->variables[0];
            qb_assert(var0->value == -12345);

            auto var1 = (qb::data::String*) program->variables[1];
            qb_assert(var1->value == "some text");
            delete program;
        })

        qb_test("instructions", {
            PARSE_PROGRAM(9, {
                qb::InstructionType::SET, 0xFF, 0x12, 0x00, 0x02, qb::DataType::UINT8, 0xAB,
                qb::InstructionType::HOLD, 0xFF
            })
            qb_assert(program->instructions.size() == 2);
            delete program;
        })

    })

})