#pragma once
#include "operator.h"
#include "i18n.h"

qb_suite(test_operator, "operator", {
    
    auto solver = qb::TypeSolver();

    qb_describe("assign", {
        
        qb_test("INT <- INT", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_U8,
                B_TYPE_U8,
            });
            block.__cpp_set(0, (uint8_t) 111);
            block.__cpp_set(1, (uint8_t) 222);

            auto data0 = block.__cpp_get<uint8_t>(0);
            auto data1 = block.__cpp_get<uint8_t>(1);
            qb_assert(*data0 == 111);
            qb_assert(*data1 == 222);
            
            qb::op::assign(block, 0, block, 1);
            qb_assert(*data0 == 222);
            qb_assert(*data1 == 222);
        })
        
        qb_test("INT <- STR: explicit", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_U8,
                B_TYPE_STR,
            });
            block.__cpp_set(0, (uint8_t) 111);
            block.__cpp_set(1, (std::string) "222");

            auto data0 = block.__cpp_get<uint8_t>(0);
            auto data1 = block.__cpp_get<std::string>(1);
            qb_assert(*data0 == 111);
            qb_assert(*data1 == "222");
            
            qb::op::assign(block, 0, block, 1, true);
            qb_assert(*data0 == 222);
            qb_assert(*data1 == "222");
        })

    })

    qb_describe("math: dec", {
        
        qb_test("INT + INT", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_U8,
                B_TYPE_U8,
            });
            block.__cpp_set(0, (uint8_t) 11);
            block.__cpp_set(1, (uint8_t) 22);

            auto data0 = block.__cpp_get<uint8_t>(0);
            auto data1 = block.__cpp_get<uint8_t>(1);
            qb_assert(*data0 == 11);
            qb_assert(*data1 == 22);
            
            qb::op::math(qb::instruction::Math::Flags::Op::ADD, block, 0, block, 1);
            qb_assert(*data0 == 33);
            qb_assert(*data1 == 22);
        })
        
    })
})