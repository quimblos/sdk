#pragma once
#include "operator.h"

#define DATA_NULL { .type = qb::DataType::_NULL, .value = nullptr, .heap = false }
#define DATA_ERROR { .type = qb::DataType::ERROR, .value = qb::data::error(0, "test"), .heap = false }
#define DATA_BOOL { .type = qb::DataType::BOOL, .value = new bool(true), .heap = false }
#define DATA_UINT8 { .type = qb::DataType::UINT8, .value = new uint8_t(0), .heap = false }
#define DATA_INT8 { .type = qb::DataType::INT8, .value = new int8_t(0), .heap = false }
#define DATA_UINT16 { .type = qb::DataType::UINT16, .value = new uint16_t(0), .heap = false }
#define DATA_INT16 { .type = qb::DataType::INT16, .value = new int16_t(0), .heap = false }
#define DATA_UINT32 { .type = qb::DataType::UINT32, .value = new uint32_t(0), .heap = false }
#define DATA_INT32 { .type = qb::DataType::INT32, .value = new int32_t(0), .heap = false }
#define DATA_FLOAT32 { .type = qb::DataType::FLOAT32, .value = new float(0), .heap = false }
#define DATA_STRING { .type = qb::DataType::STRING, .value = new std::string("test"), .heap = false }
#define DATA_VECTOR { .type = qb::DataType::VECTOR, .value = qb::data::vec(qb::DataType::UINT8, { 5 }), .heap = false }
#define DATA_REF { .type = qb::DataType::REF, .value = qb::data::ref(0, 0, 0b00000010, {{0,1}}), .heap = false }

#define DATA2_NULL { .type = qb::DataType::_NULL, .value = nullptr, .heap = false }
#define DATA2_ERROR { .type = qb::DataType::ERROR, .value = qb::data::error(1, "test2"), .heap = false }
#define DATA2_BOOL { .type = qb::DataType::BOOL, .value = new bool(false), .heap = false }
#define DATA2_UINT8 { .type = qb::DataType::UINT8, .value = new uint8_t(1), .heap = false }
#define DATA2_INT8 { .type = qb::DataType::INT8, .value = new int8_t(1), .heap = false }
#define DATA2_UINT16 { .type = qb::DataType::UINT16, .value = new uint16_t(1), .heap = false }
#define DATA2_INT16 { .type = qb::DataType::INT16, .value = new int16_t(1), .heap = false }
#define DATA2_UINT32 { .type = qb::DataType::UINT32, .value = new uint32_t(1), .heap = false }
#define DATA2_INT32 { .type = qb::DataType::INT32, .value = new int32_t(1), .heap = false }
#define DATA2_FLOAT32 { .type = qb::DataType::FLOAT32, .value = new float(1), .heap = false }
#define DATA2_STRING { .type = qb::DataType::STRING, .value = new std::string("test2"), .heap = false }
#define DATA2_VECTOR { .type = qb::DataType::VECTOR, .value = qb::data::vec(qb::DataType::UINT8, { 5 }), .heap = false }
#define DATA2_REF { .type = qb::DataType::REF, .value = qb::data::ref(0, 0, 0b00000011, {{1,2}}), .heap = false }

#define DATA_VECTOR_BIG { .type = qb::DataType::VECTOR, .value = qb::data::vec(qb::DataType::UINT8, { 30 }), .heap = false }
#define DATA_VECTOR_BIG_U16 { .type = qb::DataType::VECTOR, .value = qb::data::vec(qb::DataType::UINT16, { 30 }), .heap = false }

#define DATA_VECTOR_SLICE(TYPE, CTYPE, SHAPE...) { \
    .type = qb::DataType::VECTOR_SLICE, \
    .value = new qb::data_slice_t({ \
        .type = qb::DataType::TYPE, \
        .value = new CTYPE, \
        .heap = true, \
        .slice = qb::data::Slice::init(SHAPE) \
    }), \
    .heap = false \
}

