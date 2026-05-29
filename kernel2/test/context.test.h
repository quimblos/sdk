#pragma once
#include "context.h"

#define MAKE_CONTEXT(CODE...) \
    qb::byte_t code[] = CODE; \
    auto res = qb::parser::code(code, sizeof(code)/sizeof(qb::byte_t)); \
    if (res.code != 0) { \
        std::cout << qb::i18n::parser.at(res.code) << std::endl; \
        qb_fail() \
    } \
    std::cout << res.out.code->to_str() << std::endl; \
    auto _code = qb::Code({},{},{},{},{},{},nullptr); \
    auto thread = qb::Thread(nullptr, "test", &_code, {}); \
    auto context = qb::Context(&thread, nullptr, res.out.code);

qb_suite(test_context, "context", {

    qb_describe("Context", {
    
        qb_test("Types", {
            
            MAKE_CONTEXT({
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_TYPE, qb::TypeKind::VECTOR, false, B_TYPE_U8,
                qb::OpCode::ADD_TYPE, qb::TypeKind::STRUCT, 3, false, B_TYPE_BOOL, false, 0, false, B_TYPE_U8,
            });
            qb_assert(context.block.type_block.size() == 3)
            auto type0 = context.block.type_block.get(0);
            qb_assert(type0->kind == qb::TypeKind::VECTOR)
            qb_assert(type0->schema.of_map.type->kind == qb::TypeKind::INT)
            qb_assert(type0->schema.of_map.type->flags.of_int.is_unsigned == true)
            qb_assert(type0->schema.of_map.type->flags.of_int.res == 1)
            auto type1 = context.block.type_block.get(1);
            qb_assert(type1->kind == qb::TypeKind::STRUCT)
            qb_assert(type1->schema.of_struct.n_fields == 3)
            qb_assert(type1->schema.of_struct.fields[0]->kind == qb::TypeKind::BOOL)
            qb_assert(type1->schema.of_struct.fields[1] == type0)
            qb_assert(type1->schema.of_struct.fields[2]->kind == qb::TypeKind::INT)
            qb_assert(type1->schema.of_struct.fields[2]->flags.of_int.is_unsigned == true)
            qb_assert(type1->schema.of_struct.fields[2]->flags.of_int.res == 1)

            delete res.out.code;
        })
    
        qb_test("Consts", {
            
            MAKE_CONTEXT({
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_CONST, B_TYPE_U16, 0x12, 0x34,
                qb::OpCode::ADD_CONST, B_TYPE_U32, 0x12, 0x34, 0x56, 0x78,
                qb::OpCode::ADD_CONST, B_TYPE_STR, 0x00, 0x04, 't', 'e', 's', 't',
                qb::OpCode::ADD_TYPE, qb::TypeKind::VECTOR, false, B_TYPE_U16,
                qb::OpCode::ADD_CONST, 0, 0x00, 0x09, 0x01, 0x00, 0x03, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC,
            });
            bool init_res = context.init();
            qb_assert(init_res == true)
            qb_assert(context.block.data.size() == 5)
            qb_assert(*context.block.data.__cpp_get<uint16_t>(1) == 0x1234)
            qb_assert(*context.block.data.__cpp_get<uint32_t>(2) == 0x12345678)
            qb_assert(*context.block.data.__cpp_get<std::string>(3) == "test")
            qb_assert(*context.block.data.__cpp_get<qb::mem::Vector>(4)->__cpp_get<uint16_t>(0) == 0x1234)
            qb_assert(*context.block.data.__cpp_get<qb::mem::Vector>(4)->__cpp_get<uint16_t>(1) == 0x5678)
            qb_assert(*context.block.data.__cpp_get<qb::mem::Vector>(4)->__cpp_get<uint16_t>(2) == 0x9ABC)
            delete res.out.code;
        })
        
        qb_test("Args/Vars", {
            MAKE_CONTEXT({
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_ARG, B_TYPE_U8,
                qb::OpCode::ADD_ARG, B_TYPE_BOOL,
                qb::OpCode::ADD_VAR, B_TYPE_STR,
                qb::OpCode::ADD_VAR, B_TYPE_REF,
            });
            qb_assert(context.block.data.fields.size() == 5)
            // return
            qb_assert(context.block.data.type->schema.of_struct.fields[0]->kind == qb::TypeKind::VOID)
            qb_assert(*context.block.data.__cpp_get<uint8_t>(0) == 0)
            // args
            qb_assert(context.block.data.type->schema.of_struct.fields[1]->kind == qb::TypeKind::INT)
            qb_assert(*context.block.data.__cpp_get<uint8_t>(1) == 0)
            qb_assert(context.block.data.type->schema.of_struct.fields[2]->kind == qb::TypeKind::BOOL)
            qb_assert(*context.block.data.__cpp_get<bool>(2) == false)
            // vars
            qb_assert(context.block.data.type->schema.of_struct.fields[3]->kind == qb::TypeKind::STRING)
            qb_assert(*context.block.data.__cpp_get<std::string>(3) == "")
            qb_assert(context.block.data.type->schema.of_struct.fields[4]->kind == qb::TypeKind::REF)
            qb_assert(context.block.data.__cpp_get<qb::mem::Reference>(4)->block == 0)
            qb_assert(context.block.data.__cpp_get<qb::mem::Reference>(4)->port == 0)
            delete res.out.code;
        })

    })
})