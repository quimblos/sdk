#pragma once
#include "operator.h"
#include "i18n.h"

#define _TEST_CAST_OK(EXPLICIT, TO_TYPE, FROM_TYPE, TO_C_TYPE, FROM_C_TYPE, INIT...) \
    auto to_type = solver.get(TO_TYPE); \
    auto from_type = solver.get(FROM_TYPE); \
    auto value = new FROM_C_TYPE(INIT); \
    auto res = qb::op::cast(to_type, from_type, (qb::data_t) value, EXPLICIT); \
    if (res.code != 0) { \
        std::cout << qb::i18n::op.at(res.code) << std::endl; \
        qb_fail() \
    } \
    TO_C_TYPE out = res.out == nullptr ? 0 : *(TO_C_TYPE*) res.out; \
    LOG(out) \
    delete value; \
    if (res.temp) delete res.out;

#define TEST_CAST_OK(TO_TYPE, FROM_TYPE, TO_C_TYPE, FROM_C_TYPE, INIT...) _TEST_CAST_OK(false, TO_TYPE, FROM_TYPE, TO_C_TYPE, FROM_C_TYPE, INIT)
#define TEST_EXPLICIT_CAST_OK(TO_TYPE, FROM_TYPE, TO_C_TYPE, FROM_C_TYPE, INIT...) _TEST_CAST_OK(true, TO_TYPE, FROM_TYPE, TO_C_TYPE, FROM_C_TYPE, INIT)

#define _TEST_CAST_FAIL(EXPLICIT, EXPECTED_ERROR, TO_TYPE, FROM_TYPE, FROM_C_TYPE, INIT...) \
    auto to_type = solver.get(TO_TYPE); \
    auto from_type = solver.get(FROM_TYPE); \
    auto value = new FROM_C_TYPE(INIT); \
    auto res = qb::op::cast(to_type, from_type, (qb::data_t) value, EXPLICIT); \
    delete value; \
    if (res.temp) delete res.out; \
    qb_assert(res.code == qb::op::res_t::Code::EXPECTED_ERROR)

#define TEST_CAST_FAIL(EXPECTED_ERROR, TO_TYPE, FROM_TYPE, FROM_C_TYPE, INIT...) _TEST_CAST_FAIL(false, EXPECTED_ERROR, TO_TYPE, FROM_TYPE, FROM_C_TYPE, INIT) 
#define TEST_CAST_EXPLICIT_FAIL(EXPECTED_ERROR, TO_TYPE, FROM_TYPE, FROM_C_TYPE, INIT...) _TEST_CAST_FAIL(true, EXPECTED_ERROR, TO_TYPE, FROM_TYPE, FROM_C_TYPE, INIT) 


