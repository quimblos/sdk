#pragma once
#include "thread.h"

#define MAKE_THREAD(CODE...) \
    qb::byte_t bytecode[] = CODE; \
    auto res = qb::parser::code(bytecode, sizeof(bytecode)/sizeof(qb::byte_t)); \
    if (res.code != 0) { \
        std::cout << qb::i18n::parser.at(res.code) << std::endl; \
        qb_fail() \
    } \
    qb::Code* code = res.out.code; \
    auto engine = qb::Engine({}); \
    auto node = qb::Node(&engine, "test", {}); \
    auto thread = qb::Thread(&node, "test", code, {});

qb_suite(test_thread, "thread", {

    qb_describe("State", {
    
        qb_test("init", {
            MAKE_THREAD({
                HEADER_QUIMBLOS
            })
            qb_assert(thread.get_state() == qb::Thread::State::IDLE);
            delete code;
        })
        
        qb_test("reset", {
            MAKE_THREAD({
                HEADER_QUIMBLOS
            })
            qb_assert(thread.get_state() == qb::Thread::State::IDLE);
            thread.reset();
            qb_assert(thread.get_state() == qb::Thread::State::IDLE);
            delete code;
        })

        qb_test("wakeup", {
            MAKE_THREAD({
                HEADER_QUIMBLOS
            })
            qb_assert(thread.get_state() == qb::Thread::State::IDLE);
            thread.wakeup();
            qb_assert(thread.get_state() == qb::Thread::State::RUNNING);
            delete code;
        })
        
        qb_test("start", {
            MAKE_THREAD({
                HEADER_QUIMBLOS
            })
            qb_assert(thread.get_state() == qb::Thread::State::IDLE);
            thread.start();
            qb_assert(thread.get_state() == qb::Thread::State::RUNNING);
            delete code;
        })

    })

    qb_describe("Tick", {
    
        qb_test("false if not running", {
            MAKE_THREAD({
                HEADER_QUIMBLOS
            })
            qb_assert(thread.get_state() == qb::Thread::State::IDLE);
            bool out = thread.tick();
            qb_assert(out == false);
            delete code;
        })
    
        qb_test("false if empty code", {
            MAKE_THREAD({
                HEADER_QUIMBLOS
            })
            qb_assert(thread.get_state() == qb::Thread::State::IDLE);
            thread.start();
            bool out = thread.tick();
            qb_assert(out == false);
            delete code;
        })
    
        qb_test("true if code", {
            MAKE_THREAD({
                HEADER_QUIMBLOS,
                qb::OpCode::ADD_VAR, B_TYPE_U32,
                qb::OpCode::SLEEP, BLOCK_METHOD, 0x01,
            })
            qb_assert(thread.get_state() == qb::Thread::State::IDLE);
            thread.start();
            bool out = thread.tick();
            qb_assert(out == true);
            out = thread.tick();
            qb_assert(out == false);
            delete code;
        })

    })
})