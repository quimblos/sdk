#pragma once
#include "memory.h"

namespace qb {

    namespace _operator {

        struct res_t {
            uint8_t code;
            void* value;
        };

        struct cast_res_t {
            uint8_t code;
            type_t type = Type::VOID;
            void* value = nullptr;
            void* target = nullptr;
        };

        // Casting

        res_t as_pointer(Node* source);
        res_t as_numeric(Node* source);
        res_t as_string(Node* source);
        res_t as_array(Node* source);

        cast_res_t cast_to(Node* target, Node* source, index_t index = 0);

        // Copy
        
        res_t copy_to(Node* target, Node* source, index_t index = 0);

        // Numerical

        res_t add_to(Node* target, Node* source, index_t index = 0);
        res_t sub_to(Node* target, Node* source, index_t index = 0);
        res_t mult_to(Node* target, Node* source, index_t index = 0);
        res_t div_to(Node* target, Node* source, index_t index = 0);
        res_t mod_to(Node* target, Node* source, index_t index = 0);
        res_t pow_to(Node* target, Node* source, index_t index = 0);
        
        // Comparison

        res_t is_equal_to(Node* target, Node* source, index_t index = 0);
        res_t is_less_than(Node* target, Node* source, index_t index = 0);
        res_t is_greater_than(Node* target, Node* source, index_t index = 0);

        // Dereference pointer

        res_t dereference(Node* source);
    }
}


/* Response Codes*/

#define QB_OPERATOR_R_OK 00

#define QB_OPERATOR_R_UNKNOWN_TYPE 01

#define QB_OPERATOR_R_INVALID_CAST_VOID_TO_PTR 10
#define QB_OPERATOR_R_INVALID_CAST_ERROR_TO_PTR 11
#define QB_OPERATOR_R_INVALID_CAST_FLOAT_TO_PTR 12
#define QB_OPERATOR_R_INVALID_CAST_STRING_TO_PTR 13
#define QB_OPERATOR_R_INVALID_CAST_ARRAY_TO_PTR 14

#define QB_OPERATOR_R_INVALID_CAST_VOID_TO_NUMERIC 20
#define QB_OPERATOR_R_INVALID_CAST_ERROR_TO_NUMERIC 21
#define QB_OPERATOR_R_INVALID_CAST_PTR_TO_NUMERIC 22
#define QB_OPERATOR_R_INVALID_CAST_STRING_TO_NUMERIC 23
#define QB_OPERATOR_R_INVALID_CAST_ARRAY_TO_NUMERIC 24

#define QB_OPERATOR_R_INVALID_CAST_VOID_TO_STRING 30
#define QB_OPERATOR_R_INVALID_CAST_PTR_TO_STRING 31
#define QB_OPERATOR_R_INVALID_CAST_NUMERIC_TO_STRING 32
#define QB_OPERATOR_R_INVALID_CAST_ARRAY_TO_STRING 33

#define QB_OPERATOR_R_INVALID_CAST_TO_ARRAY 40

#define QB_OPERATOR_R_COPY_FAILED_TARGET_NOT_POINTER 50
#define QB_OPERATOR_R_COPY_FAILED_TARGET_NOT_RESOLVED 51
#define QB_OPERATOR_R_COPY_FAILED_TARGET_NOT_INDEXABLE 52
#define QB_OPERATOR_R_COPY_FAILED_TARGET_OUT_OF_BOUNDS 53

#define QB_OPERATOR_R_COPY_FAILED_TARGET_VALUE_VOID 60
#define QB_OPERATOR_R_COPY_FAILED_TARGET_VALUE_ARRAY 61
#define QB_OPERATOR_R_COPY_FAILED_TARGET_UNKNOWN_ARRAY 62

#define QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID 70
#define QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR 71
#define QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR 72
#define QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_PTR 73
#define QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_BOOL 74
#define QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_STRING 75
#define QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY 76