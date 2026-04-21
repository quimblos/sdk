#pragma once
#include "data.h"

qb_suite(test_data, "data", {

    qb_describe("constructor", {
    
        qb_test("Null", {
            auto data = qb::data::Null();
        })

        qb_test("Error, C string", {
            auto data = qb::data::Error(99, 19, "Some error message.");
            qb_assert(data.code == 99);
            qb_assert(data.message == "Some error message.");
        })

        qb_test("Error, C++ string", {
            auto data = qb::data::Error(99, "Some error message.");
            qb_assert(data.code == 99);
            qb_assert(data.message == "Some error message.");
        })
        
        qb_test("Numeric<bool>", {
            auto data = qb::data::Numeric<bool>(qb::DataType::BOOL, false);
            qb_assert(data.value == false);
            data = qb::data::Numeric<bool>(qb::DataType::BOOL, true);
            qb_assert(data.value == true);
        })
        
        qb_test("Numeric<uint8_t>", {
            auto data = qb::data::Numeric<uint8_t>(qb::DataType::UINT8, 123);
            qb_assert(data.value == 123);
        })
        
        qb_test("Numeric<int8_t>", {
            auto data = qb::data::Numeric<int8_t>(qb::DataType::UINT8, -123);
            qb_assert(data.value == -123);
        })

        qb_test("Numeric<uint16_t>", {
            auto data = qb::data::Numeric<uint16_t>(qb::DataType::UINT16, 12345);
            qb_assert(data.value == 12345);
        })
        
        qb_test("Numeric<int16_t>", {
            auto data = qb::data::Numeric<int16_t>(qb::DataType::UINT16, -12345);
            qb_assert(data.value == -12345);
        })

        qb_test("Numeric<uint32_t>", {
            auto data = qb::data::Numeric<uint32_t>(qb::DataType::UINT32, 1234567);
            qb_assert(data.value == 1234567);
        })
        
        qb_test("Numeric<int32_t>", {
            auto data = qb::data::Numeric<int32_t>(qb::DataType::UINT32, -1234567);
            qb_assert(data.value == -1234567);
        })

        qb_test("Numeric<float>", {
            auto data = qb::data::Numeric<float>(qb::DataType::FLOAT32, 123.456);
            qb_assert(data.value == (float) 123.456);
        })

        qb_test("String, C string", {
            auto data = qb::data::String(19, "Some error message.");
            qb_assert(data.value == "Some error message.");
        })

        qb_test("String, C++ string", {
            auto data = qb::data::String("Some error message.");
            qb_assert(data.value == "Some error message.");
        })

    })

    qb_describe("factory", {
        qb_test("null", {
            auto data = qb::data::_null();
            delete data;
        })
        qb_test("error", {
            auto data = qb::data::error(99, "Some error message.");
            qb_assert(data->code == 99);
            qb_assert(data->message == "Some error message.");
            delete data;
        })
        qb_test("bool", {
            auto data = qb::data::_bool(true);
            qb_assert(data->value == true);
            delete data;
        })
        qb_test("u8", {
            auto data = qb::data::u8(123);
            qb_assert(data->value == 123);
            delete data;
        })
        qb_test("i8", {
            auto data = qb::data::i8(-123);
            qb_assert(data->value == -123);
            delete data;
        })
        qb_test("u16", {
            auto data = qb::data::u16(12345);
            qb_assert(data->value == 12345);
            delete data;
        })
        qb_test("i16", {
            auto data = qb::data::i16(-12345);
            qb_assert(data->value == -12345);
            delete data;
        })
        qb_test("u32", {
            auto data = qb::data::u32(1234567);
            qb_assert(data->value == 1234567);
            delete data;
        })
        qb_test("i32", {
            auto data = qb::data::i32(-1234567);
            qb_assert(data->value == -1234567);
            delete data;
        })
        qb_test("f32", {
            auto data = qb::data::f32(123.456);
            qb_assert(data->value == (float)123.456);
            delete data;
        })
        qb_test("str", {
            auto data = qb::data::str("Some string.");
            qb_assert(data->value == "Some string.");
            delete data;
        })
    })

    qb_describe("parse", {
        qb_test("NULL", {
            qb::code_t code[] = { qb::DataType::_NULL };
            auto out = qb::Data::parse(code, 1, 0);
            qb_assert(out.code == 0);
            delete out.data;
        })
        qb_test("ERROR", {
            qb::code_t code[] = { qb::DataType::ERROR, 99, 10, 's','o','m','e',' ','e','r','r','o','r' };
            auto out = qb::Data::parse(code, 13, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::Error*) out.data;
            qb_assert(data->code == 99);
            qb_assert(data->message == "some error");
            delete data;
        })
        qb_test("BOOL", {
            qb::code_t code[] = { qb::DataType::BOOL, 1 };
            auto out = qb::Data::parse(code, 2, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::Numeric<bool>*) out.data;
            qb_assert(data->value == true);
            delete data;
        })
        qb_test("UINT8", {
            qb::code_t code[] = { qb::DataType::UINT8, 0x7B };
            auto out = qb::Data::parse(code, 2, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::Numeric<uint8_t>*) out.data;
            qb_assert(data->value == 123);
            delete data;
        })
        qb_test("INT8", {
            qb::code_t code[] = { qb::DataType::INT8, 0x85 };
            auto out = qb::Data::parse(code, 2, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::Numeric<int8_t>*) out.data;
            qb_assert(data->value == -123);
            delete data;
        })
        qb_test("UINT16", {
            qb::code_t code[] = { qb::DataType::UINT16, 0x30, 0x39 };
            auto out = qb::Data::parse(code, 3, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::Numeric<uint16_t>*) out.data;
            qb_assert(data->value == 12345);
            delete data;
        })
        qb_test("INT16", {
            qb::code_t code[] = { qb::DataType::INT16, 0xCF, 0xC7 };
            auto out = qb::Data::parse(code, 3, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::Numeric<int16_t>*) out.data;
            qb_assert(data->value == -12345);
            delete data;
        })
        qb_test("UINT32", {
            qb::code_t code[] = { qb::DataType::UINT32, 0x00, 0x12, 0xd6, 0x87 };
            auto out = qb::Data::parse(code, 5, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::Numeric<uint32_t>*) out.data;
            qb_assert(data->value == 1234567);
            delete data;
        })
        qb_test("INT32", {
            qb::code_t code[] = { qb::DataType::INT32, 0xff, 0xed, 0x29, 0x79 };
            auto out = qb::Data::parse(code, 5, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::Numeric<int32_t>*) out.data;
            qb_assert(data->value == -1234567);
            delete data;
        })
        qb_test("FLOAT32", {
            qb::code_t code[] = { qb::DataType::FLOAT32, 0x42, 0xf6, 0xe9, 0x79 };
            auto out = qb::Data::parse(code, 5, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::Numeric<float>*) out.data;
            qb_assert(data->value == (float) 123.456);
            delete data;
        })
        qb_test("STRING", {
            qb::code_t code[] = { qb::DataType::STRING, 0x00, 0x09, 's','o','m','e',' ','t','e','x','t' };
            auto out = qb::Data::parse(code, 12, 0);
            qb_assert(out.code == 0);
            
            auto data = (qb::data::String*) out.data;
            qb_assert(data->value == "some text");
            delete data;
        })
    })
})