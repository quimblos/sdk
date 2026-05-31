#pragma once
#include "engine.h"

qb_suite(test_engine, "engine", {

    qb_describe("getters", {
    
        qb_test("name", {
            auto engine = qb::Engine({});
            qb_assert(&engine != nullptr);
        })

    })

    qb_describe("drivers", {

        qb_test("link", {
            auto engine = qb::Engine({});
            auto driver = new qb::Driver("test", {});
            engine.link_driver(driver);
            auto drivers = engine.get_drivers();
            qb_assert(drivers.size() == 1);
        })

        qb_test("link twice (should fail)", {
            auto engine = qb::Engine({});
            auto driver = new qb::Driver("test", {});
            engine.link_driver(driver);
            auto res = engine.link_driver(driver);
            qb_assert(res.code == qb::engine::res_t::Code::DRIVER_ALREADY_EXISTS);
        })
    
        qb_test("get", {
            auto engine = qb::Engine({});
            auto driver = new qb::Driver("test", {});
            engine.link_driver(driver);
            auto get_res = engine.get_driver("test");
            qb_assert(get_res.code == 0);
            qb_assert(get_res.out.driver == driver);
        })

        qb_test("get non-existing (should fail)", {
            auto engine = qb::Engine({});
            auto driver = new qb::Driver("test", {});
            engine.link_driver(driver);
            auto get_res = engine.get_driver("nothing");
            qb_assert(get_res.code == qb::engine::res_t::Code::DRIVER_NOT_FOUND);
        })

        qb_test("delete", {
            auto engine = qb::Engine({});
            auto driver = new qb::Driver("test", {});
            engine.link_driver(driver);
            auto drivers = engine.get_drivers();
            qb_assert(drivers.size() == 1);
            auto get_res = engine.delete_driver("test");
            qb_assert(get_res.code == 0);
            drivers = engine.get_drivers();
            qb_assert(drivers.size() == 0);
        })

        qb_test("delete non-existing (should fail)", {
            auto engine = qb::Engine({});
            auto driver = new qb::Driver("test", {});
            engine.link_driver(driver);
            auto drivers = engine.get_drivers();
            qb_assert(drivers.size() == 1);
            auto get_res = engine.delete_driver("nothing");
            qb_assert(get_res.code == qb::engine::res_t::Code::DRIVER_NOT_FOUND);
            drivers = engine.get_drivers();
            qb_assert(drivers.size() == 1);
        })
    })

    qb_describe("nodes", {

        qb_test("link", {
            auto engine = qb::Engine({});
            auto node = new qb::Node(&engine, "test", {});
            engine.link_node(node);
            auto nodes = engine.get_nodes();
            qb_assert(nodes.size() == 1);
        })

        qb_test("link twice (should fail)", {
            auto engine = qb::Engine({});
            auto node = new qb::Node(&engine, "test", {});
            engine.link_node(node);
            auto res = engine.link_node(node);
            qb_assert(res.code == qb::engine::res_t::Code::NODE_ALREADY_EXISTS);
        })
    
        qb_test("get", {
            auto engine = qb::Engine({});
            auto node = new qb::Node(&engine, "test", {});
            engine.link_node(node);
            auto get_res = engine.get_node("test");
            qb_assert(get_res.code == 0);
            qb_assert(get_res.out.node == node);
        })

        qb_test("get non-existing (should fail)", {
            auto engine = qb::Engine({});
            auto node = new qb::Node(&engine, "test", {});
            engine.link_node(node);
            auto get_res = engine.get_node("nothing");
            qb_assert(get_res.code == qb::engine::res_t::Code::NODE_NOT_FOUND);
        })

        qb_test("delete", {
            auto engine = qb::Engine({});
            auto node = new qb::Node(&engine, "test", {});
            engine.link_node(node);
            auto nodes = engine.get_nodes();
            qb_assert(nodes.size() == 1);
            auto get_res = engine.delete_node("test");
            qb_assert(get_res.code == 0);
            nodes = engine.get_nodes();
            qb_assert(nodes.size() == 0);
        })

        qb_test("delete non-existing (should fail)", {
            auto engine = qb::Engine({});
            auto node = new qb::Node(&engine, "test", {});
            engine.link_node(node);
            auto nodes = engine.get_nodes();
            qb_assert(nodes.size() == 1);
            auto get_res = engine.delete_node("nothing");
            qb_assert(get_res.code == qb::engine::res_t::Code::NODE_NOT_FOUND);
            nodes = engine.get_nodes();
            qb_assert(nodes.size() == 1);
        })
    })
})