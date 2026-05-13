#include <iostream>
#include "parser.h"

#define ERROR(CODE) \
    return { \
        .code = CODE, \
        .addr = addr \
    };

#define OK(OUT) \
    return { \
        .code = qb::parser::res_t::Code::OK, \
        .out = OUT, \
        .addr = addr \
    };

#define ASSERT_N_BYTES(N) \
    if (addr + N > length) ERROR(qb::parser::res_t::Code::UNEXPECTED_EOF)
    
#define PARSE_BYTE(TYPE, VAR) \
    ASSERT_N_BYTES(1) \
    TYPE VAR = (TYPE) bytes[addr]; \
    addr += 1;
    
#define PARSE_U8(VAR) PARSE_BYTE(uint8_t, VAR)

#define PARSE_U16(VAR) \
    ASSERT_N_BYTES(2) \
    uint16_t VAR = parse_u16(bytes+addr); \
    addr += 2;

#define PARSE_U32(VAR) \
    ASSERT_N_BYTES(4) \
    uint32_t VAR = parse_u32(bytes+addr); \
    addr += 4;

#define PARSE(NAME, TYPE) \
    auto NAME##_res = TYPE(bytes, length, addr); \
    if (NAME##_res.code != 0) return NAME##_res; \
    auto NAME = *NAME##_res.out.TYPE; \
    delete NAME##_res.out.TYPE; \
    addr = NAME##_res.addr;

#define DEBUG() \
    std::cout << "addr: " << +addr << "/" << +length << " = " << +bytes[addr] << "(" << (char) bytes[addr] << ")" << std::endl;

const qb::parser::res_t qb::parser::string(const byte_t* bytes, code_addr_t length, code_addr_t addr) {
    PARSE_U8(str_length)
    ASSERT_N_BYTES(str_length)
    auto string = new std::string((const char*) bytes + addr, str_length);
    addr += str_length;
    OK({
        .string = string
    })
}

const qb::parser::res_t qb::parser::pointer(const byte_t* bytes, code_addr_t length, code_addr_t addr) {
    PARSE_U8(block)
    PARSE_U8(port)
    OK({
        .pointer = new Pointer(block, port)
    })
}

const qb::parser::res_t qb::parser::instruction(const byte_t* bytes, code_addr_t length, code_addr_t addr) {

    PARSE_BYTE(qb::OpCode, op_code)

    switch (op_code) {

        // Parser

        case qb::OpCode::USE_BLOCK: {
            PARSE(str, string)
            OK({
                .instruction = new qb::instruction::UseBlock(str)
            })
        }
        case qb::OpCode::ADD_CONST: {
            PARSE_U8(kind)
            switch (kind) {
                case qb::TypeKind::VOID:
                    break;
                case qb::TypeKind::BOOL: {
                    // PARSE_U8(value)
                    // OK({
                    //     .instruction = new qb::instruction::AddConst(tdx, new bool(value))
                    // })
                    break;
                }
                case qb::TypeKind::INT:
                    break;
                case qb::TypeKind::FLOAT:
                    break;
                case qb::TypeKind::STRING:
                    break;
                case qb::TypeKind::REF:
                    break;
                case qb::TypeKind::VECTOR:
                    break;
                case qb::TypeKind::MAP:
                    break;
                case qb::TypeKind::STRUCT:
                    break;
                case qb::TypeKind::EVENT:
                    break;
                case qb::TypeKind::FN:
                    break;
            }
            break;
        }

        case qb::OpCode::ADD_ARG: {
            PARSE_U8(type)
            OK({
                .instruction = new qb::instruction::AddArg(type)
            })
        }

        case qb::OpCode::ADD_VAR: {
            PARSE_U8(type)
            OK({
                .instruction = new qb::instruction::AddVar(type)
            })
        }

        case qb::OpCode::ADD_TYPE: {
            PARSE_U8(kind)
            if (kind < 0x06)
                ERROR(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE)
            PARSE_U8(flags)
            OK({
                .instruction = new qb::instruction::AddType(
                    (qb::TypeKind) kind,
                    { .value = flags },
                    nullptr
                )
            })
        }

        // Data Manipulation

        case qb::OpCode::SET: {
            PARSE_U8(flags)
            PARSE(target, pointer)
            if (target.block >= BLOCK_THREAD)
                ERROR(qb::parser::res_t::Code::CONST_ASSIGNMENT)
            PARSE(source, pointer)
            OK({
                .instruction = new qb::instruction::Set(
                    {
                        .deref_target = (flags & 1) > 0,
                        .deref_source = (flags & 2) > 0,
                    },
                    target,
                    source
                )
            })
        }

        case qb::OpCode::HOLD: {
            PARSE_U8(kind)
            if (kind > 0x01)
                ERROR(qb::parser::res_t::Code::ENTITY_KIND)
            PARSE(entity, string)
            OK({
                .instruction = new qb::instruction::Hold(
                    (qb::instruction::Hold::Kind) kind,
                    entity
                )
            })
        }

        case qb::OpCode::RELEASE: {
            PARSE_U8(kind)
            if (kind > 0x01)
                ERROR(qb::parser::res_t::Code::ENTITY_KIND)
            PARSE(entity, string)
            OK({
                .instruction = new qb::instruction::Release(
                    (qb::instruction::Release::Kind) kind,
                    entity
                )
            })
        }

        // Flow Control

        case qb::OpCode::GOTO: {
            PARSE_U16(addr_goto)
            OK({
                .instruction = new qb::instruction::Goto(
                    addr_goto
                )
            })
        }

        case qb::OpCode::IF: {
            PARSE_U8(flags)
            PARSE(source, pointer)
            PARSE_U16(addr_true)
            PARSE_U16(addr_false)
            OK({
                .instruction = new qb::instruction::If(
                    (qb::instruction::If::Flags) flags,
                    source,
                    addr_true,
                    addr_false
                )
            })
        }

        // Conditional Data Manipulation

        case qb::OpCode::SET_IF: {
            PARSE_U8(flags)
            PARSE(target, pointer)
            if (target.block >= BLOCK_THREAD)
                ERROR(qb::parser::res_t::Code::CONST_ASSIGNMENT)
            PARSE(left, pointer)
            PARSE(right, pointer)
            
            auto flags_struct = qb::instruction::SetIf::Flags({
                .deref_target = (flags & 1) > 0,
                .deref_left = (flags & 2) > 0,
                .deref_right = (flags & 4) > 0,
                .deref_data = (flags & 8) > 0,
                .has_true = (flags & 16) > 0,
                .has_false = (flags & 32) > 0,
                .op = (qb::instruction::SetIf::Flags::Op) (flags >> 6)
            });

            if (flags_struct.op > 0x02)
                ERROR(qb::parser::res_t::Code::COMPARE_OP)

            Pointer data_true;
            if (flags_struct.has_true){
                PARSE(data, pointer)
                data_true = data;
            }
            else data_true = Pointer(BLOCK_KERNEL, PORT_CONST_TRUE);

            Pointer data_false;
            if (flags_struct.has_false){
                PARSE(data, pointer)
                data_false = data;
            }
            else data_false = Pointer(BLOCK_KERNEL, PORT_CONST_FALSE);
            
            OK({
                .instruction = new qb::instruction::SetIf(
                    flags_struct,
                    target,
                    left,
                    right,
                    data_true,
                    data_false
                )
            })
        }

        // Math

        case qb::OpCode::MATH: {
            PARSE_U8(flags)

            auto flags_struct = qb::instruction::Math::Flags({
                .deref_target = (flags & 1) > 0,
                .deref_source = (flags & 2) > 0,
                .op = (qb::instruction::Math::Flags::Op) (flags >> 2)
            });

            if (
                (flags_struct.op > 0x03 && flags_struct.op < 0x10)
                || flags_struct.op > 0x16
            )
                ERROR(qb::parser::res_t::Code::MATH_OP)

            PARSE(target, pointer)
            if (target.block >= BLOCK_THREAD)
                ERROR(qb::parser::res_t::Code::CONST_ASSIGNMENT)
            PARSE(source, pointer)
            OK({
                .instruction = new qb::instruction::Math(
                    flags_struct,
                    target,
                    source
                )
            })
        }

        // Thread

        case qb::OpCode::SLEEP: {
            PARSE(source, pointer)
            OK({
                .instruction = new qb::instruction::Sleep(
                    source
                )
            })
        }

        case qb::OpCode::PUBLISH: {
            PARSE(topic, string)
            PARSE(source, pointer)
            OK({
                .instruction = new qb::instruction::Publish(
                    topic,
                    source
                )
            })
        }

        case qb::OpCode::RETURN: {
            PARSE(source, pointer)
            OK({
                .instruction = new qb::instruction::Return(
                    source
                )
            })
        }

    }

    ERROR(qb::parser::res_t::Code::UNKNOWN_OP_CODE)    
}

const qb::parser::res_t qb::parser::code(const byte_t* bytes, code_addr_t length, code_addr_t addr) {

    ASSERT_N_BYTES(4)
    if (bytes[0] != 'q'
     || bytes[1] != 'b'
     || bytes[2] != 0
     || bytes[3] != 0)
        ERROR(qb::parser::res_t::Code::INVALID_HEADER)

    addr += 4;

    std::vector<std::string> blocks;
    std::vector<type_t> args;
    std::vector<type_t> vars;
    std::vector<Instruction*> instructions;

    while (addr < length) {
        auto res = instruction(bytes, length, addr);
        if (res.code > 0) return res;
        
        switch (res.out.instruction->type) {
            case qb::OpCode::USE_BLOCK:
                blocks.push_back(((qb::instruction::UseBlock*)res.out.instruction)->name);
                delete res.out.instruction;
                break;
            case qb::OpCode::ADD_CONST:
                // TODO
                delete res.out.instruction;
                break;
            case qb::OpCode::ADD_ARG:
                args.push_back(((qb::instruction::AddArg*)res.out.instruction)->tdx);
                delete res.out.instruction;
                break;
            case qb::OpCode::ADD_VAR:
                vars.push_back(((qb::instruction::AddVar*)res.out.instruction)->tdx);
                delete res.out.instruction;
                break;
            case qb::OpCode::ADD_TYPE:
                // TODO
                delete res.out.instruction;
                break;
            default:
                instructions.push_back(res.out.instruction);
        }

        addr = res.addr;
    }

    qb::Code* code = new qb::Code(
        blocks,
        args,
        vars,
        instructions
    );

    OK({
        .code = code
    })
}