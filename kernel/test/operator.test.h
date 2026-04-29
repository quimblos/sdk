#pragma once
#include "operator.h"

#define DATA__NULL { .type = qb::DataType::_NULL, .value = nullptr, .heap = false }
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
#define DATA_VECTOR { .type = qb::DataType::VECTOR, .value = qb::data::vec<uint8_t>(qb::DataType::UINT8, { 5 }), .heap = false }
#define DATA_REF { .type = qb::DataType::REF, .value = qb::data::ref(false, 0, 0, 0), .heap = false }

#define DATA2__NULL { .type = qb::DataType::_NULL, .value = nullptr, .heap = false }
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
#define DATA2_VECTOR { .type = qb::DataType::VECTOR, .value = qb::data::vec<uint8_t>(qb::DataType::UINT8, { 6 }), .heap = false }
#define DATA2_REF { .type = qb::DataType::REF, .value = qb::data::ref(true, 1, 1, 1), .heap = false }

#define TEST_CAST_OK(TO, FROM) \
    qb_test(#TO " <- " #FROM, { \
        auto target = new qb::data_t(DATA_##TO); \
        auto source = new qb::data_t(DATA_##FROM); \
        auto res = qb::_operator::cast(target, source); \
        auto error = res.error; \
        qb::_operator::clean_heap(&res); \
        qb::_operator::delete_data(target); \
        qb::_operator::delete_data(source); \
        qb_assert(error == nullptr); \
    })

#define TEST_CAST_FAIL(TO, FROM) \
    qb_test(#TO " <- " #FROM " (should fail)", { \
        auto target = new qb::data_t(DATA_##TO); \
        auto source = new qb::data_t(DATA_##FROM); \
        auto res = qb::_operator::cast(target, source); \
        auto error = res.error; \
        qb::_operator::clean_heap(&res); \
        qb::_operator::delete_data(target); \
        qb::_operator::delete_data(source); \
        qb_assert(error != nullptr); \
    })

#define TEST_ASSIGN(TYPE, CTYPE) \
    qb_test(#TYPE " = " #TYPE, { \
        auto source = new qb::data_t(DATA2_##TYPE); \
        auto target = new qb::data_t(DATA_##TYPE); \
        qb_assert(*(CTYPE*) target->value != *(CTYPE*) source->value); \
        auto res = qb::_operator::assign(target, source); \
        qb_assert(*(CTYPE*) target->value == *(CTYPE*) source->value); \
        qb::_operator::delete_data(source); \
        qb::_operator::delete_data(target); \
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
        qb::_operator::delete_data(v1); \
        qb::_operator::delete_data(v2); \
    })

#define TEST_COMPARE_LT(TYPE, CTYPE, V1, V2) \
    qb_test(#TYPE " < " #TYPE, { \
        auto v1 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V1) }); \
        auto v2 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V2) }); \
        auto res = qb::_operator::compare(v1, v2); \
        qb_assert(*(int8_t*) res.data->value > 0); \
        qb::_operator::clean_heap(&res); \
        qb::_operator::delete_data(v1); \
        qb::_operator::delete_data(v2); \
    })

#define TEST_COMPARE_GT(TYPE, CTYPE, V1, V2) \
    qb_test(#TYPE " > " #TYPE, { \
        auto v1 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V1) }); \
        auto v2 = new qb::data_t({ .type = qb::DataType::TYPE, .value = new CTYPE(V2) }); \
        auto res = qb::_operator::compare(v2, v1); \
        qb_assert(*(int8_t*) res.data->value < 0); \
        qb::_operator::clean_heap(&res); \
        qb::_operator::delete_data(v1); \
        qb::_operator::delete_data(v2); \
    })

