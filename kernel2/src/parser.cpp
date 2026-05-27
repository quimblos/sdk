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
    auto NAME##_res = qb::parser::TYPE(bytes, length, addr); \
    if (NAME##_res.code != 0) return NAME##_res; \
    auto NAME = *NAME##_res.out.TYPE; \
    delete NAME##_res.out.TYPE; \
    addr = NAME##_res.addr;

#define PARSE_TYPE_DEF(NAME, FORCE_ADD) \
    auto NAME##_res = qb::parser::type_def(bytes, length, addr, FORCE_ADD); \
    if (NAME##_res.code != 0) return NAME##_res; \
    auto NAME = *NAME##_res.out.type_def; \
    delete NAME##_res.out.type_def; \
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

const qb::parser::res_t qb::parser::ref(const byte_t* bytes, code_addr_t length, code_addr_t addr) {
    PARSE_U8(block)
    PARSE_U8(port)
    OK({
        .ref = new qb::mem::Reference(block, port)
    })
}

const qb::parser::res_t qb::parser::type_def(const byte_t* bytes, code_addr_t length, code_addr_t addr, bool force_add) {
    bool is_add = force_add;
    if (!force_add) {
        PARSE_U8(add_flag)
        is_add = add_flag;
    }
    if (!is_add) {
        PARSE_U8(type)
        auto type_def = new TypeDef();
        type_def->use = type;
        OK({
            .type_def = type_def
        })
    }

    PARSE_U8(kind)
    switch (kind) {
        case qb::TypeKind::VOID:
        case qb::TypeKind::BOOL:
        case qb::TypeKind::INT:
        case qb::TypeKind::FLOAT:
        case qb::TypeKind::STRING:
        case qb::TypeKind::REF:
        case qb::TypeKind::REF_SLICE:
            ERROR(qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE)
        case qb::TypeKind::VECTOR:
        case qb::TypeKind::MAP:
        case qb::TypeKind::EVENT:
        case qb::TypeKind::FN: {
            PARSE_TYPE_DEF(child, false)
            auto type_def = new TypeDef({
                .add = {
                    .kind = (qb::TypeKind) kind,
                    .children = { child }
                }
            });
            type_def->add.kind = (qb::TypeKind) kind;
            type_def->add.children.resize(1);
            type_def->add.children = { child };
            OK({
                .type_def = type_def
            })
        }
        case qb::TypeKind::STRUCT: {
            PARSE_U8(n_fields)
            auto type_def = new TypeDef();
            type_def->add.kind = (qb::TypeKind) kind;
            type_def->add.children = std::vector<TypeDef>(n_fields);
            for (port_t i = 0; i < n_fields; i++) {
                PARSE_TYPE_DEF(child, false)
                type_def->add.children[i] = child;
            }
            OK({
                .type_def = type_def
            })
        }
    } 
    ERROR(qb::parser::res_t::Code::UNKNOWN_TYPE_KIND)
}

const qb::parser::res_t qb::parser::instruction(const byte_t* bytes, code_addr_t length, code_addr_t addr) {

    PARSE_BYTE(qb::OpCode, op_code)

    switch (op_code) {

        // Parser

        case qb::OpCode::USE_DRIVER: {
            PARSE(str, string)
            OK({
                .instruction = new qb::instruction::UseDriver(str)
            })
        }
        case qb::OpCode::ADD_TYPE: {
            PARSE_TYPE_DEF(type_def, true)
            OK({
                .instruction = new qb::instruction::AddType(type_def)
            })
        }
        case qb::OpCode::ADD_CONST: {
            PARSE_U8(type)
            uint16_t n_bytes = 0;
            switch (type) {
                case B_TYPE_VOID: break;
                case B_TYPE_NULL: break;
                case B_TYPE_BOOL: n_bytes = 1; break;
                case B_TYPE_U8: n_bytes = 1; break;
                case B_TYPE_I8: n_bytes = 1; break;
                case B_TYPE_U16: n_bytes = 2; break;
                case B_TYPE_I16: n_bytes = 2; break;
                case B_TYPE_U32: n_bytes = 4; break;
                case B_TYPE_I32: n_bytes = 4; break;
                case B_TYPE_F32: n_bytes = 4; break;
                case B_TYPE_REF: n_bytes = 2; break;
                case B_TYPE_REF_SLICE: {
                    ASSERT_N_BYTES(3)
                    uint8_t dims = bytes[addr+2];
                    n_bytes = 3 + dims*4;
                    break;
                }
                default: {
                    ASSERT_N_BYTES(2);
                    uint8_t len = parse_u16(bytes+addr);
                    n_bytes = 2+len;
                    break;
                }
            }
            ASSERT_N_BYTES(n_bytes);
            auto data = n_bytes == 0 ? nullptr : bytes+addr;
            addr += n_bytes;
            OK({
                .instruction = new qb::instruction::AddConst(type, data, n_bytes)
            })
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

        // Data Manipulation

        case qb::OpCode::SET: {
            PARSE_U8(flags)
            PARSE(target, ref)
            if (target.block >= BLOCK_THREAD)
                ERROR(qb::parser::res_t::Code::CONST_ASSIGNMENT)
            PARSE(source, ref)
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
            PARSE(entity, string)
            OK({
                .instruction = new qb::instruction::Hold(
                    entity
                )
            })
        }

        case qb::OpCode::RELEASE: {
            PARSE(entity, string)
            OK({
                .instruction = new qb::instruction::Release(
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
            PARSE(source, ref)
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
            PARSE(target, ref)
            if (target.block >= BLOCK_THREAD)
                ERROR(qb::parser::res_t::Code::CONST_ASSIGNMENT)
            PARSE(left, ref)
            PARSE(right, ref)
            
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

            qb::mem::Reference data_true;
            if (flags_struct.has_true){
                PARSE(data, ref)
                data_true = data;
            }
            else data_true = qb::mem::Reference(BLOCK_ENGINE, PORT_CONST_TRUE);

            qb::mem::Reference data_false;
            if (flags_struct.has_false){
                PARSE(data, ref)
                data_false = data;
            }
            else data_false = qb::mem::Reference(BLOCK_ENGINE, PORT_CONST_FALSE);
            
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

            PARSE(target, ref)
            if (target.block >= BLOCK_THREAD)
                ERROR(qb::parser::res_t::Code::CONST_ASSIGNMENT)
            PARSE(source, ref)
            OK({
                .instruction = new qb::instruction::Math(
                    flags_struct,
                    target,
                    source
                )
            })
        }
        
        // Context

        case qb::OpCode::RETURN: {
            PARSE(source, ref)
            OK({
                .instruction = new qb::instruction::Return(
                    source
                )
            })
        }

        // Thread

        case qb::OpCode::SLEEP: {
            PARSE(source, ref)
            OK({
                .instruction = new qb::instruction::Sleep(
                    source
                )
            })
        }

        // Engine

        case qb::OpCode::PUBLISH: {
            PARSE(topic, string)
            PARSE(source, ref)
            OK({
                .instruction = new qb::instruction::Publish(
                    topic,
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

    std::vector<std::string> drivers;
    std::vector<TypeDef> types;
    std::vector<qb::Code::Data> consts;
    std::vector<type_t> args;
    std::vector<type_t> vars;
    std::vector<Instruction*> instructions;
    qb::mem::Reference* out_value = nullptr;

    while (addr < length) {
        auto res = instruction(bytes, length, addr);
        if (res.code > 0) return res;
        
        switch (res.out.instruction->type) {
            case qb::OpCode::USE_DRIVER:
                drivers.push_back(((qb::instruction::UseDriver*)res.out.instruction)->name);
                delete res.out.instruction;
                break;
            case qb::OpCode::ADD_TYPE: {
                auto instruction = (qb::instruction::AddType*)res.out.instruction;
                types.push_back(instruction->type_def);
                delete res.out.instruction;
                break;
            }
            case qb::OpCode::ADD_CONST: {
                auto instruction = (qb::instruction::AddConst*)res.out.instruction;
                consts.push_back({
                    .tdx = instruction->tdx,
                    .bytes = instruction->bytes,
                    .length = instruction->length
                });
                delete res.out.instruction;
                break;
            }
            case qb::OpCode::ADD_ARG:
                args.push_back(((qb::instruction::AddArg*)res.out.instruction)->tdx);
                delete res.out.instruction;
                break;
            case qb::OpCode::ADD_VAR:
                vars.push_back(((qb::instruction::AddVar*)res.out.instruction)->tdx);
                delete res.out.instruction;
                break;
            case qb::OpCode::RETURN:
                out_value = ((qb::instruction::Return*)res.out.instruction)->source.copy();
                delete res.out.instruction;
                break;
            default:
                instructions.push_back(res.out.instruction);
        }

        addr = res.addr;
    }

    qb::Code* code = new qb::Code(
        drivers,
        types,
        consts,
        args,
        vars,
        instructions,
        out_value
    );

    OK({
        .code = code
    })
}

const qb::parser::res_t qb::parser::code(const std::string& hex) {
    auto bytecode = qb::hex_to_bytecode(hex);
    auto res = qb::parser::code(bytecode.bytes, bytecode.length);
    delete[] bytecode.bytes;
    return res;
}