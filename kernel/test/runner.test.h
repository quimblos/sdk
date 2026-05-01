#pragma once
#include "runner.h"
#include "engine.h"
#include <iostream>

class RunnerTestDevice: public qb::Device {
    public:
        RunnerTestDevice(): qb::Device("led", {
            { "dimmer", qb::data::u8() }
        }) {}
    protected:
        void tick() {}
};

#define MAKE_RUNNER(CODE...) \
    std::vector<qb::Instruction*> instructions = { \
        CODE \
    }; \
    auto engine = qb::Engine(); \
    engine.link_device(new RunnerTestDevice()); \
    auto program = qb::Program("test", instructions, {}); \
    auto runner = new qb::Runner(&engine, "test", &program);

qb_suite(test_runner, "runner", {

    qb_describe("instructions", {

        qb_test("Set (device: <runner>)", {
            auto var0_target = qb::data::Reference(DEVICE_RUNNER, 0, 0, nullptr);
            MAKE_RUNNER(
                new qb::instruction::UseVar(qb::data::u8(0)),
                new qb::instruction::Set(var0_target, qb::data::u8(123))
            )

            runner->start();
            
            auto var0 = (qb::data::Numeric<uint8_t>*)runner->get_variables()[0];
            qb_assert(var0->value == 0);

            runner->tick();
            qb_assert(runner->get_state() == qb::runner::State::OK);

            qb_assert(var0->value == 123);

            delete runner;
        })
        
        // qb_test("Set (device: led)", {
        //     auto var0_target = qb::data::Reference(DEVICE_RUNNER, 0, 0, nullptr);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseDevice("led"),
        //         new qb::instruction::Set(var0_target, qb::data::u8(123))
        //     )

        //     runner->start();

        //     qb::Device* device = runner->get_devices()[0];
        //     auto var0 = (qb::data::Numeric<uint8_t>*) device->get_variable(0);
        //     qb_assert(var0->value == 0);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == 123);
        //     delete runner;
        // })
        
        // qb_test("Hold/Release", {
        //     MAKE_RUNNER(
        //         new qb::instruction::UseDevice("led"),
        //         new qb::instruction::Hold(0),
        //         new qb::instruction::Release(0)
        //     )

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
            
        //     qb::Device* device = runner->get_devices()[0];
        //     qb_assert(device->is_held() == false);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);

        //     qb_assert(device->is_held() == true);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(device->is_held() == false);
        //     delete runner;
        // })
        
        // qb_test("Goto", {
        //     MAKE_RUNNER(
        //         new qb::instruction::Goto(3), // 0
        //         new qb::instruction::Goto(2), // 1
        //         new qb::instruction::Goto(0), // 2
        //         new qb::instruction::Goto(1), // 3
        //     )

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     qb_assert(runner->get_cursor() == 0);
        //     runner->tick();
        //     qb_assert(runner->get_cursor() == 3);
        //     runner->tick();
        //     qb_assert(runner->get_cursor() == 1);
        //     runner->tick();
        //     qb_assert(runner->get_cursor() == 2);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     qb_assert(runner->get_cursor() == 0);
        //     delete runner;
        // })

        // qb_test("Branch (true)", {
        //     MAKE_RUNNER(
        //         new qb::instruction::Branch(qb::data::_bool(true), 1, 2),  // 0
        //         new qb::instruction::Return(qb::data::u8(1)),             // 1
        //         new qb::instruction::Return(qb::data::u8(2)),             // 2
        //     )

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     qb_assert(runner->get_cursor() == 0);
        //     runner->tick();
        //     qb_assert(runner->get_cursor() == 1);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto output = (qb::data::Numeric<uint8_t>*) runner->get_output();
        //     qb_assert(output->value = 1);
        //     delete runner;
        // })

        // qb_test("Branch (false)", {
        //     MAKE_RUNNER(
        //         new qb::instruction::Branch(qb::data::_bool(false), 1, 2),  // 0
        //         new qb::instruction::Return(qb::data::u8(1)),             // 1
        //         new qb::instruction::Return(qb::data::u8(2)),             // 2
        //     )

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     qb_assert(runner->get_cursor() == 0);
        //     runner->tick();
        //     qb_assert(runner->get_cursor() == 2);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto output = (qb::data::Numeric<uint8_t>*) runner->get_output();
        //     qb_assert(output->value = 2);
        //     delete runner;
        // })

        // qb_test("Branch (13)", {
        //     MAKE_RUNNER(
        //         new qb::instruction::Branch(qb::data::u8(13), 1, 2),  // 0
        //         new qb::instruction::Return(qb::data::u8(1)),             // 1
        //         new qb::instruction::Return(qb::data::u8(2)),             // 2
        //     )

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     qb_assert(runner->get_cursor() == 0);
        //     runner->tick();
        //     qb_assert(runner->get_cursor() == 1);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto output = (qb::data::Numeric<uint8_t>*) runner->get_output();
        //     qb_assert(output->value = 1);
        //     delete runner;
        // })

        // qb_test("Branch (0)", {
        //     MAKE_RUNNER(
        //         new qb::instruction::Branch(qb::data::u8(0), 1, 2),  // 0
        //         new qb::instruction::Return(qb::data::u8(1)),             // 1
        //         new qb::instruction::Return(qb::data::u8(2)),             // 2
        //     )

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     qb_assert(runner->get_cursor() == 0);
        //     runner->tick();
        //     qb_assert(runner->get_cursor() == 2);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto output = (qb::data::Numeric<uint8_t>*) runner->get_output();
        //     qb_assert(output->value = 2);
        //     delete runner;
        // })

        // qb_test("ConditionalSet: EQ (true)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::u8(0)),   // var0
        //         new qb::instruction::UseVar(qb::data::u8(7)),   // var1
        //         new qb::instruction::ConditionalSet(qb::InstructionType::SET_IF_EQ,
        //             var0_target,
        //             qb::data::ref(false, DEVICE_RUNNER, 1, 0),
        //             qb::data::u8(7),
        //             qb::data::u8(1),
        //             qb::data::u8(2)
        //         )
        //     )
        //     delete var0_target;

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto var0 = (qb::data::Numeric<uint8_t>*) runner->get_variables().at(0);
        //     qb_assert(var0->value == 1);
        //     delete runner;
        // })

        // qb_test("ConditionalSet: EQ (false)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::u8(0)),   // var0
        //         new qb::instruction::UseVar(qb::data::u8(7)),   // var1
        //         new qb::instruction::ConditionalSet(qb::InstructionType::SET_IF_EQ,
        //             var0_target,
        //             qb::data::ref(false, DEVICE_RUNNER, 1, 0),
        //             qb::data::u8(6),
        //             qb::data::u8(1),
        //             qb::data::u8(2)
        //         )
        //     )
        //     delete var0_target;

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto var0 = (qb::data::Numeric<uint8_t>*) runner->get_variables().at(0);
        //     qb_assert(var0->value == 2);
        //     delete runner;
        // })

        // qb_test("ConditionalSet: LT (true)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::u8(0)),   // var0
        //         new qb::instruction::UseVar(qb::data::u8(7)),   // var1
        //         new qb::instruction::ConditionalSet(qb::InstructionType::SET_IF_LT,
        //             var0_target,
        //             qb::data::ref(false, DEVICE_RUNNER, 1, 0),
        //             qb::data::u8(9),
        //             qb::data::u8(1),
        //             qb::data::u8(2)
        //         )
        //     )
        //     delete var0_target;

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto var0 = (qb::data::Numeric<uint8_t>*) runner->get_variables().at(0);
        //     qb_assert(var0->value == 1);
        //     delete runner;
        // })

        // qb_test("ConditionalSet: LT (false)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::u8(0)),   // var0
        //         new qb::instruction::UseVar(qb::data::u8(7)),   // var1
        //         new qb::instruction::ConditionalSet(qb::InstructionType::SET_IF_LT,
        //             var0_target,
        //             qb::data::ref(false, DEVICE_RUNNER, 1, 0),
        //             qb::data::u8(4),
        //             qb::data::u8(1),
        //             qb::data::u8(2)
        //         )
        //     )
        //     delete var0_target;

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto var0 = (qb::data::Numeric<uint8_t>*) runner->get_variables().at(0);
        //     qb_assert(var0->value == 2);
        //     delete runner;
        // })

        // qb_test("ConditionalSet: GT (true)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::u8(0)),   // var0
        //         new qb::instruction::UseVar(qb::data::u8(7)),   // var1
        //         new qb::instruction::ConditionalSet(qb::InstructionType::SET_IF_GT,
        //             var0_target,
        //             qb::data::ref(false, DEVICE_RUNNER, 1, 0),
        //             qb::data::u8(4),
        //             qb::data::u8(1),
        //             qb::data::u8(2)
        //         )
        //     )
        //     delete var0_target;

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto var0 = (qb::data::Numeric<uint8_t>*) runner->get_variables().at(0);
        //     qb_assert(var0->value == 1);
        //     delete runner;
        // })

        // qb_test("ConditionalSet: GT (false)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::u8(0)),   // var0
        //         new qb::instruction::UseVar(qb::data::u8(7)),   // var1
        //         new qb::instruction::ConditionalSet(qb::InstructionType::SET_IF_GT,
        //             var0_target,
        //             qb::data::ref(false, DEVICE_RUNNER, 1, 0),
        //             qb::data::u8(9),
        //             qb::data::u8(1),
        //             qb::data::u8(2)
        //         )
        //     )
        //     delete var0_target;

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     auto var0 = (qb::data::Numeric<uint8_t>*) runner->get_variables().at(0);
        //     qb_assert(var0->value == 2);
        //     delete runner;
        // })

        // qb_test("Arithmetic: NOT", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::_bool(true)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::NOT, var0_target, qb::data::_bool(true))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<bool>*)runner->get_variables()[0];
        //     qb_assert(var0->value == true);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == false);

        //     delete runner;
        // })

        // qb_test("Arithmetic: NOT (self)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::_bool(false)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::NOT, var0_target, qb::data::ref(false, DEVICE_RUNNER, 0, 0))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<bool>*)runner->get_variables()[0];
        //     qb_assert(var0->value == false);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == true);

        //     delete runner;
        // })
        
        // qb_test("Arithmetic: AND", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::_bool(true)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::AND, var0_target, qb::data::_bool(false))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<bool>*)runner->get_variables()[0];
        //     qb_assert(var0->value == true);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == false);

        //     delete runner;
        // })
        
        // qb_test("Arithmetic: OR", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::_bool(false)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::OR, var0_target, qb::data::_bool(true))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<bool>*)runner->get_variables()[0];
        //     qb_assert(var0->value == false);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == true);

        //     delete runner;
        // })

        // qb_test("Arithmetic: ADD", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::u16(123)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::ADD, var0_target, qb::data::u16(456))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<uint16_t>*)runner->get_variables()[0];
        //     qb_assert(var0->value == 123);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == 579);

        //     delete runner;
        // })

        // qb_test("Arithmetic: SUB", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::i16(123)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::SUB, var0_target, qb::data::u16(456))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<int16_t>*)runner->get_variables()[0];
        //     qb_assert(var0->value == 123);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == -333);

        //     delete runner;
        // })

        // qb_test("Arithmetic: MULT", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::f32(12.34)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::MULT, var0_target, qb::data::u16(2))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<float>*)runner->get_variables()[0];
        //     qb_assert(var0->value == 12.34f);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == 12.34f*((uint16_t)2));

        //     delete runner;
        // })

        // qb_test("Arithmetic: DIV", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::f32(12.34)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::DIV, var0_target, qb::data::u16(2))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<float>*)runner->get_variables()[0];
        //     qb_assert(var0->value == 12.34f);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == 12.34f/((uint16_t)2));

        //     delete runner;
        // })

        // qb_test("Arithmetic: MOD (int)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::u8(10)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::MOD, var0_target, qb::data::u8(3))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<uint8_t>*)runner->get_variables()[0];
        //     qb_assert(var0->value == 10);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == 1);

        //     delete runner;
        // })

        // qb_test("Arithmetic: MOD (float)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::f32(0.9)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::MOD, var0_target, qb::data::f32(0.2))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<float>*)runner->get_variables()[0];
        //     qb_assert(var0->value == 0.9f);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(std::fabs(var0->value - 0.1f) < 0.0000001f);

        //     delete runner;
        // })

        // qb_test("Arithmetic: POW (int)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::u8(3)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::POW, var0_target, qb::data::u8(4))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<uint8_t>*)runner->get_variables()[0];
        //     qb_assert(var0->value == 3);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(var0->value == 81);

        //     delete runner;
        // })

        // qb_test("Arithmetic: POW (float)", {
        //     auto var0_target = qb::data::ref(false, DEVICE_RUNNER, 0, 0);
        //     MAKE_RUNNER(
        //         new qb::instruction::UseVar(qb::data::f32(12.34)),
        //         new qb::instruction::Arithmetic(qb::InstructionType::POW, var0_target, qb::data::f32(-0.5))
        //     )
        //     delete var0_target;

        //     runner->start();
            
        //     auto var0 = (qb::data::Numeric<float>*)runner->get_variables()[0];
        //     qb_assert(var0->value == 12.34f);

        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);

        //     qb_assert(std::fabs(var0->value - 0.284670471f) < 0.0000001f);

        //     delete runner;
        // })

        // qb_test("Sleep", {
        //     MAKE_RUNNER(
        //         new qb::instruction::Sleep(qb::data::u32(1000)),
        //     )

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::SLEEPING);
        //     std::cout << runner->get_sleep() << std::endl;
        //     qb_assert(runner->get_sleep() == 1000);
        //     qb_assert(runner->get_cursor() == 1);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::SLEEPING);
        //     runner->wakeup();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     delete runner;
        // })
        
        // qb_test("Log", {
        //     MAKE_RUNNER(
        //         new qb::instruction::Log(qb::data::str("test")),
        //     )

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     // TODO: implement and test logger
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     delete runner;
        // })
        
        // qb_test("Return", {
        //     MAKE_RUNNER(
        //         new qb::instruction::Return(qb::data::str("test")),
        //     )

        //     qb_assert(runner->get_state() == qb::runner::State::IDLE);
        //     runner->start();
        //     qb_assert(runner->get_state() == qb::runner::State::RUNNING);
        //     runner->tick();
        //     qb_assert(runner->get_state() == qb::runner::State::OK);
        //     qb::Data* output = runner->get_output();
        //     qb_assert(output->type == qb::DataType::STRING);
        //     qb_assert(((qb::data::String*)output)->value == "test");
        //     delete runner;
        // })

    })

})