qb_suite(test_operator_cast, "operator: cast", {
    
    auto solver = qb::TypeBlock();

    qb_describe("built-in: void", {
    
        qb_test("VOID <- VOID", {
            TEST_CAST_OK(B_TYPE_VOID, B_TYPE_VOID, qb::void_t, qb::void_t, 0)
            qb_assert(out == 0)
        })
        qb_test("VOID <- BOOL", {
            TEST_CAST_OK(B_TYPE_VOID, B_TYPE_BOOL, qb::void_t, bool, true)
            qb_assert(out == 0)
        })
        qb_test("VOID <- I16", {
            TEST_CAST_OK(B_TYPE_VOID, B_TYPE_I16, qb::void_t, int16_t, 123)
            qb_assert(out == 0)
        })
        qb_test("VOID <- F32", {
            TEST_CAST_OK(B_TYPE_VOID, B_TYPE_F32, qb::void_t, float, 12.34)
            qb_assert(out == 0)
        })
        qb_test("VOID <- STR", {
            TEST_CAST_OK(B_TYPE_VOID, B_TYPE_STR, qb::void_t, std::string, "test")
            qb_assert(out == 0)
        })
        qb_test("VOID <- REF", {
            TEST_CAST_OK(B_TYPE_VOID, B_TYPE_REF, qb::void_t, qb::mem::Reference, {})
            qb_assert(out == 0)
        })

    })

    qb_describe("built-in: bool", {
    
        qb_test("BOOL <- VOID", {
            TEST_CAST_OK(B_TYPE_BOOL, B_TYPE_VOID, bool, qb::void_t, 0)
            qb_assert(out == false)
        })
        qb_test("BOOL <- BOOL", {
            TEST_CAST_OK(B_TYPE_BOOL, B_TYPE_BOOL, bool, bool, true)
            qb_assert(out == true)
        })
        qb_test("BOOL <- I16", {
            TEST_CAST_OK(B_TYPE_BOOL, B_TYPE_I16, bool, int16_t, 123)
            qb_assert(out == true)
        })
        qb_test("BOOL <- F32", {
            TEST_CAST_OK(B_TYPE_BOOL, B_TYPE_F32, bool, float, 12.34)
            qb_assert(out == true)
        })
        qb_test("BOOL <- STR", {
            TEST_CAST_OK(B_TYPE_BOOL, B_TYPE_STR, bool, std::string, "test")
            qb_assert(out == true)
        })
        qb_test("BOOL <- REF", {
            TEST_CAST_OK(B_TYPE_BOOL, B_TYPE_REF, bool, qb::mem::Reference, {})
            qb_assert(out == true)
        })

    })

    qb_describe("built-in: int", {
    
        qb_test("I16 <- VOID (should fail)", {
            TEST_CAST_FAIL(CAST_VOID_TO_INT, B_TYPE_I16, B_TYPE_VOID, qb::void_t, 0)
        })

        // int <- int
        {
            qb_test("U8 <- BOOL", {
                TEST_CAST_OK(B_TYPE_U8, B_TYPE_BOOL, uint8_t, bool, true)
                qb_assert(out == 1)
            })
            qb_test("U16 <- BOOL", {
                TEST_CAST_OK(B_TYPE_U16, B_TYPE_BOOL, uint16_t, bool, true)
                qb_assert(out == 1)
            })
            qb_test("U32 <- BOOL", {
                TEST_CAST_OK(B_TYPE_U32, B_TYPE_BOOL, uint32_t, bool, true)
                qb_assert(out == 1)
            })
            qb_test("I8 <- BOOL", {
                TEST_CAST_OK(B_TYPE_I8, B_TYPE_BOOL, int8_t, bool, true)
                qb_assert(out == 1)
            })
            qb_test("I16 <- BOOL", {
                TEST_CAST_OK(B_TYPE_I16, B_TYPE_BOOL, int16_t, bool, true)
                qb_assert(out == 1)
            })
            qb_test("I32 <- BOOL", {
                TEST_CAST_OK(B_TYPE_I32, B_TYPE_BOOL, int32_t, bool, true)
                qb_assert(out == 1)
            })
    
            qb_test("U8 <- U8", {
                TEST_CAST_OK(B_TYPE_U8, B_TYPE_U8, uint8_t, uint8_t, 123)
                qb_assert(out == 123)
            })
            qb_test("U8 <- U16 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_SIZE, B_TYPE_U8, B_TYPE_U16, uint16_t, 12345)
            })
            qb_test("U8 <- U32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_SIZE, B_TYPE_U8, B_TYPE_U32, uint32_t, 12345678)
            })
            qb_test("U8 <- I8 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_TO_UINT, B_TYPE_U8, B_TYPE_I8, int8_t, -123)
            })
            qb_test("U8 <- I16 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_TO_UINT, B_TYPE_U8, B_TYPE_I16, int16_t, -12345)
            })
            qb_test("U8 <- I32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_TO_UINT, B_TYPE_U8, B_TYPE_I32, int32_t, -12345678)
            })
    
            qb_test("U16 <- U8", {
                TEST_CAST_OK(B_TYPE_U16, B_TYPE_U8, uint16_t, uint8_t, 123)
                qb_assert(out == 123)
            })
            qb_test("U16 <- U16", {
                TEST_CAST_OK(B_TYPE_U16, B_TYPE_U16, uint16_t, uint16_t, 12345)
                qb_assert(out == 12345)
            })
            qb_test("U16 <- U32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_SIZE, B_TYPE_U16, B_TYPE_U32, uint32_t, 12345678)
            })
            qb_test("U16 <- I8 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_TO_UINT, B_TYPE_U16, B_TYPE_I8, int8_t, -123)
            })
            qb_test("U16 <- I16 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_TO_UINT, B_TYPE_U16, B_TYPE_I16, int16_t, -12345)
            })
            qb_test("U16 <- I32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_TO_UINT, B_TYPE_U16, B_TYPE_I32, int32_t, -12345678)
            })
    
            qb_test("U32 <- U8", {
                TEST_CAST_OK(B_TYPE_U32, B_TYPE_U8, uint32_t, uint8_t, 123)
                qb_assert(out == 123)
            })
            qb_test("U32 <- U16", {
                TEST_CAST_OK(B_TYPE_U32, B_TYPE_U16, uint32_t, uint16_t, 12345)
                qb_assert(out == 12345)
            })
            qb_test("U32 <- U32", {
                TEST_CAST_OK(B_TYPE_U32, B_TYPE_U32, uint32_t, uint32_t, 12345678)
                qb_assert(out == 12345678)
            })
            qb_test("U32 <- I8 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_TO_UINT, B_TYPE_U32, B_TYPE_I8, int8_t, -123)
            })
            qb_test("U32 <- I16 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_TO_UINT, B_TYPE_U32, B_TYPE_I16, int16_t, -12345)
            })
            qb_test("U32 <- I32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_TO_UINT, B_TYPE_U32, B_TYPE_I32, int32_t, -12345678)
            })
    
            qb_test("I8 <- U8 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_UINT_SIZE, B_TYPE_I8, B_TYPE_U8, uint8_t, 123)
            })
            qb_test("I8 <- U16 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_SIZE, B_TYPE_I8, B_TYPE_U16, uint16_t, 12345)
            })
            qb_test("I8 <- U32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_SIZE, B_TYPE_I8, B_TYPE_U32, uint32_t, 12345678)
            })
            qb_test("I8 <- I8", {
                TEST_CAST_OK(B_TYPE_I8, B_TYPE_I8, int8_t, int8_t, -123)
                qb_assert(out == -123)
            })
            qb_test("I8 <- I16 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_SIZE, B_TYPE_I8, B_TYPE_I16, int16_t, -12345)
            })
            qb_test("I8 <- I32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_SIZE, B_TYPE_I8, B_TYPE_I32, int32_t, -12345678)
            })
    
            qb_test("I16 <- U8", {
                TEST_CAST_OK(B_TYPE_I16, B_TYPE_U8, int16_t, uint8_t, 123)
                qb_assert(out == 123)
            })
            qb_test("I16 <- U16 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_UINT_SIZE, B_TYPE_I16, B_TYPE_U16, uint16_t, 12345)
            })
            qb_test("I16 <- U32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_SIZE, B_TYPE_I16, B_TYPE_U32, uint32_t, 12345678)
            })
            qb_test("I16 <- I8", {
                TEST_CAST_OK(B_TYPE_I16, B_TYPE_I8, int16_t, int8_t, -123)
                qb_assert(out == -123)
            })
            qb_test("I16 <- I16", {
                TEST_CAST_OK(B_TYPE_I16, B_TYPE_I16, int16_t, int16_t, -12345)
                qb_assert(out == -12345)
            })
            qb_test("I16 <- I32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_SIZE, B_TYPE_I16, B_TYPE_I32, int32_t, -12345678)
            })
    
            qb_test("I32 <- U8", {
                TEST_CAST_OK(B_TYPE_I32, B_TYPE_U8, int16_t, uint8_t, 123)
                qb_assert(out == 123)
            })
            qb_test("I32 <- U16", {
                TEST_CAST_OK(B_TYPE_I32, B_TYPE_U16, int16_t, uint16_t, 12345)
                qb_assert(out == 12345)
            })
            qb_test("I32 <- U32 (should fail)", {
                TEST_CAST_FAIL(CAST_INT_UINT_SIZE, B_TYPE_I32, B_TYPE_U32, uint32_t, 12345678)
            })
            qb_test("I32 <- I8", {
                TEST_CAST_OK(B_TYPE_I32, B_TYPE_I8, int32_t, int8_t, -123)
                qb_assert(out == -123)
            })
            qb_test("I32 <- I16", {
                TEST_CAST_OK(B_TYPE_I32, B_TYPE_I16, int32_t, int16_t, -12345)
                qb_assert(out == -12345)
            })
            qb_test("I32 <- I32", {
                TEST_CAST_OK(B_TYPE_I32, B_TYPE_I32, int32_t, int32_t, -12345678)
                qb_assert(out == -12345678)
            })
        }

        // int <- float
        {
            qb_test("U8 <- F32 (should fail)", {
                TEST_CAST_FAIL(CAST_IMPLICIT_FLOAT_TO_INT, B_TYPE_U8, B_TYPE_F32, float, 12.34)
            })
            qb_test("U8 <- F32: explicit", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_U8, B_TYPE_F32, uint8_t, float, 12.34)
                qb_assert(out == 12)
            })
            qb_test("U16 <- F32 (should fail)", {
                TEST_CAST_FAIL(CAST_IMPLICIT_FLOAT_TO_INT, B_TYPE_U16, B_TYPE_F32, float, 12.34)
            })
            qb_test("U16 <- F32: explicit", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_U16, B_TYPE_F32, uint16_t, float, 12345.67)
                qb_assert(out == 12345)
            })
            qb_test("U32 <- F32 (should fail)", {
                TEST_CAST_FAIL(CAST_IMPLICIT_FLOAT_TO_INT, B_TYPE_U32, B_TYPE_F32, float, 12.34)
            })
            qb_test("U32 <- F32: explicit", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_U32, B_TYPE_F32, uint32_t, float, 12345678.9)
                qb_assert(out == 12345679)
            })
            qb_test("I8 <- F32 (should fail)", {
                TEST_CAST_FAIL(CAST_IMPLICIT_FLOAT_TO_INT, B_TYPE_I8, B_TYPE_F32, float, 12.34)
            })
            qb_test("I8 <- F32: explicit", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_I8, B_TYPE_F32, int8_t, float, -12.34)
                qb_assert(out == -12)
            })
            qb_test("I16 <- F32 (should fail)", {
                TEST_CAST_FAIL(CAST_IMPLICIT_FLOAT_TO_INT, B_TYPE_I16, B_TYPE_F32, float, 12.34)
            })
            qb_test("I16 <- F32: explicit", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_I16, B_TYPE_F32, int16_t, float, -12345.67)
                qb_assert(out == -12345)
            })
            qb_test("I32 <- F32 (should fail)", {
                TEST_CAST_FAIL(CAST_IMPLICIT_FLOAT_TO_INT, B_TYPE_I32, B_TYPE_F32, float, 12.34)
            })
            qb_test("I32 <- F32: explicit", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_I32, B_TYPE_F32, int32_t, float, -12345678.9)
                qb_assert(out == -12345679)
            })
        }

        // int <- string
        {
            qb_test("U8 <- STR (should fail)", {
                TEST_CAST_FAIL(CAST_IMPLICIT_STRING_TO_INT, B_TYPE_U8, B_TYPE_STR, std::string, "123")
            })
            qb_test("U8 <- STR: explicit", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_U8, B_TYPE_STR, uint8_t, std::string, "123")
                qb_assert(out == 123)
            })
            qb_test("U8 <- STR: explicit, nan", {
                TEST_CAST_EXPLICIT_FAIL(CAST_NAN_STRING_TO_INT, B_TYPE_U8, B_TYPE_STR, std::string, "abc")
            })
            qb_test("U8 <- STR: explicit, float", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_U8, B_TYPE_STR, uint8_t, std::string, "12.34")
                qb_assert(out == 12)
            })
            qb_test("U8 <- STR: explicit, overflow", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_U8, B_TYPE_STR, uint8_t, std::string, "258")
                qb_assert(out == 2)
            })
            qb_test("U8 <- STR: explicit, signed overflow", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_U8, B_TYPE_STR, uint8_t, std::string, "-4")
                qb_assert(out == 252)
            })
        }

        // int <- ref
        {
            qb_test("U8 <- REF (should fail)", {
                TEST_CAST_FAIL(CAST_IMPLICIT_REF_TO_INT, B_TYPE_U8, B_TYPE_REF, qb::mem::Reference, 0, 123)
            })
            qb_test("U8 <- REF: explicit", {
                TEST_EXPLICIT_CAST_OK(B_TYPE_U8, B_TYPE_REF, uint8_t, qb::mem::Reference, 0, 123)
                qb_assert(out == 123)
            })
        }

    })
})