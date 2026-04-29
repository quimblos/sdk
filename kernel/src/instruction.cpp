#include "instruction.h"
#define QB_PARSER_DEBUG
#include <iostream>

using namespace qb;

#define ASSERT_N_BYTES(N) \
    if (addr + N > code_len) return { \
        .code = QB_CODE_R_PARSE_FAILED_UNEXPECTED_EOF, \
        .next_addr = 0xFFFF \
    };

#define PARSE_U8(VAR) \
    ASSERT_N_BYTES(1) \
    uint8_t VAR = bytes[addr]; \
    addr += 1;

#define PARSE_U16(VAR) \
    ASSERT_N_BYTES(2) \
    uint16_t VAR = parse_u16(bytes+addr); \
    addr += 2;

#define PARSE_U32(VAR) \
    ASSERT_N_BYTES(4) \
    uint32_t VAR = parse_u32(bytes+addr); \
    addr += 4;

#define PARSE_TARGET() \
    bool deref = false; \
    PARSE_U8(device); \
    PARSE_U8(port); \
    PARSE_U8(flags); \
    bool flag_deref = flags & 0b00000001; \
    bool flag_slice = flags & 0b00000010; \
    qb::data::Slice* slice; \
    if (flag_slice) { \
        PARSE_U8(dims); \
        slice = new qb::data::Slice(dims); \
    } \
    qb::data::Reference target = { \
        device = device, \
        port = port, \
        deref = flag_deref, \
        slice = slice \
    };

#define PARSE_DATA(VAR) \
    qb::data::res_t VAR##_res = qb::Data::parse(bytes, code_len, addr); \
    if (VAR##_res.code > 0) return { \
        .code = QB_CODE_R_PARSE_FAILED_DATA, \
        .next_addr = 0xFFFF \
    }; \
    addr = VAR##_res.next_addr; \
    qb::Data* VAR = VAR##_res.data;

#define OK(INSTR) \
    return { \
        .code = QB_CODE_R_PARSE_OK, \
        .instr = INSTR, \
        .next_addr = (qb::code_addr_t) (addr) \
    };

qb::instruction::res_t qb::Instruction::make(qb::code_t* bytes, qb::code_addr_t code_len, qb::code_addr_t addr) {
    
    PARSE_U8(type)

    switch (type) {
        // Parser
        case qb::InstructionType::USE_DEVICE:
        {
            PARSE_U8(length)
            ASSERT_N_BYTES(length);
            auto name = std::string((char*) bytes+addr, length);
            addr += length;
            OK(new qb::instruction::UseDevice(name))
        }
        case qb::InstructionType::USE_VAR:
        {
            PARSE_DATA(data)
            OK(new qb::instruction::UseVar(data))
        }

        // Data Manipulation
        case qb::InstructionType::SET:
        {
            PARSE_TARGET()
            PARSE_DATA(data)
            OK(new qb::instruction::Set(&target, data))
        }
        case qb::InstructionType::HOLD:
        {
            PARSE_U8(device)
            OK(new qb::instruction::Hold(device))
        }
        case qb::InstructionType::RELEASE:
        {
            PARSE_U8(device)
            OK(new qb::instruction::Release(device))
        }

        case qb::InstructionType::GOTO:
        {
            PARSE_U16(addr_to)
            OK(new qb::instruction::Goto(addr_to))
        }
        case qb::InstructionType::BRANCH:
        {
            PARSE_DATA(data)
            PARSE_U16(addr_true)
            PARSE_U16(addr_false)
            OK(new qb::instruction::Branch(data, addr_true, addr_false))
        }

        case qb::InstructionType::SET_IF_EQ:
        case qb::InstructionType::SET_IF_GT:
        case qb::InstructionType::SET_IF_LT:
        {
            PARSE_TARGET()
            PARSE_DATA(left)
            PARSE_DATA(right)
            PARSE_DATA(data_true)
            PARSE_DATA(data_false)
            OK(new qb::instruction::ConditionalSet((qb::InstructionType) type, &target, left, right, data_true, data_false))
        }

        // Arithmetic
        case qb::InstructionType::NOT:
        case qb::InstructionType::AND:
        case qb::InstructionType::OR:
        case qb::InstructionType::ADD:
        case qb::InstructionType::SUB:
        case qb::InstructionType::MULT:
        case qb::InstructionType::DIV:
        case qb::InstructionType::MOD:
        case qb::InstructionType::POW:
        {
            PARSE_TARGET()
            PARSE_DATA(data)
            OK(new qb::instruction::Arithmetic((qb::InstructionType) type, &target, data))
        }

        // Runner
        case qb::InstructionType::SLEEP:
        {
            PARSE_DATA(time)
            OK(new qb::instruction::Sleep(time))
        }
        case qb::InstructionType::LOG:
        {
            PARSE_DATA(data)
            OK(new qb::instruction::Log(data))
        }
        case qb::InstructionType::RETURN:
        {
            PARSE_DATA(data)
            OK(new qb::instruction::Return(data))
        }
    }

    return {
        .code = QB_CODE_R_PARSE_FAILED_UNKNOWN_TYPE,
        .next_addr = 0xFFFF
    };
}