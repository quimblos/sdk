#pragma once
#include "stack.h"

qb_suite(test_stack, "stack", {

    qb_describe("Stack", {
    
        qb_test("Push", {
            
            qb::byte_t code[] = {
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_ARG, B_TYPE_U8,
                qb::OpCode::ADD_VAR, B_TYPE_STR
            };
            auto res = qb::parser::code(code, sizeof(code)/sizeof(qb::byte_t));
            if (res.code != 0) {
                std::cout << qb::i18n::parser.at(res.code) << std::endl;
                qb_fail()
            }

            auto stack = qb::Stack();
            auto method0 = stack.push(res.out.code);
            qb_assert(method0->block.data.pos.size() == 3)
            
            auto method1 = stack.push(res.out.code);
            qb_assert(method1->block.data.pos.size() == 3)
            qb_assert(method1->parent == method0)

            delete res.out.code;
        })
    
        qb_test("Pop", {
            
            qb::byte_t code[] = {
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_ARG, B_TYPE_U8,
                qb::OpCode::ADD_VAR, B_TYPE_STR
            };
            auto res = qb::parser::code(code, sizeof(code)/sizeof(qb::byte_t));
            if (res.code != 0) {
                std::cout << qb::i18n::parser.at(res.code) << std::endl;
                qb_fail()
            }

            auto stack = qb::Stack();
            auto method0 = stack.push(res.out.code);
            auto method1 = stack.push(res.out.code);
            
            qb_assert(stack.get_size() == 2)
            qb_assert(stack.get_root() == method1)
            
            stack.pop();
            qb_assert(stack.get_size() == 1)
            qb_assert(stack.get_root() == method0)

            stack.pop();
            qb_assert(stack.get_size() == 0)
            qb_assert(stack.get_root() == nullptr)

            delete res.out.code;
        })

    })
})