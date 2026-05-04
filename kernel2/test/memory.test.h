#pragma once
#include "memory.h"

qb_suite(test_memory, "memory", {

    auto solver = qb::TypeSolver();

    qb_describe("Block: built-in", {
    
        qb_test("void", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_VOID
            });

            auto data = block.__cpp_get<qb::void_t>(0);
            // qb_assert(sizeof(data) == 1);
            qb_assert(*data == 0);
        })
    
        qb_test("null", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_NULL
            });
            
            auto data = block.__cpp_get<qb::void_t>(0);
            // qb_assert(sizeof(data) == 1);
            qb_assert(*data == 0);
        })
    
        qb_test("bool", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_BOOL
            });
            block.__cpp_set(0, true);

            auto data = block.__cpp_get<bool>(0);
            qb_assert(*data == true);
        })
    
        qb_test("u8", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_U8
            });
            block.__cpp_set(0, (uint8_t) 123);

            auto data = block.__cpp_get<uint8_t>(0);
            qb_assert(*data == 123);
        })
    
        qb_test("i8", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_I8
            });
            block.__cpp_set(0, (int8_t) -123);

            auto data = block.__cpp_get<int8_t>(0);
            qb_assert(*data == -123);
        })
    
        qb_test("u16", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_U16
            });
            block.__cpp_set(0, (uint16_t) 1234);

            auto data = block.__cpp_get<uint16_t>(0);
            qb_assert(*data == 1234);
        })
    
        qb_test("i16", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_I16
            });
            block.__cpp_set(0, (int16_t) -1234);

            auto data = block.__cpp_get<int16_t>(0);
            qb_assert(*data == -1234);
        })
    
        qb_test("u32", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_U32
            });
            block.__cpp_set(0, (uint32_t) 123456);

            auto data = block.__cpp_get<uint32_t>(0);
            qb_assert(*data == 123456);
        })
    
        qb_test("i32", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_I32
            });
            block.__cpp_set(0, (int32_t) -123456);

            auto data = block.__cpp_get<int32_t>(0);
            qb_assert(*data == -123456);
        })
    
        qb_test("f32", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_F32
            });
            block.__cpp_set(0, (float) -12.34);

            auto data = block.__cpp_get<float>(0);
            qb_assert(abs(*data - -12.34) < 0.000000000000000001);
        })
    
        qb_test("str", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_STR
            });
            block.__cpp_set(0, std::string("test"));

            auto data = block.__cpp_get<std::string>(0);
            qb_assert(*data == "test");
        })
    
        qb_test("ref", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_REF
            });
            block.__cpp_set(0, qb::mem::Reference(0xFF, 0x01, false, false));

            auto data = block.__cpp_get<qb::mem::Reference>(0);
            qb_assert(data->to_str() == "@255.1");
        })

        qb_test("multiple", {
            auto block = qb::mem::Block(solver, {
                B_TYPE_VOID, // 0
                B_TYPE_NULL, // 1
                B_TYPE_BOOL, // 2
                B_TYPE_U8,   // 3
                B_TYPE_F32,  // 4
                B_TYPE_STR,  // 5
                B_TYPE_REF,  // 6
            });

            block.__cpp_set(2, true);
            block.__cpp_set(3, (uint8_t) 123);
            block.__cpp_set(4, (float) 123.456);
            block.__cpp_set(5, std::string("test"));
            block.__cpp_set(6, qb::mem::Reference(0xFF,0x01,false,false));

            qb_assert(*block.__cpp_get<qb::void_t>(0) == 0);
            qb_assert(*block.__cpp_get<qb::void_t>(1) == 0);
            qb_assert(*block.__cpp_get<bool>(2) == true);
            qb_assert(*block.__cpp_get<uint8_t>(3) == 123);
            qb_assert(abs(*block.__cpp_get<float>(4) - 123.456) < 0.000000000000000001);
            qb_assert(*block.__cpp_get<std::string>(5) == "test");
            qb_assert(block.__cpp_get<qb::mem::Reference>(6)->to_str() == "@255.1");
        })

    })

    qb_describe("Block: vec", {
    
        qb_test("vec[bool]", {
            auto block = qb::mem::Block(solver, {
                solver.add_vec(B_TYPE_BOOL)
            });
            auto data = block.__cpp_get<qb::mem::Vector>(0);
            data->resize(2);
            data->__cpp_set<bool>(0, true);
            data->__cpp_set<bool>(1, false);

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_at<bool>(0) == true);
            qb_assert(*data->__cpp_at<bool>(1) == false);
        })
    
        qb_test("vec[i16]", {
            auto block = qb::mem::Block(solver, {
                solver.add_vec(B_TYPE_I16)
            });
            auto data = block.__cpp_get<qb::mem::Vector>(0);
            data->resize(2);
            data->__cpp_set<int16_t>(0, 123);
            data->__cpp_set<int16_t>(1, -234);

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_at<int16_t>(0) == 123);
            qb_assert(*data->__cpp_at<int16_t>(1) == -234);
        })
    
        qb_test("vec[f32]", {
            auto block = qb::mem::Block(solver, {
                solver.add_vec(B_TYPE_F32)
            });
            auto data = block.__cpp_get<qb::mem::Vector>(0);
            data->resize(2);
            data->__cpp_set<float>(0, 12.34);
            data->__cpp_set<float>(1, -56.78);

            qb_assert(data->size() == 2);
            qb_assert(abs(*data->__cpp_at<float>(0) - 12.34) < 0.000000000000000001);
            qb_assert(abs(*data->__cpp_at<float>(1) - -56.78) < 0.000000000000000001);
        })
    
        qb_test("vec[str]", {
            auto block = qb::mem::Block(solver, {
                solver.add_vec(B_TYPE_STR)
            });
            auto data = block.__cpp_get<qb::mem::Vector>(0);
            data->resize(2);
            data->__cpp_set<std::string>(0, "test");
            data->__cpp_set<std::string>(1, "nice");

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_at<std::string>(0) == "test");
            qb_assert(*data->__cpp_at<std::string>(1) == "nice");
        })
    
        qb_test("vec[ref]", {
            auto block = qb::mem::Block(solver, {
                solver.add_vec(B_TYPE_REF)
            });
            auto data = block.__cpp_get<qb::mem::Vector>(0);
            data->resize(2);
            data->__cpp_set(0, qb::mem::Reference(0xFF, 0x01, false, false));
            data->__cpp_set(1, qb::mem::Reference(0xFF, 0x02, false, false));

            qb_assert(data->size() == 2);
            qb_assert(data->__cpp_at<qb::mem::Reference>(0)->to_str() == "@255.1");
            qb_assert(data->__cpp_at<qb::mem::Reference>(1)->to_str() == "@255.2");
        })
   
    })

    qb_describe("Block: map", {
    
        qb_test("map{bool}", {
            auto block = qb::mem::Block(solver, {
                solver.add_map(B_TYPE_BOOL)
            });
            auto data = block.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set<bool>("a", true);
            data->__cpp_set<bool>("b", false);

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_at<bool>("a") == true);
            qb_assert(*data->__cpp_at<bool>("b") == false);
        })
    
        qb_test("map{i16}", {
            auto block = qb::mem::Block(solver, {
                solver.add_map(B_TYPE_I16)
            });
            auto data = block.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set<int16_t>("a", 123);
            data->__cpp_set<int16_t>("b", -234);

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_at<int16_t>("a") == 123);
            qb_assert(*data->__cpp_at<int16_t>("b") == -234);
        })
    
        qb_test("map{f32}", {
            auto block = qb::mem::Block(solver, {
                solver.add_map(B_TYPE_F32)
            });
            auto data = block.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set<float>("a", 12.34);
            data->__cpp_set<float>("b", -56.78);

            qb_assert(data->size() == 2);
            qb_assert(abs(*data->__cpp_at<float>("a") - 12.34) < 0.000000000000000001);
            qb_assert(abs(*data->__cpp_at<float>("b") - -56.78) < 0.000000000000000001);
        })
    
        qb_test("map{str}", {
            auto block = qb::mem::Block(solver, {
                solver.add_map(B_TYPE_STR)
            });
            auto data = block.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set<std::string>("a", "test");
            data->__cpp_set<std::string>("b", "nice");

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_at<std::string>("a") == "test");
            qb_assert(*data->__cpp_at<std::string>("b") == "nice");
        })
    
        qb_test("map{ref}", {
            auto block = qb::mem::Block(solver, {
                solver.add_map(B_TYPE_REF)
            });
            auto data = block.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set("a", qb::mem::Reference(0xFF, 0x01, false, false));
            data->__cpp_set("b", qb::mem::Reference(0xFF, 0x02, false, false));

            qb_assert(data->size() == 2);
            qb_assert(data->__cpp_at<qb::mem::Reference>("a")->to_str() == "@255.1");
            qb_assert(data->__cpp_at<qb::mem::Reference>("b")->to_str() == "@255.2");
        })
   
    })

    qb_describe("Block: struct", {
    
        qb_test("struct{bool, u8}", {
            auto tdx = solver.add_struct({
                B_TYPE_BOOL,
                B_TYPE_U8
            });
            auto block = qb::mem::Block(solver, { tdx });

            auto _struct = block.__cpp_get<qb::mem::Struct>(0);
            qb_assert(_struct->type == solver.get(tdx));

            _struct->__cpp_set<bool>(0, true);
            auto data0 = _struct->__cpp_get<bool>(0);
            qb_assert(*data0 == true);

            _struct->__cpp_set<uint8_t>(1, 123);
            auto data1 = _struct->__cpp_get<uint8_t>(1);
            qb_assert(*data1 == 123);
        })
        
    })

    qb_describe("Block: event", {
    
        qb_test("event{bool}", {
            auto block = qb::mem::Block(solver, {
                solver.add_event(B_TYPE_BOOL)
            });
            
            auto event = block.__cpp_get<qb::mem::Event>(0);
            event->__cpp_set(
                qb::EventLevel::DEBUG,
                0x12,
                true
            );

            qb_assert(event->level == qb::EventLevel::DEBUG);
            qb_assert(event->code == 0x12);
            qb_assert(*(bool*)event->data == true);
        })
    
        qb_test("event{i16}", {
            auto block = qb::mem::Block(solver, {
                solver.add_event(B_TYPE_I16)
            });
            auto event = block.__cpp_get<qb::mem::Event>(0);
            event->__cpp_set(
                qb::EventLevel::DEBUG,
                0x12,
                -1234
            );

            qb_assert(event->level == qb::EventLevel::DEBUG);
            qb_assert(event->code == 0x12);
            qb_assert(*(int16_t*)event->data == -1234);
        })
    
        qb_test("event{f32}", {
            auto block = qb::mem::Block(solver, {
                solver.add_event(B_TYPE_F32)
            });
            auto event = block.__cpp_get<qb::mem::Event>(0);
            event->__cpp_set(
                qb::EventLevel::DEBUG,
                0x12,
                12.34
            );

            qb_assert(event->level == qb::EventLevel::DEBUG);
            qb_assert(event->code == 0x12);
            qb_assert(abs(*(float*)event->data - 12.34) < 0.000000000000000001);
        })
        
        qb_test("event{str}", {
            auto block = qb::mem::Block(solver, {
                solver.add_event(B_TYPE_STR)
            });
            auto event = block.__cpp_get<qb::mem::Event>(0);
            event->__cpp_set(
                qb::EventLevel::DEBUG,
                0x12,
                std::string("Something went wrong")
            );

            qb_assert(event->level == qb::EventLevel::DEBUG);
            qb_assert(event->code == 0x12);        
            qb_assert(*(std::string*)event->data == "Something went wrong");
        })

        qb_test("event{ref}", {
            auto block = qb::mem::Block(solver, {
                solver.add_event(B_TYPE_REF)
            });
            auto ref = qb::mem::Reference(0xFF, 0x01, false, false);
            auto event = block.__cpp_get<qb::mem::Event>(0);
            event->__cpp_set(
                qb::EventLevel::DEBUG,
                0x12,
                ref
            );

            qb_assert(event->level == qb::EventLevel::DEBUG);
            qb_assert(event->code == 0x12);
            qb_assert(((qb::mem::Reference*)event->data)->block == ref.block);
            qb_assert(((qb::mem::Reference*)event->data)->port == ref.port);
        })
    })
})