#define TEST_CAST_OK(TO, FROM) \
    qb_test(#TO " <- " #FROM, { \
        auto target = new qb::data_t(TO); \
        auto source = new qb::data_t(FROM); \
        auto res = qb::_operator::cast(target, source); \
        auto error = res.error; \
        qb::_operator::clean_heap(&res); \
        qb::_operator::clean_data(target); \
        delete target; \
        qb::_operator::clean_data(source); \
        delete source; \
        qb_assert(error == nullptr); \
    })

#define TEST_CAST_FAIL(TO, FROM) \
    qb_test(#TO " <- " #FROM " (should fail)", { \
        auto target = new qb::data_t(TO); \
        auto source = new qb::data_t(FROM); \
        auto res = qb::_operator::cast(target, source); \
        auto error = res.error; \
        qb::_operator::clean_heap(&res); \
        qb::_operator::clean_data(target); \
        delete target; \
        qb::_operator::clean_data(source); \
        delete source; \
        qb_assert(error != nullptr); \
    })

#define TEST_ASSIGN(TYPE, CTYPE) \
    qb_test(#TYPE " = " #TYPE, { \
        auto target = new qb::data_t(DATA_##TYPE); \
        auto source = new qb::data_t(DATA2_##TYPE); \
        qb_assert(*(CTYPE*) target->value != *(CTYPE*) source->value); \
        auto res = qb::_operator::assign(target, source); \
        qb_assert(*(CTYPE*) target->value == *(CTYPE*) source->value); \
        qb::_operator::clean_data(source); \
        delete source; \
        qb::_operator::clean_data(target); \
        delete target; \
    })

#define TEST_COMPARE_EQ(TYPE, CTYPE, V1, V2) \
    qb_test(#TYPE " == " #TYPE, { \
        auto v1 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V1) }); \
        auto v2 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V2) }); \
        auto res_true = qb::_operator::compare(v1, v1); \
        qb_assert(*(int8_t*) res_true.data->value == 0); \
        auto res_false = qb::_operator::compare(v1, v2); \
        qb_assert(*(int8_t*) res_false.data->value != 0); \
        qb::_operator::clean_heap(&res_true); \
        qb::_operator::clean_heap(&res_false); \
        qb::_operator::clean_data(v1); \
        delete v1; \
        qb::_operator::clean_data(v2); \
        delete v2; \
    })

#define TEST_COMPARE_LT(TYPE, CTYPE, V1, V2) \
    qb_test(#TYPE " < " #TYPE, { \
        auto v1 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V1) }); \
        auto v2 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V2) }); \
        auto res = qb::_operator::compare(v1, v2); \
        qb_assert(*(int8_t*) res.data->value > 0); \
        qb::_operator::clean_heap(&res); \
        qb::_operator::clean_data(v1); \
        delete v1; \
        qb::_operator::clean_data(v2); \
        delete v2; \
    })

#define TEST_COMPARE_GT(TYPE, CTYPE, V1, V2) \
    qb_test(#TYPE " > " #TYPE, { \
        auto v1 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V1) }); \
        auto v2 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V2) }); \
        auto res = qb::_operator::compare(v2, v1); \
        qb_assert(*(int8_t*) res.data->value < 0); \
        qb::_operator::clean_heap(&res); \
        qb::_operator::clean_data(v1); \
        delete v1; \
        qb::_operator::clean_data(v2); \
        delete v2; \
    })

