#pragma once
#include "node.h"

#define MAKE_NODE(CODE...) \
    qb::byte_t bytecode[] = CODE; \
    auto parser_res = qb::parser::code(bytecode, sizeof(bytecode)/sizeof(qb::byte_t)); \
    if (parser_res.code != 0) { \
        std::cout << qb::i18n::parser.at(parser_res.code) << std::endl; \
        qb_fail() \
    } \
    qb::Code* code = parser_res.out.code; \
    auto thread = new qb::Thread(solver, "test", code, {}); \
    auto node = qb::Node("test"); \
    auto link_res = node.link_thread(thread); \
    qb_assert(link_res.code == 0);
    
qb_suite(test_node, "node", {

    auto solver = qb::TypeSolver();

    qb_describe("getters", {
    
        qb_test("name", {
            auto node = qb::Node("test");
            qb_assert(node.get_name() == "test");
        })

    })

    qb_describe("threads", {
    
        qb_test("link", {
            MAKE_NODE({
                HEADER_QUIMBLOS
            })
            auto threads = node.get_threads();
            qb_assert(threads.size() == 1);
            delete code;
        })
    
        qb_test("link twice (should fail)", {
            MAKE_NODE({
                HEADER_QUIMBLOS
            })
            link_res = node.link_thread(thread);
            qb_assert(link_res.code == qb::node::res_t::Code::THREAD_ALREADY_EXISTS);
            delete code;
        })
    
        qb_test("get", {
            MAKE_NODE({
                HEADER_QUIMBLOS
            })
            auto get_res = node.get_thread("test");
            qb_assert(get_res.code == 0);
            qb_assert(get_res.out.thread == thread);
            delete code;
        })

        qb_test("get non-existing (should fail)", {
            MAKE_NODE({
                HEADER_QUIMBLOS
            })
            auto get_res = node.get_thread("nothing");
            qb_assert(get_res.code == qb::node::res_t::Code::THREAD_NOT_FOUND);
            delete code;
        })

        qb_test("delete", {
            MAKE_NODE({
                HEADER_QUIMBLOS
            })
            auto threads = node.get_threads();
            qb_assert(threads.size() == 1);
            auto get_res = node.delete_thread("test");
            qb_assert(get_res.code == 0);
            threads = node.get_threads();
            qb_assert(threads.size() == 0);
            delete code;
        })

        qb_test("delete non-existing (should fail)", {
            MAKE_NODE({
                HEADER_QUIMBLOS
            })
            auto threads = node.get_threads();
            qb_assert(threads.size() == 1);
            auto get_res = node.delete_thread("nothing");
            qb_assert(get_res.code == qb::node::res_t::Code::THREAD_NOT_FOUND);
            threads = node.get_threads();
            qb_assert(threads.size() == 1);
            delete code;
        })

    })
})