#pragma once
#include "float.h"

qb_suite(test_float, "float", {

    qb_describe("float_to_u32", {

        qb_test("1", {
            uint32_t v = float_to_u32(1);
            qb_assert(v == 0x3f800000);
        })

        qb_test("0.1", {
            uint32_t v = float_to_u32(0.1);
            qb_assert(v == 0x3dcccccd);
        })

        qb_test("0.000000000000000000000000000000000000000000001", {
            uint32_t v = float_to_u32(0.000000000000000000000000000000000000000000001);
            qb_assert(v == 0x00000001);
        })

        qb_test("123.456", {
            uint32_t v = float_to_u32(123.456);
            qb_assert(v == 0x42f6e979);
        })

        qb_test("-123.456", {
            uint32_t v = float_to_u32(-123.456);
            qb_assert(v == 0xc2f6e979);
        })

    })

    qb_describe("u32_to_float", {

        qb_test("1", {
            float v = u32_to_float(0x3f800000);
            qb_assert(v == (float) 1);
        })

        qb_test("0.1", {
            float v = u32_to_float(0x3dcccccd);
            qb_assert(v == (float) 0.1);
        })

        qb_test("0.000000000000000000000000000000000000000000001", {
            float v = u32_to_float(0x00000001);
            qb_assert(v == (float) 0.000000000000000000000000000000000000000000001);
        })

        qb_test("123.456", {
            float v = u32_to_float(0x42f6e979);
            qb_assert(v == (float) 123.456);
        })

        qb_test("-123.456", {
            float v = u32_to_float(0xc2f6e979);
            qb_assert(v == (float) -123.456);
        })

    })
})