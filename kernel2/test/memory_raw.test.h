#pragma once
#include "memory.h"

#define TEST_SET_RAW(TDX, TYPE, CODE...) \
    auto type_def = qb::TypeDef::block({ \
        qb::TypeDef::_use(TDX) \
    }); \
    auto block = qb::mem::Block(type_def); \
    qb::byte_t data[] = CODE; \
    block.data.set_raw(0, data, sizeof(data)/sizeof(qb::byte_t)); \
    auto value = block.data.__cpp_get<TYPE>(0);

qb_suite(test_memory_raw, "memory_raw", {

    qb_describe("Primitives", {
    
        qb_test("void", {
            TEST_SET_RAW(B_TYPE_VOID, qb::void_t, {})
            qb_assert(*value == 0);
        })
    
        qb_test("null", {
            TEST_SET_RAW(B_TYPE_NULL, qb::void_t, {})
            qb_assert(*value == 0);
        })
    
        qb_test("bool = false", {
            TEST_SET_RAW(B_TYPE_BOOL, bool, { false })
            qb_assert(*value == false);
        })
    
        qb_test("bool = true", {
            TEST_SET_RAW(B_TYPE_BOOL, bool, { true })
            qb_assert(*value == true);
        })
    
        qb_test("u8 = 0", {
            TEST_SET_RAW(B_TYPE_U8, uint8_t, { 0 })
            qb_assert(*value == 0);
        })

        qb_test("u8 = 123", {
            TEST_SET_RAW(B_TYPE_U8, uint8_t, { 123 })
            qb_assert(*value == 123);
        })

        qb_test("i8 = -123", {
            TEST_SET_RAW(B_TYPE_I8, int8_t, { (uint8_t) -123 })
            qb_assert(*value == -123);
        })
    
        qb_test("i8 = 123", {
            TEST_SET_RAW(B_TYPE_I8, int8_t, { 123 })
            qb_assert(*value == 123);
        })

        qb_test("u16 = 0", {
            TEST_SET_RAW(B_TYPE_U16, uint16_t, { 0 })
            qb_assert(*value == 0);
        })

        qb_test("u16 = 0x1234 = 4660", {
            TEST_SET_RAW(B_TYPE_U16, uint16_t, { 0x12, 0x34 })
            qb_assert(*value == 0x1234);
            qb_assert(*value == 4660);
        })

        qb_test("i16 = 0xEDCC = -4660", {
            TEST_SET_RAW(B_TYPE_I16, int16_t, { 0xED, 0xCC })
            qb_assert(*value == (int16_t) 0xEDCC);
            qb_assert(*value == -4660);
        })

        qb_test("i16 = 0x1234", {
            TEST_SET_RAW(B_TYPE_I16, int16_t, { 0x12, 0x34 })
            qb_assert(*value == 0x1234);
            qb_assert(*value == 4660);
        })

        qb_test("u32 = 0", {
            TEST_SET_RAW(B_TYPE_U32, uint32_t, { 0 })
            qb_assert(*value == 0);
        })

        qb_test("u32 = 0x12345678 = 305419896", {
            TEST_SET_RAW(B_TYPE_U32, uint32_t, { 0x12, 0x34,0x56, 0x78 })
            qb_assert(*value == 0x12345678);
            qb_assert(*value == 305419896);
        })

        qb_test("i32 = 0xEDCBA988 = -305419896", {
            TEST_SET_RAW(B_TYPE_I32, int32_t, { 0xED, 0xCB, 0xA9, 0x88 })
            qb_assert(*value == (int32_t) 0xEDCBA988);
            qb_assert(*value == -305419896);
        })

        qb_test("i32 = 0x12345678", {
            TEST_SET_RAW(B_TYPE_I32, int32_t, { 0x12, 0x34, 0x56, 0x78 })
            qb_assert(*value == 0x12345678);
            qb_assert(*value == 305419896);
        })

        qb_test("f32 = 0.0", {
            TEST_SET_RAW(B_TYPE_F32, float, { 0x00, 0x00, 0x00, 0x00 })
            qb_assert(*value == (float) 0x00000000);
            qb_assert(*value == 0.0);
        })

        qb_test("f32 = 12.34", {
            TEST_SET_RAW(B_TYPE_F32, float, { 0xA4, 0x70, 0x45, 0x41 })
            qb_assert(abs(*value - (float) 0xA4704541) < EPSILON);
            qb_assert(abs(*value - 12.34) < EPSILON);
        })

        qb_test("str = 'test'", {
            TEST_SET_RAW(B_TYPE_STR, std::string, { 0x00, 0x04, 't', 'e', 's', 't' })
            qb_assert(*value == "test");
        })
    
    })
})