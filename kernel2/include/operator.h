#pragma once
#include <charconv>
#include "code.h"

namespace qb {

    namespace op {

        struct res_t {
            enum Code {
                OK = 0x00,
                UNKNOWN_TARGET_TYPE,
                UNKNOWN_SOURCE_TYPE,
                NOT_IMPLEMENTED,
                CAST_VOID_TO_INT,
                CAST_INT_SIZE,
                CAST_INT_TO_UINT,
                CAST_INT_UINT_SIZE,
                CAST_IMPLICIT_FLOAT_TO_INT,
                CAST_IMPLICIT_STRING_TO_INT,
                CAST_NAN_STRING_TO_INT,
                CAST_IMPLICIT_REF_TO_INT,
                CAST_IMPLICIT_VEC_TO_INT,
                CAST_IMPLICIT_MAP_TO_INT,
                CAST_STRUCT_TO_INT,
                CAST_IMPLICIT_EVENT_TO_INT,
                CAST_VOID_TO_FLOAT,
                CAST_STRING_TO_FLOAT,
                CAST_REF_TO_FLOAT,
                CAST_VECTOR_TO_FLOAT,
                CAST_MAP_TO_FLOAT,
                CAST_STRUCT_TO_FLOAT,
                CAST_EVENT_TO_FLOAT,
                CAST_VOID_TO_STRING,
                CAST_BOOL_TO_STRING,
                CAST_INT_TO_STRING,
                CAST_FLOAT,
                CAST_REF_TO_STRING,
                CAST_VECTOR_TO_STRING,
                CAST_MAP_TO_STRING,
                CAST_STRUCT_TO_STRING,
                CAST_EVENT_TO_STRING,
                CAST_VOID_TO_REF,
                CAST_BOOL_TO_REF,
                CAST_INT_TO_REF,
                CAST_FLOAT_TO_REF,
                CAST_STRING_TO_REF,
                CAST_VECTOR_TO_REF,
                CAST_MAP_TO_REF,
                CAST_STRUCT_TO_REF,
                CAST_EVENT_TO_REF,
                MATH_BOOL_TARGET,
                MATH_DEC_TARGET
            } code : 7;
            bool temp : 1;
            data_t out;
        };

        // cast

        res_t cast_to_bool(const Type* from_type, data_t value);
        res_t cast_to_int(const Type* to_type, const Type* from_type, data_t value, bool is_explicit = false);
        res_t cast_to_float(const Type* from_type, data_t value);
        res_t cast_to_string(const Type* from_type, data_t value);
        res_t cast_to_ref(const Type* from_type, data_t value);
        res_t cast(const Type* to_type, const Type* from_type, data_t value, bool is_explicit = false);

        // assign

        res_t assign(mem::Block& t_block, port_t t_port, mem::Block& s_block, port_t s_port, bool explicit_cast = false);
        res_t math(qb::instruction::Math::Flags::Op op, mem::Block& t_block, port_t t_port, mem::Block& s_block, port_t s_port);

    }

}