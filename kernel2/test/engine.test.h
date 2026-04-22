#pragma once
#include "engine.h"

class EngineTestDevice: public qb::Device {
    public:
        EngineTestDevice(): qb::Device("led", {
            { "dimmer", qb::data::u8() }
        }) {}
    protected:
        void tick() {}
};    

qb_suite(test_engine, "engine", {

    qb_describe("devices", {
        
        qb_test("link_device", {
            auto engine = qb::Engine();
            auto device = new EngineTestDevice();
            auto res = engine.link_device(device);
            qb_assert(res.ok == true);
        })

        qb_test("get_device", {
            auto engine = qb::Engine();
            auto device = new EngineTestDevice();
            engine.link_device(device);

            qb::Device* out = engine.get_device("led");
            qb_assert(out == device);
        })

        qb_test("delete_device", {
            auto engine = qb::Engine();
            auto device = new EngineTestDevice();
            engine.link_device(device);
            
            qb::Device* out = engine.get_device("led");
            qb_assert(out == device);

            engine.delete_device("led");
            
            out = engine.get_device("led");
            qb_assert(out == nullptr);
        })

    })

    qb_describe("runners", {
        
        qb_test("link_runner", {
            auto engine = qb::Engine();
            auto program = qb::Program("test", {
                new qb::instruction::Log(qb::data::str("test"))
            }, {});
            auto runner = new qb::Runner(&engine, "test", &program);
            auto res = engine.link_runner(runner);
            qb_assert(res.ok == true);
        })

        qb_test("get_runner", {
            auto engine = qb::Engine();
            auto program = qb::Program("test", {
                new qb::instruction::Log(qb::data::str("test"))
            }, {});
            auto runner = new qb::Runner(&engine, "test", &program);
            engine.link_runner(runner);

            qb::Runner* out = engine.get_runner("test");
            qb_assert(out == runner);
        })

        qb_test("delete_runner", {
            auto engine = qb::Engine();
            auto program = qb::Program("test", {
                new qb::instruction::Log(qb::data::str("test"))
            }, {});
            auto runner = new qb::Runner(&engine, "test", &program);
            engine.link_runner(runner);
            
            qb::Runner* out = engine.get_runner("test");
            qb_assert(out == runner);

            engine.delete_runner("test");
            
            out = engine.get_runner("test");
            qb_assert(out == nullptr);
        })

    })

})