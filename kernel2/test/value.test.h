#pragma once
#include "values.h"

qb_suite(test_value, "value", {

    auto checker = qb::TypeChecker();

    qb_describe("built-in", {
    
        qb_test("void", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_VOID
            });

            auto data = block.get<qb::void_t>(0);
            // qb_assert(sizeof(data) == 1);
            qb_assert(*data == 0);
        })
    
        qb_test("null", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_NULL
            });
            
            auto data = block.get<qb::void_t>(0);
            // qb_assert(sizeof(data) == 1);
            qb_assert(*data == 0);
        })
    
        qb_test("bool", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_BOOL
            });
            block.set(0, true);

            auto data = block.get<bool>(0);
            qb_assert(*data == true);
        })
    
        qb_test("u8", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_U8
            });
            block.set(0, (uint8_t) 123);

            auto data = block.get<uint8_t>(0);
            qb_assert(*data == 123);
        })
    
        qb_test("i8", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_I8
            });
            block.set(0, (int8_t) -123);

            auto data = block.get<int8_t>(0);
            qb_assert(*data == -123);
        })
    
        qb_test("u16", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_U16
            });
            block.set(0, (uint16_t) 1234);

            auto data = block.get<uint16_t>(0);
            qb_assert(*data == 1234);
        })
    
        qb_test("i16", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_I16
            });
            block.set(0, (int16_t) -1234);

            auto data = block.get<int16_t>(0);
            qb_assert(*data == -1234);
        })
    
        qb_test("u32", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_U32
            });
            block.set(0, (uint32_t) 123456);

            auto data = block.get<uint32_t>(0);
            qb_assert(*data == 123456);
        })
    
        qb_test("i32", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_I32
            });
            block.set(0, (int32_t) -123456);

            auto data = block.get<int32_t>(0);
            qb_assert(*data == -123456);
        })
    
        qb_test("f32", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_F32
            });
            block.set(0, (float) -12.34);

            auto data = block.get<float>(0);
            qb_assert(abs(*data - -12.34) < 0.000000000000000001);
        })
    
        qb_test("str", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_STR
            });
            block.set(0, std::string("test"));

            auto data = block.get<std::string>(0);
            qb_assert(*data == "test");
        })
    
        qb_test("ref", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_REF
            });
            block.set(0, qb::data::Reference(0xFF, 0x01, false, false));

            auto data = block.get<qb::data::Reference>(0);
            qb_assert(data->to_str() == "@255.1");
        })

        qb_test("multiple", {
            auto block = qb::DataBlock(checker, {
                B_TYPE_VOID, // 0
                B_TYPE_NULL, // 1
                B_TYPE_BOOL, // 2
                B_TYPE_U8,   // 3
                B_TYPE_F32,  // 4
                B_TYPE_STR,  // 5
                B_TYPE_REF,  // 6
            });

            block.set(2, true);
            block.set(3, (uint8_t) 123);
            block.set(4, (float) 123.456);
            block.set(5, std::string("test"));
            block.set(6, qb::data::Reference(0xFF,0x01,false,false));

            qb_assert(*block.get<qb::void_t>(0) == 0);
            qb_assert(*block.get<qb::void_t>(1) == 0);
            qb_assert(*block.get<bool>(2) == true);
            qb_assert(*block.get<uint8_t>(3) == 123);
            qb_assert(abs(*block.get<float>(4) - 123.456) < 0.000000000000000001);
            qb_assert(*block.get<std::string>(5) == "test");
            qb_assert(block.get<qb::data::Reference>(6)->to_str() == "@255.1");
        })

    })

    qb_describe("map", {
    
        qb_test("map{bool}", {
            auto block = qb::DataBlock(checker, {
                checker.add_map(B_TYPE_BOOL)
            });
            block.set_map(0, "a", true);
            block.set_map(0, "b", false);

            auto data = block.get_map<bool>(0);
            qb_assert(data->size() == 2);
            qb_assert(data->at("a") == true);
            qb_assert(data->at("b") == false);
        })
    
        qb_test("map{u8}", {
            auto block = qb::DataBlock(checker, {
                checker.add_map(B_TYPE_U8)
            });
            block.set_map(0, "c", (uint8_t) 123);
            block.set_map(0, "d", (uint8_t) 234);

            auto data = block.get_map<uint8_t>(0);
            qb_assert(data->size() == 2);
            qb_assert(data->at("c") == 123);
            qb_assert(data->at("d") == 234);
        })
    
        qb_test("map{f32}", {
            auto block = qb::DataBlock(checker, {
                checker.add_map(B_TYPE_F32)
            });
            block.set_map(0, "e", (float) 12.34);
            block.set_map(0, "f", (float) 56.78);

            auto data = block.get_map<float>(0);
            qb_assert(data->size() == 2);
            qb_assert(abs(data->at("e") - 12.34) < 0.000000000000000001);
            qb_assert(abs(data->at("f") - 56.78) < 0.000000000000000001);
        })
    
        qb_test("map{str}", {
            auto block = qb::DataBlock(checker, {
                checker.add_map(B_TYPE_STR)
            });
            block.set_map(0, "g", std::string("gaga"));
            block.set_map(0, "h", std::string("haha"));

            auto data = block.get_map<std::string>(0);
            qb_assert(data->size() == 2);
            qb_assert(data->at("g") == "gaga");
            qb_assert(data->at("h") == "haha");
        })
    
        qb_test("map{ref}", {
            auto block = qb::DataBlock(checker, {
                checker.add_map(B_TYPE_REF)
            });
            block.set_map(0, "i", qb::data::Reference(0xFF, 0x01, false, false));
            block.set_map(0, "j", qb::data::Reference(0xFF, 0x02, false, false));

            auto data = block.get_map<qb::data::Reference>(0);
            qb_assert(data->size() == 2);
            qb_assert(data->at("i").to_str() == "@255.1");
            qb_assert(data->at("j").to_str() == "@255.2");
        })
   
    })

    qb_describe("vec", {
    
        qb_test("vec{bool}", {
            auto block = qb::DataBlock(checker, {
                checker.add_vec(B_TYPE_BOOL)
            });
            block.resize_vec(0, 2);
            block.set_vec(0, 0, true);
            block.set_vec(0, 1, false);

            auto data = block.get_vec<bool>(0);
            qb_assert(data->size() == 2);
            qb_assert(data->at(0) == true);
            qb_assert(data->at(1) == false);
        })
    
        // qb_test("map{u8}", {
        //     auto block = qb::DataBlock(checker, {
        //         checker.add_map(B_TYPE_U8)
        //     });
        //     block.set_map(0, "c", (uint8_t) 123);
        //     block.set_map(0, "d", (uint8_t) 234);

        //     auto data = block.get_map<uint8_t>(0);
        //     qb_assert(data->size() == 2);
        //     qb_assert(data->at("c") == 123);
        //     qb_assert(data->at("d") == 234);
        // })
    
        // qb_test("map{f32}", {
        //     auto block = qb::DataBlock(checker, {
        //         checker.add_map(B_TYPE_F32)
        //     });
        //     block.set_map(0, "e", (float) 12.34);
        //     block.set_map(0, "f", (float) 56.78);

        //     auto data = block.get_map<float>(0);
        //     qb_assert(data->size() == 2);
        //     qb_assert(abs(data->at("e") - 12.34) < 0.000000000000000001);
        //     qb_assert(abs(data->at("f") - 56.78) < 0.000000000000000001);
        // })
    
        // qb_test("map{str}", {
        //     auto block = qb::DataBlock(checker, {
        //         checker.add_map(B_TYPE_STR)
        //     });
        //     block.set_map(0, "g", std::string("gaga"));
        //     block.set_map(0, "h", std::string("haha"));

        //     auto data = block.get_map<std::string>(0);
        //     qb_assert(data->size() == 2);
        //     qb_assert(data->at("g") == "gaga");
        //     qb_assert(data->at("h") == "haha");
        // })
    
        // qb_test("map{ref}", {
        //     auto block = qb::DataBlock(checker, {
        //         checker.add_map(B_TYPE_REF)
        //     });
        //     block.set_map(0, "i", qb::data::Reference(0xFF, 0x01, false, false));
        //     block.set_map(0, "j", qb::data::Reference(0xFF, 0x02, false, false));

        //     auto data = block.get_map<qb::data::Reference>(0);
        //     qb_assert(data->size() == 2);
        //     qb_assert(data->at("i").to_str() == "@255.1");
        //     qb_assert(data->at("j").to_str() == "@255.2");
        // })
   
    })
})