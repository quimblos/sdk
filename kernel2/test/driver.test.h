#pragma once
#include "driver.h"

#define MAKE_DRIVER_THREAD(NAME, CODE...) \
    qb::Code* code_##NAME; \
    { \
        qb::byte_t bytecode[] = CODE; \
        auto parser_res = qb::parser::code(bytecode, sizeof(bytecode)/sizeof(qb::byte_t)); \
        if (parser_res.code != 0) { \
            std::cout << qb::i18n::parser.at(parser_res.code) << std::endl; \
            qb_fail() \
        } \
        code_##NAME = parser_res.out.code; \
    } \
    auto engine_##NAME = qb::Engine({}); \
    auto node_##NAME = qb::Node(&engine_##NAME, "test", {}); \
    auto NAME = qb::Thread(&node_##NAME, "test", code_##NAME, {});

qb_suite(test_driver, "driver", {

    qb_describe("getters", {
    
        qb_test("name", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_BOOL),
                qb::TypeDef::_use(B_TYPE_U8)
            });
            auto driver = qb::Driver("test", type_def);
            qb_assert(driver.get_name() == "test");
        })

    })

    qb_describe("hold/release", {
    
        qb_test("should start not held", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_BOOL),
                qb::TypeDef::_use(B_TYPE_U8)
            });
            auto driver = qb::Driver("test", type_def);
            qb_assert(driver.is_held() == false);
        })
    
        qb_test("should hold", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_BOOL),
                qb::TypeDef::_use(B_TYPE_U8)
            });
            auto driver = qb::Driver("test", type_def);
            qb_assert(driver.is_held() == false);
            MAKE_DRIVER_THREAD(thread, {
                HEADER_QUIMBLOS
            })
            auto res = driver.hold(&thread);
            qb_assert(res.code == qb::driver::res_t::Code::OK);
            qb_assert(driver.is_held() == true);
            delete code_thread;
        })
    
        qb_test("should hold twice", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_BOOL),
                qb::TypeDef::_use(B_TYPE_U8)
            });
            auto driver = qb::Driver("test", type_def);
            qb_assert(driver.is_held() == false);
            MAKE_DRIVER_THREAD(thread, {
                HEADER_QUIMBLOS
            })
            auto res = driver.hold(&thread);
            qb_assert(res.code == qb::driver::res_t::Code::OK);
            qb_assert(driver.is_held() == true);
            res = driver.hold(&thread);
            qb_assert(res.code == qb::driver::res_t::Code::OK);
            qb_assert(driver.is_held() == true);
            delete code_thread;
        })
    
        qb_test("should not allow hold while held", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_BOOL),
                qb::TypeDef::_use(B_TYPE_U8)
            });
            auto driver = qb::Driver("test", type_def);
            qb_assert(driver.is_held() == false);
            MAKE_DRIVER_THREAD(thread1, {
                HEADER_QUIMBLOS
            })
            MAKE_DRIVER_THREAD(thread2, {
                HEADER_QUIMBLOS
            })
            auto res = driver.hold(&thread1);
            qb_assert(res.code == qb::driver::res_t::Code::OK);
            qb_assert(driver.is_held() == true);
            res = driver.hold(&thread2);
            qb_assert(res.code == qb::driver::res_t::Code::HELD_BY_OTHER_THREAD);
            qb_assert(driver.is_held() == true);
            delete code_thread1;
            delete code_thread2;
        })
    
        qb_test("should fail to release not held", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_BOOL),
                qb::TypeDef::_use(B_TYPE_U8)
            });
            auto driver = qb::Driver("test", type_def);
            qb_assert(driver.is_held() == false);
            MAKE_DRIVER_THREAD(thread, {
                HEADER_QUIMBLOS
            })
            auto res = driver.release(&thread);
            qb_assert(res.code == qb::driver::res_t::Code::NOT_HELD);
            qb_assert(driver.is_held() == false);
            delete code_thread;
        })
    
        qb_test("should release", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_BOOL),
                qb::TypeDef::_use(B_TYPE_U8)
            });
            auto driver = qb::Driver("test", type_def);
            qb_assert(driver.is_held() == false);
            MAKE_DRIVER_THREAD(thread, {
                HEADER_QUIMBLOS
            })
            auto res = driver.hold(&thread);
            qb_assert(res.code == qb::driver::res_t::Code::OK);
            qb_assert(driver.is_held() == true);
            res = driver.release(&thread);
            qb_assert(res.code == qb::driver::res_t::Code::OK);
            qb_assert(driver.is_held() == false);
            delete code_thread;
        })
    
        qb_test("should fail to release held by other", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_BOOL),
                qb::TypeDef::_use(B_TYPE_U8)
            });
            auto driver = qb::Driver("test", type_def);
            qb_assert(driver.is_held() == false);
            MAKE_DRIVER_THREAD(thread1, {
                HEADER_QUIMBLOS
            })
            MAKE_DRIVER_THREAD(thread2, {
                HEADER_QUIMBLOS
            })
            auto res = driver.hold(&thread1);
            qb_assert(res.code == qb::driver::res_t::Code::OK);
            qb_assert(driver.is_held() == true);
            res = driver.release(&thread2);
            qb_assert(res.code == qb::driver::res_t::Code::HELD_BY_OTHER_THREAD);
            qb_assert(driver.is_held() == true);
            delete code_thread1;
            delete code_thread2;
        })

    })
})