qb_suite(test_operator, "operator", {

    qb_describe("cast", {
        TEST_CAST_OK(DATA_NULL, DATA2_NULL)
        TEST_CAST_FAIL(DATA_NULL, DATA2_ERROR)
        TEST_CAST_FAIL(DATA_NULL, DATA2_BOOL)
        TEST_CAST_FAIL(DATA_NULL, DATA2_UINT8)
        TEST_CAST_FAIL(DATA_NULL, DATA2_INT8)
        TEST_CAST_FAIL(DATA_NULL, DATA2_UINT16)
        TEST_CAST_FAIL(DATA_NULL, DATA2_INT16)
        TEST_CAST_FAIL(DATA_NULL, DATA2_UINT32)
        TEST_CAST_FAIL(DATA_NULL, DATA2_INT32)
        TEST_CAST_FAIL(DATA_NULL, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_NULL, DATA2_STRING)
        TEST_CAST_FAIL(DATA_NULL, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_NULL, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_ERROR, DATA2_NULL)
        TEST_CAST_OK(DATA_ERROR, DATA2_ERROR)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_BOOL)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_UINT8)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_INT8)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_UINT16)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_INT16)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_UINT32)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_INT32)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_FLOAT32)
        TEST_CAST_OK(DATA_ERROR, DATA2_STRING)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_ERROR, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_BOOL, DATA2_NULL)
        TEST_CAST_FAIL(DATA_BOOL, DATA2_ERROR)
        TEST_CAST_OK(DATA_BOOL, DATA2_BOOL)
        TEST_CAST_OK(DATA_BOOL, DATA2_UINT8)
        TEST_CAST_OK(DATA_BOOL, DATA2_INT8)
        TEST_CAST_OK(DATA_BOOL, DATA2_UINT16)
        TEST_CAST_OK(DATA_BOOL, DATA2_INT16)
        TEST_CAST_OK(DATA_BOOL, DATA2_UINT32)
        TEST_CAST_OK(DATA_BOOL, DATA2_INT32)
        TEST_CAST_OK(DATA_BOOL, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_BOOL, DATA2_STRING)
        TEST_CAST_FAIL(DATA_BOOL, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_BOOL, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_UINT8, DATA2_NULL)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_ERROR)
        TEST_CAST_OK(DATA_UINT8, DATA2_BOOL)
        TEST_CAST_OK(DATA_UINT8, DATA2_UINT8)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_INT8)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_UINT16)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_INT16)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_UINT32)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_INT32)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_STRING)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_UINT8, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_INT8, DATA2_NULL)
        TEST_CAST_FAIL(DATA_INT8, DATA2_ERROR)
        TEST_CAST_OK(DATA_INT8, DATA2_BOOL)
        TEST_CAST_OK(DATA_INT8, DATA2_UINT8)
        TEST_CAST_OK(DATA_INT8, DATA2_INT8)
        TEST_CAST_FAIL(DATA_INT8, DATA2_UINT16)
        TEST_CAST_FAIL(DATA_INT8, DATA2_INT16)
        TEST_CAST_FAIL(DATA_INT8, DATA2_UINT32)
        TEST_CAST_FAIL(DATA_INT8, DATA2_INT32)
        TEST_CAST_FAIL(DATA_INT8, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_INT8, DATA2_STRING)
        TEST_CAST_FAIL(DATA_INT8, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_INT8, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_UINT16, DATA2_NULL)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_ERROR)
        TEST_CAST_OK(DATA_UINT16, DATA2_BOOL)
        TEST_CAST_OK(DATA_UINT16, DATA2_UINT8)
        TEST_CAST_OK(DATA_UINT16, DATA2_INT8)
        TEST_CAST_OK(DATA_UINT16, DATA2_UINT16)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_INT16)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_UINT32)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_INT32)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_STRING)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_INT16, DATA2_NULL)
        TEST_CAST_FAIL(DATA_INT16, DATA2_ERROR)
        TEST_CAST_OK(DATA_INT16, DATA2_BOOL)
        TEST_CAST_OK(DATA_INT16, DATA2_UINT8)
        TEST_CAST_OK(DATA_INT16, DATA2_INT8)
        TEST_CAST_OK(DATA_INT16, DATA2_UINT16)
        TEST_CAST_OK(DATA_INT16, DATA2_INT16)
        TEST_CAST_FAIL(DATA_INT16, DATA2_UINT32)
        TEST_CAST_FAIL(DATA_INT16, DATA2_INT32)
        TEST_CAST_FAIL(DATA_INT16, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_INT16, DATA2_STRING)
        TEST_CAST_FAIL(DATA_INT16, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_INT16, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_UINT32, DATA2_NULL)
        TEST_CAST_FAIL(DATA_UINT32, DATA2_ERROR)
        TEST_CAST_OK(DATA_UINT32, DATA2_BOOL)
        TEST_CAST_OK(DATA_UINT32, DATA2_UINT8)
        TEST_CAST_OK(DATA_UINT32, DATA2_INT8)
        TEST_CAST_OK(DATA_UINT32, DATA2_UINT16)
        TEST_CAST_OK(DATA_UINT32, DATA2_INT16)
        TEST_CAST_OK(DATA_UINT32, DATA2_UINT32)
        TEST_CAST_FAIL(DATA_UINT32, DATA2_INT32)
        TEST_CAST_FAIL(DATA_UINT32, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_UINT32, DATA2_STRING)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_UINT16, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_INT32, DATA2_NULL)
        TEST_CAST_FAIL(DATA_INT32, DATA2_ERROR)
        TEST_CAST_OK(DATA_INT32, DATA2_BOOL)
        TEST_CAST_OK(DATA_INT32, DATA2_UINT8)
        TEST_CAST_OK(DATA_INT32, DATA2_INT8)
        TEST_CAST_OK(DATA_INT32, DATA2_UINT16)
        TEST_CAST_OK(DATA_INT32, DATA2_INT16)
        TEST_CAST_OK(DATA_INT32, DATA2_UINT32)
        TEST_CAST_OK(DATA_INT32, DATA2_INT32)
        TEST_CAST_FAIL(DATA_INT32, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_INT32, DATA2_STRING)
        TEST_CAST_FAIL(DATA_INT32, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_INT32, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_FLOAT32, DATA2_NULL)
        TEST_CAST_FAIL(DATA_FLOAT32, DATA2_ERROR)
        TEST_CAST_OK(DATA_FLOAT32, DATA2_BOOL)
        TEST_CAST_OK(DATA_FLOAT32, DATA2_UINT8)
        TEST_CAST_OK(DATA_FLOAT32, DATA2_INT8)
        TEST_CAST_OK(DATA_FLOAT32, DATA2_UINT16)
        TEST_CAST_OK(DATA_FLOAT32, DATA2_INT16)
        TEST_CAST_OK(DATA_FLOAT32, DATA2_UINT32)
        TEST_CAST_OK(DATA_FLOAT32, DATA2_INT32)
        TEST_CAST_OK(DATA_FLOAT32, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_FLOAT32, DATA2_STRING)
        TEST_CAST_FAIL(DATA_FLOAT32, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_FLOAT32, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_STRING, DATA2_NULL)
        TEST_CAST_OK(DATA_STRING, DATA2_ERROR)
        TEST_CAST_OK(DATA_STRING, DATA2_BOOL)
        TEST_CAST_OK(DATA_STRING, DATA2_UINT8)
        TEST_CAST_OK(DATA_STRING, DATA2_INT8)
        TEST_CAST_OK(DATA_STRING, DATA2_UINT16)
        TEST_CAST_OK(DATA_STRING, DATA2_INT16)
        TEST_CAST_OK(DATA_STRING, DATA2_UINT32)
        TEST_CAST_OK(DATA_STRING, DATA2_INT32)
        TEST_CAST_OK(DATA_STRING, DATA2_FLOAT32)
        TEST_CAST_OK(DATA_STRING, DATA2_STRING)
        TEST_CAST_FAIL(DATA_STRING, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_STRING, DATA2_REF)
        
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_NULL)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_ERROR)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_BOOL)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_UINT8)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_INT8)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_UINT16)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_INT16)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_UINT32)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_INT32)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_STRING)
        TEST_CAST_OK(DATA_VECTOR, DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_VECTOR, DATA2_REF)
        TEST_CAST_FAIL(DATA_VECTOR, DATA_VECTOR_BIG)
        TEST_CAST_FAIL(DATA_VECTOR_BIG, DATA_VECTOR_BIG_U16)
        
        TEST_CAST_FAIL(DATA_REF, DATA2_NULL)
        TEST_CAST_FAIL(DATA_REF, DATA2_ERROR)
        TEST_CAST_FAIL(DATA_REF, DATA2_BOOL)
        TEST_CAST_FAIL(DATA_REF, DATA2_UINT8)
        TEST_CAST_FAIL(DATA_REF, DATA2_INT8)
        TEST_CAST_FAIL(DATA_REF, DATA2_UINT16)
        TEST_CAST_FAIL(DATA_REF, DATA2_INT16)
        TEST_CAST_FAIL(DATA_REF, DATA2_UINT32)
        TEST_CAST_FAIL(DATA_REF, DATA2_INT32)
        TEST_CAST_FAIL(DATA_REF, DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_REF, DATA2_STRING)
        TEST_CAST_FAIL(DATA_REF, DATA2_VECTOR)
        TEST_CAST_OK(DATA_REF, DATA2_REF)

        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_NULL)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_ERROR)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_BOOL)
        TEST_CAST_OK(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_UINT8)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_INT8)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_UINT16)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_INT16)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_UINT32)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_INT32)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_FLOAT32)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_STRING)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_VECTOR)
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA2_REF)

        TEST_CAST_OK(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}))
        TEST_CAST_FAIL(DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,1}}), DATA_VECTOR_SLICE(UINT8, uint8_t, {{0,2}}))
    })

    qb_describe("assign", {
        // TEST_ASSIGN(_NULL)
        TEST_ASSIGN(BOOL, bool)
        TEST_ASSIGN(UINT8, uint8_t)
        TEST_ASSIGN(INT8, int8_t)
        TEST_ASSIGN(UINT16, uint16_t)
        TEST_ASSIGN(INT16, int16_t)
        TEST_ASSIGN(UINT32, uint32_t)
        TEST_ASSIGN(INT32, int32_t)
        TEST_ASSIGN(FLOAT32, float)
        TEST_ASSIGN(STRING, std::string)
        // TEST_ASSIGN(VECTOR, ...) TODO
        // TEST_ASSIGN(REF, ...) TODO
    })

    qb_describe("compare", {
        // TEST_COMPARE_EQ(_NULL)
        TEST_COMPARE_EQ(BOOL, bool, true, false)
        TEST_COMPARE_EQ(UINT8, uint8_t, 3, 7)
        TEST_COMPARE_LT(UINT8, uint8_t, 3, 7)
        TEST_COMPARE_GT(UINT8, uint8_t, 3, 7)
        TEST_COMPARE_EQ(INT8, int8_t, -7, 3)
        TEST_COMPARE_LT(INT8, int8_t, -7, 3)
        TEST_COMPARE_GT(INT8, int8_t, -7, 3)
        TEST_COMPARE_EQ(UINT16, uint16_t, 333, 777)
        TEST_COMPARE_LT(UINT16, uint16_t, 333, 777)
        TEST_COMPARE_GT(UINT16, uint16_t, 333, 777)
        TEST_COMPARE_EQ(INT16, int16_t, -777, 333)
        TEST_COMPARE_LT(INT16, int16_t, -777, 333)
        TEST_COMPARE_GT(INT16, int16_t, -777, 333)
        TEST_COMPARE_EQ(UINT32, uint32_t, 333333, 777777)
        TEST_COMPARE_LT(UINT32, uint32_t, 333333, 777777)
        TEST_COMPARE_GT(UINT32, uint32_t, 333333, 777777)
        TEST_COMPARE_EQ(INT32, int32_t, -777777, 333333)
        TEST_COMPARE_LT(INT32, int32_t, -777777, 333333)
        TEST_COMPARE_GT(INT32, int32_t, -777777, 333333)
        TEST_COMPARE_EQ(FLOAT32, float, -12.34, 56.78)
        TEST_COMPARE_LT(FLOAT32, float, -12.34, 56.78)
        TEST_COMPARE_GT(FLOAT32, float, -12.34, 56.78)
        TEST_COMPARE_EQ(STRING, std::string, "test", "test2")
        // TEST_COMPARE_EQ(VECTOR, ...) TODO
        // TEST_COMPARE_EQ(REF, ...) TODO
    })
    
})