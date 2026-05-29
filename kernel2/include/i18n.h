#pragma once
#include "parser.h"
#include "operator.h"
#include "node.h"
#include "engine.h"

namespace qb {

    namespace i18n {

        const std::map<uint8_t, std::string> parser = {
            { qb::parser::res_t::Code::OK,                      "[parser] OK" },
            { qb::parser::res_t::Code::UNEXPECTED_EOF,          "[parser] Unexpected EOF" },
            { qb::parser::res_t::Code::CUSTOM_PRIMITIVE_TYPE,   "[parser] Creating custom primitive types is not allowed" },
            { qb::parser::res_t::Code::UNKNOWN_TYPE_KIND,       "[parser] Unknown type kind" },
            { qb::parser::res_t::Code::CONST_ASSIGNMENT,        "[parser] Assignment to constant block is not allowed" },
            { qb::parser::res_t::Code::ENTITY_KIND,             "[parser] Invalid kind of entity" },
            { qb::parser::res_t::Code::COMPARE_OP,              "[parser] Invalid comparison operation" },
            { qb::parser::res_t::Code::MATH_OP,                 "[parser] Invalid arithmetic operation" },
            { qb::parser::res_t::Code::UNKNOWN_OP_CODE,         "[parser] Invalid op code" },
            { qb::parser::res_t::Code::INVALID_HEADER,          "[parser] Invalid header" },
        };

