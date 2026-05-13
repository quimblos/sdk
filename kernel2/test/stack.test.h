#pragma once
#include "stack.h"

qb_suite(test_stack, "stack", {

    auto solver = qb::TypeSolver();

    qb_describe("Routine", {
    
        qb_test("Vars", {
            
            qb::byte_t code[] = {
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_VAR, B_TYPE_U8,
                qb::OpCode::ADD_VAR, B_TYPE_STR
            };
            auto res = qb::parser::code(code, sizeof(code)/sizeof(qb::byte_t));
            if (res.code != 0) {
                std::cout << qb::i18n::parser.at(res.code) << std::endl;
                qb_fail()
            }
            std::cout << res.out.code->to_str() << std::endl;

            auto routine = qb::exec::Routine(solver, nullptr, res.out.code);

            qb_assert(routine.block.pos.size() == 2)

            delete res.out.code;
        })

    })

    qb_describe("Stack", {
    
        qb_test("Push", {
            
            qb::byte_t code[] = {
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_VAR, B_TYPE_U8,
                qb::OpCode::ADD_VAR, B_TYPE_STR
            };
            auto res = qb::parser::code(code, sizeof(code)/sizeof(qb::byte_t));
            if (res.code != 0) {
                std::cout << qb::i18n::parser.at(res.code) << std::endl;
                qb_fail()
            }
            std::cout << res.out.code->to_str() << std::endl;

            auto stack = qb::exec::Stack(&solver);
            auto routine0 = stack.push(res.out.code);
            qb_assert(routine0->block.pos.size() == 2)
            
            auto routine1 = stack.push(res.out.code);
            qb_assert(routine1->block.pos.size() == 2)
            qb_assert(routine1->parent == routine0)

            delete res.out.code;
        })

    })
})