qb_suite(test_operator, "operator", {

    qb_describe("cast", {
        TEST_CAST_OK(_NULL, _NULL)
        TEST_CAST_FAIL(_NULL, ERROR)
        TEST_CAST_FAIL(_NULL, BOOL)
        TEST_CAST_FAIL(_NULL, UINT8)
        TEST_CAST_FAIL(_NULL, INT8)
        TEST_CAST_FAIL(_NULL, UINT16)
        TEST_CAST_FAIL(_NULL, INT16)
        TEST_CAST_FAIL(_NULL, UINT32)
        TEST_CAST_FAIL(_NULL, INT32)
        TEST_CAST_FAIL(_NULL, FLOAT32)
        TEST_CAST_FAIL(_NULL, STRING)
        TEST_CAST_FAIL(_NULL, VECTOR)
        TEST_CAST_FAIL(_NULL, REF)
        
        TEST_CAST_FAIL(ERROR, _NULL)
        TEST_CAST_OK(ERROR, ERROR)
        TEST_CAST_FAIL(ERROR, BOOL)
        TEST_CAST_FAIL(ERROR, UINT8)
        TEST_CAST_FAIL(ERROR, INT8)
        TEST_CAST_FAIL(ERROR, UINT16)
        TEST_CAST_FAIL(ERROR, INT16)
        TEST_CAST_FAIL(ERROR, UINT32)
        TEST_CAST_FAIL(ERROR, INT32)
        TEST_CAST_FAIL(ERROR, FLOAT32)
        TEST_CAST_OK(ERROR, STRING)
        TEST_CAST_FAIL(ERROR, VECTOR)
        TEST_CAST_FAIL(ERROR, REF)
        
        TEST_CAST_FAIL(BOOL, _NULL)
        TEST_CAST_FAIL(BOOL, ERROR)
        TEST_CAST_OK(BOOL, BOOL)
        TEST_CAST_OK(BOOL, UINT8)
        TEST_CAST_OK(BOOL, INT8)
        TEST_CAST_OK(BOOL, UINT16)
        TEST_CAST_OK(BOOL, INT16)
        TEST_CAST_OK(BOOL, UINT32)
        TEST_CAST_OK(BOOL, INT32)
        TEST_CAST_OK(BOOL, FLOAT32)
        TEST_CAST_FAIL(BOOL, STRING)
        TEST_CAST_FAIL(BOOL, VECTOR)
        TEST_CAST_FAIL(BOOL, REF)
        
        TEST_CAST_FAIL(UINT8, _NULL)
        TEST_CAST_FAIL(UINT8, ERROR)
        TEST_CAST_OK(UINT8, BOOL)
        TEST_CAST_OK(UINT8, UINT8)
        TEST_CAST_FAIL(UINT8, INT8)
        TEST_CAST_FAIL(UINT8, UINT16)
        TEST_CAST_FAIL(UINT8, INT16)
        TEST_CAST_FAIL(UINT8, UINT32)
        TEST_CAST_FAIL(UINT8, INT32)
        TEST_CAST_FAIL(UINT8, FLOAT32)
        TEST_CAST_FAIL(UINT8, STRING)
        TEST_CAST_FAIL(UINT8, VECTOR)
        TEST_CAST_FAIL(UINT8, REF)
        
        TEST_CAST_FAIL(INT8, _NULL)
        TEST_CAST_FAIL(INT8, ERROR)
        TEST_CAST_OK(INT8, BOOL)
        TEST_CAST_OK(INT8, UINT8)
        TEST_CAST_OK(INT8, INT8)
        TEST_CAST_FAIL(INT8, UINT16)
        TEST_CAST_FAIL(INT8, INT16)
        TEST_CAST_FAIL(INT8, UINT32)
        TEST_CAST_FAIL(INT8, INT32)
        TEST_CAST_FAIL(INT8, FLOAT32)
        TEST_CAST_FAIL(INT8, STRING)
        TEST_CAST_FAIL(INT8, VECTOR)
        TEST_CAST_FAIL(INT8, REF)
        
        TEST_CAST_FAIL(UINT16, _NULL)
        TEST_CAST_FAIL(UINT16, ERROR)
        TEST_CAST_OK(UINT16, BOOL)
        TEST_CAST_OK(UINT16, UINT8)
        TEST_CAST_OK(UINT16, INT8)
        TEST_CAST_OK(UINT16, UINT16)
        TEST_CAST_FAIL(UINT16, INT16)
        TEST_CAST_FAIL(UINT16, UINT32)
        TEST_CAST_FAIL(UINT16, INT32)
        TEST_CAST_FAIL(UINT16, FLOAT32)
        TEST_CAST_FAIL(UINT16, STRING)
        TEST_CAST_FAIL(UINT16, VECTOR)
        TEST_CAST_FAIL(UINT16, REF)
        
        TEST_CAST_FAIL(INT16, _NULL)
        TEST_CAST_FAIL(INT16, ERROR)
        TEST_CAST_OK(INT16, BOOL)
        TEST_CAST_OK(INT16, UINT8)
        TEST_CAST_OK(INT16, INT8)
        TEST_CAST_OK(INT16, UINT16)
        TEST_CAST_OK(INT16, INT16)
        TEST_CAST_FAIL(INT16, UINT32)
        TEST_CAST_FAIL(INT16, INT32)
        TEST_CAST_FAIL(INT16, FLOAT32)
        TEST_CAST_FAIL(INT16, STRING)
        TEST_CAST_FAIL(INT16, VECTOR)
        TEST_CAST_FAIL(INT16, REF)
        
        TEST_CAST_FAIL(UINT32, _NULL)
        TEST_CAST_FAIL(UINT32, ERROR)
        TEST_CAST_OK(UINT32, BOOL)
        TEST_CAST_OK(UINT32, UINT8)
        TEST_CAST_OK(UINT32, INT8)
        TEST_CAST_OK(UINT32, UINT16)
        TEST_CAST_OK(UINT32, INT16)
        TEST_CAST_OK(UINT32, UINT32)
        TEST_CAST_FAIL(UINT32, INT32)
        TEST_CAST_FAIL(UINT32, FLOAT32)
        TEST_CAST_FAIL(UINT32, STRING)
        TEST_CAST_FAIL(UINT16, VECTOR)
        TEST_CAST_FAIL(UINT16, REF)
        
        TEST_CAST_FAIL(INT32, _NULL)
        TEST_CAST_FAIL(INT32, ERROR)
        TEST_CAST_OK(INT32, BOOL)
        TEST_CAST_OK(INT32, UINT8)
        TEST_CAST_OK(INT32, INT8)
        TEST_CAST_OK(INT32, UINT16)
        TEST_CAST_OK(INT32, INT16)
        TEST_CAST_OK(INT32, UINT32)
        TEST_CAST_OK(INT32, INT32)
        TEST_CAST_FAIL(INT32, FLOAT32)
        TEST_CAST_FAIL(INT32, STRING)
        TEST_CAST_FAIL(INT32, VECTOR)
        TEST_CAST_FAIL(INT32, REF)
        
        TEST_CAST_FAIL(FLOAT32, _NULL)
        TEST_CAST_FAIL(FLOAT32, ERROR)
        TEST_CAST_OK(FLOAT32, BOOL)
        TEST_CAST_OK(FLOAT32, UINT8)
        TEST_CAST_OK(FLOAT32, INT8)
        TEST_CAST_OK(FLOAT32, UINT16)
        TEST_CAST_OK(FLOAT32, INT16)
        TEST_CAST_OK(FLOAT32, UINT32)
        TEST_CAST_OK(FLOAT32, INT32)
        TEST_CAST_OK(FLOAT32, FLOAT32)
        TEST_CAST_FAIL(FLOAT32, STRING)
        TEST_CAST_FAIL(FLOAT32, VECTOR)
        TEST_CAST_FAIL(FLOAT32, REF)
        
        TEST_CAST_FAIL(STRING, _NULL)
        TEST_CAST_OK(STRING, ERROR)
        TEST_CAST_OK(STRING, BOOL)
        TEST_CAST_OK(STRING, UINT8)
        TEST_CAST_OK(STRING, INT8)
        TEST_CAST_OK(STRING, UINT16)
        TEST_CAST_OK(STRING, INT16)
        TEST_CAST_OK(STRING, UINT32)
        TEST_CAST_OK(STRING, INT32)
        TEST_CAST_OK(STRING, FLOAT32)
        TEST_CAST_OK(STRING, STRING)
        TEST_CAST_FAIL(STRING, VECTOR)
        TEST_CAST_FAIL(STRING, REF)
        
        TEST_CAST_FAIL(VECTOR, _NULL)
        TEST_CAST_FAIL(VECTOR, ERROR)
        TEST_CAST_FAIL(VECTOR, BOOL)
        TEST_CAST_FAIL(VECTOR, UINT8)
        TEST_CAST_FAIL(VECTOR, INT8)
        TEST_CAST_FAIL(VECTOR, UINT16)
        TEST_CAST_FAIL(VECTOR, INT16)
        TEST_CAST_FAIL(VECTOR, UINT32)
        TEST_CAST_FAIL(VECTOR, INT32)
        TEST_CAST_FAIL(VECTOR, FLOAT32)
        TEST_CAST_FAIL(VECTOR, STRING)
        TEST_CAST_OK(VECTOR, VECTOR)
        TEST_CAST_FAIL(VECTOR, REF)
        
        TEST_CAST_FAIL(REF, _NULL)
        TEST_CAST_FAIL(REF, ERROR)
        TEST_CAST_FAIL(REF, BOOL)
        TEST_CAST_FAIL(REF, UINT8)
        TEST_CAST_FAIL(REF, INT8)
        TEST_CAST_FAIL(REF, UINT16)
        TEST_CAST_FAIL(REF, INT16)
        TEST_CAST_FAIL(REF, UINT32)
        TEST_CAST_FAIL(REF, INT32)
        TEST_CAST_FAIL(REF, FLOAT32)
        TEST_CAST_FAIL(REF, STRING)
        TEST_CAST_FAIL(REF, VECTOR)
        TEST_CAST_OK(REF, REF)
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