        const std::map<uint8_t, std::string> op = {
            { qb::op::res_t::Code::OK,                          "[op] OK" },
            { qb::op::res_t::Code::UNKNOWN_TARGET_TYPE,         "[op] UNKNOWN_TARGET_TYPE" },
            { qb::op::res_t::Code::UNKNOWN_SOURCE_TYPE,         "[op] UNKNOWN_SOURCE_TYPE" },
            { qb::op::res_t::Code::NOT_IMPLEMENTED,             "[op] NOT_IMPLEMENTED" },
            { qb::op::res_t::Code::CAST_VOID_TO_INT,            "[op] CAST_VOID_TO_INT" },
            { qb::op::res_t::Code::CAST_INT_SIZE,               "[op] CAST_INT_SIZE" },
            { qb::op::res_t::Code::CAST_INT_TO_UINT,            "[op] CAST_INT_TO_UINT" },
            { qb::op::res_t::Code::CAST_INT_UINT_SIZE,          "[op] CAST_INT_UINT_SIZE" },
            { qb::op::res_t::Code::CAST_IMPLICIT_FLOAT_TO_INT,  "[op] CAST_IMPLICIT_FLOAT_TO_INT" },
            { qb::op::res_t::Code::CAST_IMPLICIT_STRING_TO_INT, "[op] CAST_IMPLICIT_STRING_TO_INT" },
            { qb::op::res_t::Code::CAST_NAN_STRING_TO_INT,      "[op] CAST_NAN_STRING_TO_INT" },
            { qb::op::res_t::Code::CAST_IMPLICIT_REF_TO_INT,    "[op] CAST_IMPLICIT_REF_TO_INT" },
            { qb::op::res_t::Code::CAST_IMPLICIT_VEC_TO_INT,    "[op] CAST_IMPLICIT_VEC_TO_INT" },
            { qb::op::res_t::Code::CAST_IMPLICIT_MAP_TO_INT,    "[op] CAST_IMPLICIT_MAP_TO_INT" },
            { qb::op::res_t::Code::CAST_STRUCT_TO_INT,          "[op] CAST_STRUCT_TO_INT" },
            { qb::op::res_t::Code::CAST_IMPLICIT_EVENT_TO_INT,  "[op] CAST_IMPLICIT_EVENT_TO_INT" },
            { qb::op::res_t::Code::CAST_VOID_TO_FLOAT,          "[op] CAST_VOID_TO_FLOAT" },
            { qb::op::res_t::Code::CAST_STRING_TO_FLOAT,        "[op] CAST_STRING_TO_FLOAT" },
            { qb::op::res_t::Code::CAST_REF_TO_FLOAT,           "[op] CAST_REF_TO_FLOAT" },
            { qb::op::res_t::Code::CAST_VECTOR_TO_FLOAT,        "[op] CAST_VECTOR_TO_FLOAT" },
            { qb::op::res_t::Code::CAST_MAP_TO_FLOAT,           "[op] CAST_MAP_TO_FLOAT" },
            { qb::op::res_t::Code::CAST_STRUCT_TO_FLOAT,        "[op] CAST_STRUCT_TO_FLOAT" },
            { qb::op::res_t::Code::CAST_EVENT_TO_FLOAT,         "[op] CAST_EVENT_TO_FLOAT" },
            { qb::op::res_t::Code::CAST_VOID_TO_STRING,         "[op] CAST_VOID_TO_STRING" },
            { qb::op::res_t::Code::CAST_BOOL_TO_STRING,         "[op] CAST_BOOL_TO_STRING" },
            { qb::op::res_t::Code::CAST_INT_TO_STRING,          "[op] CAST_INT_TO_STRING" },
            { qb::op::res_t::Code::CAST_FLOAT_TO_STRING,        "[op] CAST_FLOAT_TO_STRING" },
            { qb::op::res_t::Code::CAST_REF_TO_STRING,          "[op] CAST_REF_TO_STRING" },
            { qb::op::res_t::Code::CAST_VECTOR_TO_STRING,       "[op] CAST_VECTOR_TO_STRING" },
            { qb::op::res_t::Code::CAST_MAP_TO_STRING,          "[op] CAST_MAP_TO_STRING" },
            { qb::op::res_t::Code::CAST_STRUCT_TO_STRING,       "[op] CAST_STRUCT_TO_STRING" },
            { qb::op::res_t::Code::CAST_EVENT_TO_STRING,        "[op] CAST_EVENT_TO_STRING" },
            { qb::op::res_t::Code::CAST_VOID_TO_REF,            "[op] CAST_VOID_TO_REF" },
            { qb::op::res_t::Code::CAST_BOOL_TO_REF,            "[op] CAST_BOOL_TO_REF" },
            { qb::op::res_t::Code::CAST_INT_TO_REF,             "[op] CAST_INT_TO_REF" },
            { qb::op::res_t::Code::CAST_FLOAT_TO_REF,           "[op] CAST_FLOAT_TO_REF" },
            { qb::op::res_t::Code::CAST_STRING_TO_REF,          "[op] CAST_STRING_TO_REF" },
            { qb::op::res_t::Code::CAST_VECTOR_TO_REF,          "[op] CAST_VECTOR_TO_REF" },
            { qb::op::res_t::Code::CAST_MAP_TO_REF,             "[op] CAST_MAP_TO_REF" },
            { qb::op::res_t::Code::CAST_STRUCT_TO_REF,          "[op] CAST_STRUCT_TO_REF" },
            { qb::op::res_t::Code::CAST_EVENT_TO_REF,           "[op] CAST_EVENT_TO_REF" },
            { qb::op::res_t::Code::MATH_BOOL_TARGET,            "[op] MATH_BOOL_TARGET" },
            { qb::op::res_t::Code::MATH_DEC_TARGET,             "[op] MATH_DEC_TARGET" }
        };

        const std::map<uint8_t, std::string> node = {
            { qb::node::res_t::Code::OK,                        "[node] OK" },
            { qb::node::res_t::Code::THREAD_ALREADY_EXISTS,     "[node] THREAD_ALREADY_EXISTS" },
            { qb::node::res_t::Code::THREAD_NOT_FOUND,          "[node] UNKNOWN_SOURCE_TYPE" }
        };

        const std::map<uint8_t, std::string> engine = {
            { qb::engine::res_t::Code::OK,                      "[engine] OK" },
            { qb::engine::res_t::Code::NODE_ALREADY_EXISTS,     "[engine] NODE_ALREADY_EXISTS" },
            { qb::engine::res_t::Code::NODE_NOT_FOUND,          "[engine] UNKNOWN_SOURCE_TYPE" }
        };

    }

}