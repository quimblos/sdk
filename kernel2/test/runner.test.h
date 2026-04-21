#pragma once
#include "runner.h"
#include "engine.h"
#include <iostream>

class LedDevice: public qb::Device {
    public:
        LedDevice(): qb::Device("led", {
            qb::data::u8()
        }) {}
    protected:
        void tick() {}
};

#define MAKE_RUNNER(CODE...) \
    std::vector<qb::Instruction*> instructions = { \
        CODE \
    }; \
    auto engine = qb::Engine(); \
    engine.put_device(new LedDevice()); \
    auto program = qb::Program("test", instructions, {}); \
    auto runner = new qb::Runner(engine, "test", &program);

qb_suite(test_runner, "runner", {

    qb_describe("constructor", {

        qb_test("use var", {
            MAKE_RUNNER(
                new qb::instruction::UseVar(qb::data::u8())
            )   
            qb_assert(runner->program->variables.size() == 1);
            delete runner;
        })

        qb_test("use device", {
            MAKE_RUNNER(
                new qb::instruction::UseDevice("led")
            )   
            qb_assert(runner->program->devices.size() == 1);
            delete runner;
        })

    })

    qb_describe("instructions", {

        qb_test("Set (device: <runner>)", {
            auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
            MAKE_RUNNER(
                new qb::instruction::UseVar(qb::data::u8(0)),
                new qb::instruction::Set(var0_target, qb::data::u8(123))
            )
            delete var0_target;

            runner->start();
            
            auto var0 = (qb::data::Numeric<uint8_t>*)runner->variables[0];
            qb_assert(var0->value == 0);

            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);

            qb_assert(var0->value);

            delete runner;
        })
        
        qb_test("Set (device: led)", {
            auto var0_target = qb::data::ref(false, 0, 0, 0);
            MAKE_RUNNER(
                new qb::instruction::UseDevice("led"),
                new qb::instruction::Set(var0_target, qb::data::u8(123))
            )
            delete var0_target;

            runner->start();

            qb::Device* device = runner->devices[0];
            auto var0 = (qb::data::Numeric<uint8_t>*) device->get_variable(0);
            qb_assert(var0->value == 0);

            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);

            qb_assert(var0->value == 123);
            delete runner;
        })
        
        qb_test("Hold/Release", {
            MAKE_RUNNER(
                new qb::instruction::UseDevice("led"),
                new qb::instruction::Hold(0),
                new qb::instruction::Release(0)
            )

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            
            qb::Device* device = runner->devices[0];
            qb_assert(device->is_held() == false);

            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);

            qb_assert(device->is_held() == true);

            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);

            qb_assert(device->is_held() == false);
            delete runner;
        })
        
        qb_test("Goto", {
            MAKE_RUNNER(
                new qb::instruction::Goto(3), // 0
                new qb::instruction::Goto(2), // 1
                new qb::instruction::Goto(0), // 2
                new qb::instruction::Goto(1), // 3
            )

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            qb_assert(runner->get_cursor() == 0);
            runner->tick();
            qb_assert(runner->get_cursor() == 3);
            runner->tick();
            qb_assert(runner->get_cursor() == 1);
            runner->tick();
            qb_assert(runner->get_cursor() == 2);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            qb_assert(runner->get_cursor() == 0);
            delete runner;
        })

        qb_test("Branch (true)", {
            MAKE_RUNNER(
                new qb::instruction::Branch(qb::data::_bool(true), 1, 2),  // 0
                new qb::instruction::Return(qb::data::u8(1)),             // 1
                new qb::instruction::Return(qb::data::u8(2)),             // 2
            )

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            qb_assert(runner->get_cursor() == 0);
            runner->tick();
            qb_assert(runner->get_cursor() == 1);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);
            auto output = (qb::data::Numeric<uint8_t>*) runner->get_output();
            qb_assert(output->value = 1);
            delete runner;
        })

        qb_test("Branch (false)", {
            MAKE_RUNNER(
                new qb::instruction::Branch(qb::data::_bool(false), 1, 2),  // 0
                new qb::instruction::Return(qb::data::u8(1)),             // 1
                new qb::instruction::Return(qb::data::u8(2)),             // 2
            )

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            qb_assert(runner->get_cursor() == 0);
            runner->tick();
            qb_assert(runner->get_cursor() == 2);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);
            auto output = (qb::data::Numeric<uint8_t>*) runner->get_output();
            qb_assert(output->value = 2);
            delete runner;
        })

        qb_test("Branch (13)", {
            MAKE_RUNNER(
                new qb::instruction::Branch(qb::data::u8(13), 1, 2),  // 0
                new qb::instruction::Return(qb::data::u8(1)),             // 1
                new qb::instruction::Return(qb::data::u8(2)),             // 2
            )

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            qb_assert(runner->get_cursor() == 0);
            runner->tick();
            qb_assert(runner->get_cursor() == 1);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);
            auto output = (qb::data::Numeric<uint8_t>*) runner->get_output();
            qb_assert(output->value = 1);
            delete runner;
        })

        qb_test("Branch (0)", {
            MAKE_RUNNER(
                new qb::instruction::Branch(qb::data::u8(0), 1, 2),  // 0
                new qb::instruction::Return(qb::data::u8(1)),             // 1
                new qb::instruction::Return(qb::data::u8(2)),             // 2
            )

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            qb_assert(runner->get_cursor() == 0);
            runner->tick();
            qb_assert(runner->get_cursor() == 2);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);
            auto output = (qb::data::Numeric<uint8_t>*) runner->get_output();
            qb_assert(output->value = 2);
            delete runner;
        })

        qb_test("ConditionalSet (EQ true)", {
            auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
            MAKE_RUNNER(
                new qb::instruction::UseVar(qb::data::u8(0)),   // var0
                new qb::instruction::UseVar(qb::data::u8(7)),   // var1
                new qb::instruction::ConditionalSet(qb::InstructionType::SET_IF_EQ,
                    var0_target,
                    qb::data::ref(false, DEVICE_RUNNER, 1, 0),
                    qb::data::u8(7),
                    qb::data::u8(1),
                    qb::data::u8(2)
                )
            )
            delete var0_target;

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);
            auto var0 = (qb::data::Numeric<uint8_t>*) runner->variables.at(0);
            qb_assert(var0->value == 1);
            delete runner;
        })

        qb_test("ConditionalSet (EQ false)", {
            auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
            MAKE_RUNNER(
                new qb::instruction::UseVar(qb::data::u8(0)),   // var0
                new qb::instruction::UseVar(qb::data::u8(7)),   // var1
                new qb::instruction::ConditionalSet(qb::InstructionType::SET_IF_EQ,
                    var0_target,
                    qb::data::ref(false, DEVICE_RUNNER, 1, 0),
                    qb::data::u8(6),
                    qb::data::u8(1),
                    qb::data::u8(2)
                )
            )
            delete var0_target;

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);
            auto var0 = (qb::data::Numeric<uint8_t>*) runner->variables.at(0);
            qb_assert(var0->value == 2);
            delete runner;
        })
        
        qb_test("Sleep", {
            MAKE_RUNNER(
                new qb::instruction::Sleep(1000),
            )

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::SLEEPING);
            qb_assert(runner->get_sleep() == 1000);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::SLEEPING);
            delete runner;
        })
        
        qb_test("Log", {
            MAKE_RUNNER(
                new qb::instruction::Log(qb::data::str("test")),
            )

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            runner->tick();
            // TODO: implement and test logger
            qb_assert(runner->get_state() == qb::runner::State::OK);
            delete runner;
        })
        
        qb_test("Return", {
            MAKE_RUNNER(
                new qb::instruction::Return(qb::data::str("test")),
            )

            qb_assert(runner->get_state() == qb::runner::State::IDLE);
            runner->start();
            qb_assert(runner->get_state() == qb::runner::State::RUNNING);
            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);
            qb::Data* output = runner->get_output();
            qb_assert(output->type == qb::DataType::STRING);
            qb_assert(((qb::data::String*)output)->value == "test");
            delete runner;
        })

    })

})