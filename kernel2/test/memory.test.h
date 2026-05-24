#pragma once
#include "memory.h"

qb_suite(test_memory, "memory", {

    qb_describe("Block: built-in", {
    
        qb_test("void", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_VOID)
            });
            auto block = qb::mem::Block(type_def);

            auto data = block.data.__cpp_get<qb::void_t>(0);
            qb_assert(*data == 0);
        })
    
        qb_test("null", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_NULL)
            });
            auto block = qb::mem::Block(type_def);
            
            auto data = block.data.__cpp_get<qb::void_t>(0);
            qb_assert(*data == 0);
        })
    
        qb_test("bool", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_BOOL)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, true);

            auto data = block.data.__cpp_get<bool>(0);
            qb_assert(*data == true);
        })
    
        qb_test("u8", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_U8)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, (uint8_t) 123);

            auto data = block.data.__cpp_get<uint8_t>(0);
            qb_assert(*data == 123);
        })
    
        qb_test("i8", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_I8)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, (int8_t) -123);

            auto data = block.data.__cpp_get<int8_t>(0);
            qb_assert(*data == -123);
        })
    
        qb_test("u16", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_U16)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, (uint16_t) 1234);

            auto data = block.data.__cpp_get<uint16_t>(0);
            qb_assert(*data == 1234);
        })
    
        qb_test("i16", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_I16)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, (int16_t) -1234);

            auto data = block.data.__cpp_get<int16_t>(0);
            qb_assert(*data == -1234);
        })
    
        qb_test("u32", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_U32)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, (uint32_t) 123456);

            auto data = block.data.__cpp_get<uint32_t>(0);
            qb_assert(*data == 123456);
        })
    
        qb_test("i32", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_I32)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, (int32_t) -123456);

            auto data = block.data.__cpp_get<int32_t>(0);
            qb_assert(*data == -123456);
        })
    
        qb_test("f32", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_F32)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, (float) -12.34);

            auto data = block.data.__cpp_get<float>(0);
            qb_assert(abs(*data - -12.34) < EPSILON);
        })
    
        qb_test("str", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_STR)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, std::string("test"));

            auto data = block.data.__cpp_get<std::string>(0);
            qb_assert(*data == "test");
        })
    
        qb_test("ref", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_REF)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, qb::mem::Reference(BLOCK_ENGINE, PORT_CONST_TRUE));

            auto data = block.data.__cpp_get<qb::mem::Reference>(0);
            qb_assert(data->to_str() == "<engine.true>");
        })
    
        qb_test("ref_slice", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_REF_SLICE)
            });
            auto block = qb::mem::Block(type_def);
            block.data.__cpp_set(0, qb::mem::SlicedReference(BLOCK_ENGINE, PORT_CONST_TRUE, 2, {
                { .start = 1, .end = 3 },
                { .start = 2, .end = 4 },
            }));

            auto data = block.data.__cpp_get<qb::mem::SlicedReference>(0);
            qb_assert(data->to_str() == "<engine.true>[1:3][2:4]");
        })

        qb_test("multiple", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_use(B_TYPE_VOID), // 0
                qb::TypeDef::_use(B_TYPE_NULL), // 1
                qb::TypeDef::_use(B_TYPE_BOOL), // 2
                qb::TypeDef::_use(B_TYPE_U8),   // 3
                qb::TypeDef::_use(B_TYPE_F32),  // 4
                qb::TypeDef::_use(B_TYPE_STR),  // 5
                qb::TypeDef::_use(B_TYPE_REF),  // 6
            });
            auto block = qb::mem::Block(type_def);

            block.data.__cpp_set(2, true);
            block.data.__cpp_set(3, (uint8_t) 123);
            block.data.__cpp_set(4, (float) 123.456);
            block.data.__cpp_set(5, std::string("test"));
            block.data.__cpp_set(6, qb::mem::Reference(BLOCK_ENGINE,PORT_CONST_TRUE));

            qb_assert(*block.data.__cpp_get<qb::void_t>(0) == 0);
            qb_assert(*block.data.__cpp_get<qb::void_t>(1) == 0);
            qb_assert(*block.data.__cpp_get<bool>(2) == true);
            qb_assert(*block.data.__cpp_get<uint8_t>(3) == 123);
            qb_assert(abs(*block.data.__cpp_get<float>(4) - 123.456) < EPSILON);
            qb_assert(*block.data.__cpp_get<std::string>(5) == "test");
            qb_assert(block.data.__cpp_get<qb::mem::Reference>(6)->to_str() == "<engine.true>");
        })

    })

    qb_describe("Block: vec", {
    
        qb_test("vec<1>[bool]", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                    qb::TypeDef::_use(B_TYPE_BOOL)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Vector>(0);
            data->resize({2});
            data->__cpp_set<bool>(0, true);
            data->__cpp_set<bool>(1, false);

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_get<bool>(0) == true);
            qb_assert(*data->__cpp_get<bool>(1) == false);
        })
    
        qb_test("vec<1>[i16]", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                    qb::TypeDef::_use(B_TYPE_I16)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Vector>(0);
            data->resize({2});
            data->__cpp_set<int16_t>(0, 123);
            data->__cpp_set<int16_t>(1, -234);
            
            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_get<int16_t>(0) == 123);
            qb_assert(*data->__cpp_get<int16_t>(1) == -234);
        })
    
        qb_test("vec<1>[f32]", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                    qb::TypeDef::_use(B_TYPE_F32)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Vector>(0);
            data->resize({2});
            data->__cpp_set<float>(0, 12.34);
            data->__cpp_set<float>(1, -56.78);

            qb_assert(data->size() == 2);
            qb_assert(abs(*data->__cpp_get<float>(0) - 12.34) < EPSILON);
            qb_assert(abs(*data->__cpp_get<float>(1) - -56.78) < EPSILON);
        })
    
        qb_test("vec<1>[str]", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                    qb::TypeDef::_use(B_TYPE_STR)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Vector>(0);
            data->resize({2});
            data->__cpp_set<std::string>(0, "test");
            data->__cpp_set<std::string>(1, "nice");

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_get<std::string>(0) == "test");
            qb_assert(*data->__cpp_get<std::string>(1) == "nice");
        })
    
        qb_test("vec<1>[ref]", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                    qb::TypeDef::_use(B_TYPE_REF)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Vector>(0);
            data->resize({2});
            data->__cpp_set(0, qb::mem::Reference(BLOCK_ENGINE, PORT_CONST_TRUE));
            data->__cpp_set(1, qb::mem::Reference(BLOCK_NODE, 0x02));

            qb_assert(data->size() == 2);
            qb_assert(data->__cpp_get<qb::mem::Reference>(0)->to_str() == "<engine.true>");
            qb_assert(data->__cpp_get<qb::mem::Reference>(1)->to_str() == "<node.2>");
        })
   
    })

    qb_describe("Block: map", {
    
        qb_test("map{bool}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::MAP, {
                    qb::TypeDef::_use(B_TYPE_BOOL)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set<bool>("a", true);
            data->__cpp_set<bool>("b", false);

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_get<bool>("a") == true);
            qb_assert(*data->__cpp_get<bool>("b") == false);
        })
    
        qb_test("map{i16}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::MAP, {
                    qb::TypeDef::_use(B_TYPE_I16)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set<int16_t>("a", 123);
            data->__cpp_set<int16_t>("b", -234);

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_get<int16_t>("a") == 123);
            qb_assert(*data->__cpp_get<int16_t>("b") == -234);
        })
    
        qb_test("map{f32}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::MAP, {
                    qb::TypeDef::_use(B_TYPE_F32)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set<float>("a", 12.34);
            data->__cpp_set<float>("b", -56.78);

            qb_assert(data->size() == 2);
            qb_assert(abs(*data->__cpp_get<float>("a") - 12.34) < EPSILON);
            qb_assert(abs(*data->__cpp_get<float>("b") - -56.78) < EPSILON);
        })
    
        qb_test("map{str}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::MAP, {
                    qb::TypeDef::_use(B_TYPE_STR)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set<std::string>("a", "test");
            data->__cpp_set<std::string>("b", "nice");

            qb_assert(data->size() == 2);
            qb_assert(*data->__cpp_get<std::string>("a") == "test");
            qb_assert(*data->__cpp_get<std::string>("b") == "nice");
        })
    
        qb_test("map{ref}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::MAP, {
                    qb::TypeDef::_use(B_TYPE_REF)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Map>(0);
            data->__cpp_set("a", qb::mem::Reference(BLOCK_ENGINE, PORT_CONST_TRUE));
            data->__cpp_set("b", qb::mem::Reference(BLOCK_NODE, 0x02));

            qb_assert(data->size() == 2);
            qb_assert(data->__cpp_get<qb::mem::Reference>("a")->to_str() == "<engine.true>");
            qb_assert(data->__cpp_get<qb::mem::Reference>("b")->to_str() == "<node.2>");
        })
   
    })

    qb_describe("Block: struct", {
    
        qb_test("struct{bool, u8}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::STRUCT, {
                    qb::TypeDef::_use(B_TYPE_BOOL),
                    qb::TypeDef::_use(B_TYPE_U8)
                })
            });
            auto block = qb::mem::Block(type_def);

            auto _struct = block.data.__cpp_get<qb::mem::Struct>(0);
            qb_assert(_struct->type == block.types.get(0));

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
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::EVENT, {
                    qb::TypeDef::_use(B_TYPE_BOOL)
                })
            });
            auto block = qb::mem::Block(type_def);
            
            auto event = block.data.__cpp_get<qb::mem::Event>(0);
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
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::EVENT, {
                    qb::TypeDef::_use(B_TYPE_I16)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto event = block.data.__cpp_get<qb::mem::Event>(0);
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
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::EVENT, {
                    qb::TypeDef::_use(B_TYPE_F32)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto event = block.data.__cpp_get<qb::mem::Event>(0);
            event->__cpp_set(
                qb::EventLevel::DEBUG,
                0x12,
                12.34
            );

            qb_assert(event->level == qb::EventLevel::DEBUG);
            qb_assert(event->code == 0x12);
            qb_assert(abs(*(float*)event->data - 12.34) < EPSILON);
        })
        
        qb_test("event{str}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::EVENT, {
                    qb::TypeDef::_use(B_TYPE_STR)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto event = block.data.__cpp_get<qb::mem::Event>(0);
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
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::EVENT, {
                    qb::TypeDef::_use(B_TYPE_REF)
                })
            });
            auto block = qb::mem::Block(type_def);
            auto ref = qb::mem::Reference(BLOCK_ENGINE, PORT_CONST_TRUE);
            auto event = block.data.__cpp_get<qb::mem::Event>(0);
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

    qb_describe("Block: complex vec", {
    
        qb_test("vec<1>[vec<1>[u8]]", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                    qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto data = block.data.__cpp_get<qb::mem::Vector>(0);
            data->resize({2});
            
            auto inner0 = data->__cpp_get<qb::mem::Vector>(0);
            inner0->resize({3});
            inner0->__cpp_set<uint8_t>(0, 10);
            inner0->__cpp_set<uint8_t>(1, 11);
            inner0->__cpp_set<uint8_t>(2, 12);
            
            auto inner1 = data->__cpp_get<qb::mem::Vector>(1);
            inner1->resize({4});
            inner1->__cpp_set<uint8_t>(0, 13);
            inner1->__cpp_set<uint8_t>(1, 14);
            inner1->__cpp_set<uint8_t>(2, 15);
            inner1->__cpp_set<uint8_t>(3, 16);

            qb_assert(data->size() == 2);
            qb_assert(inner0->size() == 3);
            qb_assert(*inner0->__cpp_get<uint8_t>(0) == 10);
            qb_assert(*inner0->__cpp_get<uint8_t>(1) == 11);
            qb_assert(*inner0->__cpp_get<uint8_t>(2) == 12);
            qb_assert(inner1->size() == 4);
            qb_assert(*inner1->__cpp_get<uint8_t>(0) == 13);
            qb_assert(*inner1->__cpp_get<uint8_t>(1) == 14);
            qb_assert(*inner1->__cpp_get<uint8_t>(2) == 15);
            qb_assert(*inner1->__cpp_get<uint8_t>(3) == 16);
        })
    
        qb_test("vec<1>[map{u8}]", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                    qb::TypeDef::_add(qb::TypeKind::MAP, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Vector>(0);
            data->resize({2});

            data->__cpp_set<qb::mem::Map>(0, qb::mem::Map(type));
            auto inner0 = data->__cpp_get<qb::mem::Map>(0);
            inner0->__cpp_set<uint8_t>("a", 10);
            inner0->__cpp_set<uint8_t>("b", 11);
            inner0->__cpp_set<uint8_t>("c", 12);
            
            data->__cpp_set<qb::mem::Map>(1, qb::mem::Map(type));
            auto inner1 = data->__cpp_get<qb::mem::Map>(1);
            inner1->__cpp_set<uint8_t>("d", 13);
            inner1->__cpp_set<uint8_t>("e", 14);
            inner1->__cpp_set<uint8_t>("f", 15);
            inner1->__cpp_set<uint8_t>("g", 16);

            qb_assert(data->size() == 2);
            qb_assert(inner0->size() == 3);
            qb_assert(*inner0->__cpp_get<uint8_t>("a") == 10);
            qb_assert(*inner0->__cpp_get<uint8_t>("b") == 11);
            qb_assert(*inner0->__cpp_get<uint8_t>("c") == 12);
            qb_assert(inner1->size() == 4);
            qb_assert(*inner1->__cpp_get<uint8_t>("d") == 13);
            qb_assert(*inner1->__cpp_get<uint8_t>("e") == 14);
            qb_assert(*inner1->__cpp_get<uint8_t>("f") == 15);
            qb_assert(*inner1->__cpp_get<uint8_t>("g") == 16);
        })
    
        qb_test("vec<1>[struct{bool,u8}]", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                    qb::TypeDef::_add(qb::TypeKind::STRUCT, {
                        qb::TypeDef::_use(B_TYPE_BOOL),
                        qb::TypeDef::_use(B_TYPE_U8)    
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(0);
            auto data = block.data.__cpp_get<qb::mem::Vector>(0);
            data->resize({2});

            data->__cpp_set<qb::mem::Struct>(0, qb::mem::Struct(type));
            auto inner0 = data->__cpp_get<qb::mem::Struct>(0);
            inner0->__cpp_set<bool>(0, false);
            inner0->__cpp_set<uint8_t>(1, 11);
            
            data->__cpp_set<qb::mem::Struct>(1, qb::mem::Struct(type));
            auto inner1 = data->__cpp_get<qb::mem::Struct>(1);
            inner1->__cpp_set<bool>(0, true);
            inner1->__cpp_set<uint8_t>(1, 22);

            qb_assert(data->size() == 2);
            qb_assert(*inner0->__cpp_get<bool>(0) == false);
            qb_assert(*inner0->__cpp_get<uint8_t>(1) == 11);
            qb_assert(*inner1->__cpp_get<bool>(0) == true);
            qb_assert(*inner1->__cpp_get<uint8_t>(1) == 22);
        })
    
        qb_test("vec[event{u8}]", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                    qb::TypeDef::_add(qb::TypeKind::EVENT, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Vector>(0);
            data->resize({2});

            data->__cpp_set<qb::mem::Event>(0, qb::mem::Event(type));
            auto inner0 = data->__cpp_get<qb::mem::Event>(0);
            inner0->__cpp_set<uint8_t>(qb::EventLevel::INFO, 123, 12);
            
            data->__cpp_set<qb::mem::Event>(1, qb::mem::Event(type));
            auto inner1 = data->__cpp_get<qb::mem::Event>(1);
            inner1->__cpp_set<uint8_t>(qb::EventLevel::WARN, 124, 21);

            qb_assert(data->size() == 2);
            qb_assert(inner0->level == qb::EventLevel::INFO);
            qb_assert(inner0->code == 123);
            qb_assert(*inner0->__cpp_get<uint8_t>() == 12);
            qb_assert(inner1->level == qb::EventLevel::WARN);
            qb_assert(inner1->code == 124);
            qb_assert(*inner1->__cpp_get<uint8_t>() == 21);
        })
    
    })
    
    qb_describe("Block: complex map", {
    
        qb_test("map{vec<1>[u8]}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::MAP, {
                    qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Map>(0);
            
            data->__cpp_set<qb::mem::Vector>("a", qb::mem::Vector(type));
            auto inner0 = data->__cpp_get<qb::mem::Vector>("a");
            inner0->resize({3});
            inner0->__cpp_set<uint8_t>(0, 10);
            inner0->__cpp_set<uint8_t>(1, 11);
            inner0->__cpp_set<uint8_t>(2, 12);
            
            data->__cpp_set<qb::mem::Vector>("b", qb::mem::Vector(type));
            auto inner1 = data->__cpp_get<qb::mem::Vector>("b");
            inner1->resize({4});
            inner1->__cpp_set<uint8_t>(0, 13);
            inner1->__cpp_set<uint8_t>(1, 14);
            inner1->__cpp_set<uint8_t>(2, 15);
            inner1->__cpp_set<uint8_t>(3, 16);

            qb_assert(data->size() == 2);
            qb_assert(inner0->size() == 3);
            qb_assert(*inner0->__cpp_get<uint8_t>(0) == 10);
            qb_assert(*inner0->__cpp_get<uint8_t>(1) == 11);
            qb_assert(*inner0->__cpp_get<uint8_t>(2) == 12);
            qb_assert(inner1->size() == 4);
            qb_assert(*inner1->__cpp_get<uint8_t>(0) == 13);
            qb_assert(*inner1->__cpp_get<uint8_t>(1) == 14);
            qb_assert(*inner1->__cpp_get<uint8_t>(2) == 15);
            qb_assert(*inner1->__cpp_get<uint8_t>(3) == 16);
        })
    
        qb_test("map{map{u8}}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::MAP, {
                    qb::TypeDef::_add(qb::TypeKind::MAP, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Map>(0);

            data->__cpp_set<qb::mem::Map>("a", qb::mem::Map(type));
            auto inner0 = data->__cpp_get<qb::mem::Map>("a");
            inner0->__cpp_set<uint8_t>("a", 10);
            inner0->__cpp_set<uint8_t>("b", 11);
            inner0->__cpp_set<uint8_t>("c", 12);
            
            data->__cpp_set<qb::mem::Map>("b", qb::mem::Map(type));
            auto inner1 = data->__cpp_get<qb::mem::Map>("b");
            inner1->__cpp_set<uint8_t>("d", 13);
            inner1->__cpp_set<uint8_t>("e", 14);
            inner1->__cpp_set<uint8_t>("f", 15);
            inner1->__cpp_set<uint8_t>("g", 16);

            qb_assert(data->size() == 2);
            qb_assert(inner0->size() == 3);
            qb_assert(*inner0->__cpp_get<uint8_t>("a") == 10);
            qb_assert(*inner0->__cpp_get<uint8_t>("b") == 11);
            qb_assert(*inner0->__cpp_get<uint8_t>("c") == 12);
            qb_assert(inner1->size() == 4);
            qb_assert(*inner1->__cpp_get<uint8_t>("d") == 13);
            qb_assert(*inner1->__cpp_get<uint8_t>("e") == 14);
            qb_assert(*inner1->__cpp_get<uint8_t>("f") == 15);
            qb_assert(*inner1->__cpp_get<uint8_t>("g") == 16);
        })
    
        qb_test("map{struct{bool,u8}}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::MAP, {
                    qb::TypeDef::_add(qb::TypeKind::STRUCT, {
                        qb::TypeDef::_use(B_TYPE_BOOL),
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(0);
            auto data = block.data.__cpp_get<qb::mem::Map>(0);

            data->__cpp_set<qb::mem::Struct>("a", qb::mem::Struct(type));
            auto inner0 = data->__cpp_get<qb::mem::Struct>("a");
            inner0->__cpp_set<bool>(0, false);
            inner0->__cpp_set<uint8_t>(1, 11);
            
            data->__cpp_set<qb::mem::Struct>("b", qb::mem::Struct(type));
            auto inner1 = data->__cpp_get<qb::mem::Struct>("b");
            inner1->__cpp_set<bool>(0, true);
            inner1->__cpp_set<uint8_t>(1, 22);

            qb_assert(data->size() == 2);
            qb_assert(*inner0->__cpp_get<bool>(0) == false);
            qb_assert(*inner0->__cpp_get<uint8_t>(1) == 11);
            qb_assert(*inner1->__cpp_get<bool>(0) == true);
            qb_assert(*inner1->__cpp_get<uint8_t>(1) == 22);
        })
    
        qb_test("map{event{u8}}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::MAP, {
                    qb::TypeDef::_add(qb::TypeKind::EVENT, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Map>(0);

            data->__cpp_set<qb::mem::Event>("a", qb::mem::Event(type));
            auto inner0 = data->__cpp_get<qb::mem::Event>("a");
            inner0->__cpp_set<uint8_t>(qb::EventLevel::INFO, 123, 12);
            
            data->__cpp_set<qb::mem::Event>("b", qb::mem::Event(type));
            auto inner1 = data->__cpp_get<qb::mem::Event>("b");
            inner1->__cpp_set<uint8_t>(qb::EventLevel::WARN, 124, 21);

            qb_assert(data->size() == 2);
            qb_assert(inner0->level == qb::EventLevel::INFO);
            qb_assert(inner0->code == 123);
            qb_assert(*inner0->__cpp_get<uint8_t>() == 12);
            qb_assert(inner1->level == qb::EventLevel::WARN);
            qb_assert(inner1->code == 124);
            qb_assert(*inner1->__cpp_get<uint8_t>() == 21);
        })
    
    })
    
    qb_describe("Block: complex struct", {
    
        qb_test("struct{bool,vec<1>[u8]}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::STRUCT, {
                    qb::TypeDef::_use(B_TYPE_BOOL),
                    qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Struct>(0);
            
            data->__cpp_set<bool>(0, true);
            data->__cpp_set<qb::mem::Vector>(1, qb::mem::Vector(type));
            auto inner = data->__cpp_get<qb::mem::Vector>(1);
            inner->resize({3});
            inner->__cpp_set<uint8_t>(0, 10);
            inner->__cpp_set<uint8_t>(1, 11);
            inner->__cpp_set<uint8_t>(2, 12);
            
            qb_assert(*data->__cpp_get<bool>(0) == true);
            qb_assert(inner->size() == 3);
            qb_assert(*inner->__cpp_get<uint8_t>(0) == 10);
            qb_assert(*inner->__cpp_get<uint8_t>(1) == 11);
            qb_assert(*inner->__cpp_get<uint8_t>(2) == 12);
        })
    
        qb_test("struct{bool,map{u8}}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::STRUCT, {
                    qb::TypeDef::_use(B_TYPE_BOOL),
                    qb::TypeDef::_add(qb::TypeKind::MAP, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Struct>(0);

            data->__cpp_set<bool>(0, true);
            data->__cpp_set<qb::mem::Map>(1, qb::mem::Map(type));
            auto inner = data->__cpp_get<qb::mem::Map>(1);
            inner->__cpp_set<uint8_t>("a", 10);
            inner->__cpp_set<uint8_t>("b", 11);
            inner->__cpp_set<uint8_t>("c", 12);
            
            qb_assert(*data->__cpp_get<bool>(0) == true);
            qb_assert(inner->size() == 3);
            qb_assert(*inner->__cpp_get<uint8_t>("a") == 10);
            qb_assert(*inner->__cpp_get<uint8_t>("b") == 11);
            qb_assert(*inner->__cpp_get<uint8_t>("c") == 12);
        })
    
        qb_test("struct{bool,struct{bool,u8}}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::STRUCT, {
                    qb::TypeDef::_use(B_TYPE_BOOL),
                    qb::TypeDef::_add(qb::TypeKind::STRUCT, {
                        qb::TypeDef::_use(B_TYPE_BOOL),
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(0);
            auto data = block.data.__cpp_get<qb::mem::Struct>(0);

            data->__cpp_set<bool>(0, true);
            data->__cpp_set<qb::mem::Struct>(1, qb::mem::Struct(type));
            auto inner = data->__cpp_get<qb::mem::Struct>(1);
            inner->__cpp_set<bool>(0, false);
            inner->__cpp_set<uint8_t>(1, 11);
            
            qb_assert(*data->__cpp_get<bool>(0) == true);
            qb_assert(*inner->__cpp_get<bool>(0) == false);
            qb_assert(*inner->__cpp_get<uint8_t>(1) == 11);
        })
    
        qb_test("struct{bool,event{u8}}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::STRUCT, {
                    qb::TypeDef::_use(B_TYPE_BOOL),
                    qb::TypeDef::_add(qb::TypeKind::EVENT, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Struct>(0);

            data->__cpp_set<bool>(0, true);
            data->__cpp_set<qb::mem::Event>(1, qb::mem::Event(type));
            auto inner = data->__cpp_get<qb::mem::Event>(1);
            inner->__cpp_set<uint8_t>(qb::EventLevel::INFO, 123, 12);
            
            qb_assert(*data->__cpp_get<bool>(0) == true);
            qb_assert(inner->level == qb::EventLevel::INFO);
            qb_assert(inner->code == 123);
            qb_assert(*inner->__cpp_get<uint8_t>() == 12);
        })
    
    })
    
    qb_describe("Block: complex event", {
    
        qb_test("event{vec<1>[u8]}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::EVENT, {
                    qb::TypeDef::_add(qb::TypeKind::VECTOR, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Event>(0);
            
            data->__cpp_set<qb::mem::Vector>(qb::EventLevel::INFO, 123, qb::mem::Vector(type));
            auto inner = data->__cpp_get<qb::mem::Vector>();
            inner->resize({3});
            inner->__cpp_set<uint8_t>(0, 10);
            inner->__cpp_set<uint8_t>(1, 11);
            inner->__cpp_set<uint8_t>(2, 12);
            
            qb_assert(data->level == qb::EventLevel::INFO);
            qb_assert(data->code == 123);
            qb_assert(inner->size() == 3);
            qb_assert(*inner->__cpp_get<uint8_t>(0) == 10);
            qb_assert(*inner->__cpp_get<uint8_t>(1) == 11);
            qb_assert(*inner->__cpp_get<uint8_t>(2) == 12);
        })
    
        qb_test("event{map{u8}}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::EVENT, {
                    qb::TypeDef::_add(qb::TypeKind::MAP, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Event>(0);

            data->__cpp_set<qb::mem::Map>(qb::EventLevel::INFO, 123, qb::mem::Map(type));
            auto inner = data->__cpp_get<qb::mem::Map>();
            inner->__cpp_set<uint8_t>("a", 10);
            inner->__cpp_set<uint8_t>("b", 11);
            inner->__cpp_set<uint8_t>("c", 12);
            
            qb_assert(data->level == qb::EventLevel::INFO);
            qb_assert(data->code == 123);
            qb_assert(inner->size() == 3);
            qb_assert(*inner->__cpp_get<uint8_t>("a") == 10);
            qb_assert(*inner->__cpp_get<uint8_t>("b") == 11);
            qb_assert(*inner->__cpp_get<uint8_t>("c") == 12);
        })
    
        qb_test("event{struct{bool,u8}}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::EVENT, {
                    qb::TypeDef::_add(qb::TypeKind::STRUCT, {
                        qb::TypeDef::_use(B_TYPE_BOOL),
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(0);
            auto data = block.data.__cpp_get<qb::mem::Event>(0);

            data->__cpp_set<qb::mem::Struct>(qb::EventLevel::INFO, 123, qb::mem::Struct(type));
            auto inner = data->__cpp_get<qb::mem::Struct>();
            inner->__cpp_set<bool>(0, false);
            inner->__cpp_set<uint8_t>(1, 11);
            
            qb_assert(data->level == qb::EventLevel::INFO);
            qb_assert(data->code == 123);
            qb_assert(*inner->__cpp_get<bool>(0) == false);
            qb_assert(*inner->__cpp_get<uint8_t>(1) == 11);
        })
    
        qb_test("event{event{u8}}", {
            auto type_def = qb::TypeDef::block({
                qb::TypeDef::_add(qb::TypeKind::EVENT, {
                    qb::TypeDef::_add(qb::TypeKind::EVENT, {
                        qb::TypeDef::_use(B_TYPE_U8)
                    })
                })
            });
            auto block = qb::mem::Block(type_def);
            auto type = block.types.get(B_TYPE_U8);
            auto data = block.data.__cpp_get<qb::mem::Event>(0);

            data->__cpp_set<qb::mem::Event>(qb::EventLevel::INFO, 123, qb::mem::Event(type));
            auto inner = data->__cpp_get<qb::mem::Event>();
            inner->__cpp_set<uint8_t>(qb::EventLevel::INFO, 123, 12);
            
            qb_assert(data->level == qb::EventLevel::INFO);
            qb_assert(data->code == 123);
            qb_assert(inner->level == qb::EventLevel::INFO);
            qb_assert(inner->code == 123);
            qb_assert(*inner->__cpp_get<uint8_t>() == 12);
        })
    
    })
})