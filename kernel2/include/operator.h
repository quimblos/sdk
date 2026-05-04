#pragma once
#include <charconv>
#include "memory.h"

#define ERROR(CODE) return { \
    .code = qb::op::res_t::Code::CODE, \
};

#define OK(OUT) return { \
    .code = res_t::Code::OK, \
    .temp = false, \
    .out = OUT \
};

#define OK_TEMP(T, OUT) return { \
    .code = res_t::Code::OK, \
    .temp = true, \
    .out = (data_t) new T(OUT) \
};

#define CAST_INT_TO(TO_TYPE, VALUE) \
    if (from_type->flags.of_int.is_unsigned) { \
        switch (from_type->flags.of_int.res) { \
            case 1:             OK_TEMP(TO_TYPE, *(uint8_t*) VALUE) \
            case 2:             OK_TEMP(TO_TYPE, *(uint16_t*) VALUE) \
            case 4:             OK_TEMP(TO_TYPE, *(uint32_t*) VALUE) \
        } break; \
    } \
    else { \
        switch (from_type->flags.of_int.res) { \
            case 1:             OK_TEMP(TO_TYPE, *(int8_t*) VALUE) \
            case 2:             OK_TEMP(TO_TYPE, *(int16_t*) VALUE) \
            case 4:             OK_TEMP(TO_TYPE, *(int32_t*) VALUE) \
        } break; \
    }

#define CAST_TO_INT(METHOD, VALUE) \
    if (to_type->flags.of_int.is_unsigned) { \
        switch (to_type->flags.of_int.res) { \
            case 1:             METHOD(uint8_t, VALUE) \
            case 2:             METHOD(uint16_t, VALUE) \
            case 4:             METHOD(uint32_t, VALUE) \
        } break; \
    } \
    else { \
        switch (to_type->flags.of_int.res) { \
            case 1:             METHOD(int8_t, VALUE) \
            case 2:             METHOD(int16_t, VALUE) \
            case 4:             METHOD(int32_t, VALUE) \
        } break; \
    }

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
                CAST_EVENT_TO_REF
            } code : 7;
            bool temp : 1;
            data_t out;
        };

        res_t cast_to_bool(const Type* to_type, const Type* from_type, data_t value) {
            switch (from_type->kind) {
                case qb::TypeKind::VOID:    OK_TEMP(bool, false)
                case qb::TypeKind::BOOL:    OK(value)
                case qb::TypeKind::INT:     CAST_INT_TO(bool, value > 0)
                case qb::TypeKind::FLOAT:   OK_TEMP(bool, *(float*) value > 0)
                case qb::TypeKind::STRING:  OK_TEMP(bool, ((std::string*) value)->size() > 0)
                case qb::TypeKind::REF:     OK_TEMP(bool, true)
                case qb::TypeKind::VECTOR:  OK_TEMP(bool, ((std::vector<void_t>*) value)->size() > 0)
                case qb::TypeKind::MAP:     OK_TEMP(bool, ((std::map<std::string, void_t>*) value)->size() > 0)
                case qb::TypeKind::STRUCT:  OK_TEMP(bool, true)
                case qb::TypeKind::EVENT:   OK_TEMP(bool, ((mem::Event*) value)->code > 0)
            }
            ERROR(UNKNOWN_SOURCE_TYPE)
        }

        res_t cast_to_int(const Type* to_type, const Type* from_type, data_t value, bool is_explicit = false) {
            switch (from_type->kind) {
                case qb::TypeKind::VOID:    ERROR(CAST_VOID_TO_INT)
                case qb::TypeKind::BOOL: {
                    CAST_TO_INT(OK_TEMP, *(bool*) value ? 1 : 0)
                }   
                case qb::TypeKind::INT: {
                    if (to_type->flags.value == from_type->flags.value) {
                        OK(value)
                    }
                    // Implicit casting doesn't allow data loss
                    if (!is_explicit) {
                        // Casting (u <- i) fails due to possible data loss
                        if (to_type->flags.of_int.is_unsigned && !from_type->flags.of_int.is_unsigned) {
                            ERROR(CAST_INT_TO_UINT)
                        }
                        // Casting (16|8 <- 32) or (8 <- 16) fails due to possible data loss
                        if (to_type->flags.of_int.res < from_type->flags.of_int.res) {
                            ERROR(CAST_INT_SIZE)
                        }
                        // Casting (i8 <- u8) or (i8|16 <- u16) or (i8|16|32 <- u32) fails due to possible data loss
                        if (!to_type->flags.of_int.is_unsigned && from_type->flags.of_int.is_unsigned
                            && to_type->flags.of_int.res <= from_type->flags.of_int.res
                        ) {
                            ERROR(CAST_INT_UINT_SIZE)
                        }
                    }
                    CAST_TO_INT(CAST_INT_TO, value)
                }   
                case qb::TypeKind::FLOAT: {
                    // Implicit casting doesn't allow data loss
                    if (!is_explicit) {
                        ERROR(CAST_IMPLICIT_FLOAT_TO_INT)
                    }
                    // Explicit: round to nearest int
                    CAST_TO_INT(OK_TEMP, *(float*) value)
                }   
                case qb::TypeKind::STRING: {
                    // Implicit casting doesn't allow string-to-int conversion
                    if (!is_explicit) {
                        ERROR(CAST_IMPLICIT_STRING_TO_INT)
                    }
                    // Explicit: parse as int
                    auto str = (std::string*) value;
                    int32_t out{};
            #if __cpp_lib_to_chars >= 202306L
                    if (std::from_chars(str->data(), str->data() + str->size(), out))
            #else
                    if (std::from_chars(str->data(), str->data() + str->size(), out).ec == std::errc{})
            #endif
                        CAST_TO_INT(OK_TEMP, out)
                    else
                        ERROR(CAST_NAN_STRING_TO_INT)
                }
                case qb::TypeKind::REF: {
                    if (!is_explicit) {
                        ERROR(CAST_IMPLICIT_REF_TO_INT)
                    }
                    CAST_TO_INT(OK_TEMP, ((qb::mem::Reference*) value)->port)
                }
                case qb::TypeKind::VECTOR: {
                    if (!is_explicit) {
                        ERROR(CAST_IMPLICIT_VEC_TO_INT)
                    }
                    CAST_TO_INT(OK_TEMP, ((qb::mem::Vector*) value)->size())
                }
                case qb::TypeKind::MAP: {
                    if (!is_explicit) {
                        ERROR(CAST_IMPLICIT_MAP_TO_INT)
                    }
                    CAST_TO_INT(OK_TEMP, ((qb::mem::Map*) value)->size())
                }
                case qb::TypeKind::STRUCT:  ERROR(CAST_STRUCT_TO_INT)
                case qb::TypeKind::EVENT: {
                    if (!is_explicit) {
                        ERROR(CAST_IMPLICIT_EVENT_TO_INT)
                    }
                    CAST_TO_INT(OK_TEMP, ((qb::mem::Event*) value)->code)
                }
            }
            ERROR(UNKNOWN_SOURCE_TYPE)
        }

        res_t cast_to_float(const Type* to_type, const Type* from_type, data_t value) {
            switch (from_type->kind) {
                case qb::TypeKind::VOID:    ERROR(CAST_VOID_TO_FLOAT)
                case qb::TypeKind::BOOL:    OK_TEMP(float, *(bool*) value ? 1 : 0)
                case qb::TypeKind::INT:     CAST_INT_TO(float, value)
                case qb::TypeKind::FLOAT:   OK(value)
                case qb::TypeKind::STRING:  ERROR(CAST_STRING_TO_FLOAT)
                case qb::TypeKind::REF:     ERROR(CAST_REF_TO_FLOAT)
                case qb::TypeKind::VECTOR:  ERROR(CAST_VECTOR_TO_FLOAT)
                case qb::TypeKind::MAP:     ERROR(CAST_MAP_TO_FLOAT)
                case qb::TypeKind::STRUCT:  ERROR(CAST_STRUCT_TO_FLOAT)
                case qb::TypeKind::EVENT:   ERROR(CAST_EVENT_TO_FLOAT)
            }
            ERROR(UNKNOWN_SOURCE_TYPE)
        }

        res_t cast_to_string(const Type* to_type, const Type* from_type, data_t value) {
            switch (from_type->kind) {
                case qb::TypeKind::VOID:    ERROR(CAST_VOID_TO_STRING)
                case qb::TypeKind::BOOL:    ERROR(CAST_BOOL_TO_STRING)
                case qb::TypeKind::INT:     ERROR(CAST_INT_TO_STRING)
                case qb::TypeKind::FLOAT:   ERROR(CAST_FLOAT)
                case qb::TypeKind::STRING:  OK(value)
                case qb::TypeKind::REF:     ERROR(CAST_REF_TO_STRING)
                case qb::TypeKind::VECTOR:  ERROR(CAST_VECTOR_TO_STRING)
                case qb::TypeKind::MAP:     ERROR(CAST_MAP_TO_STRING)
                case qb::TypeKind::STRUCT:  ERROR(CAST_STRUCT_TO_STRING)
                case qb::TypeKind::EVENT:   ERROR(CAST_EVENT_TO_STRING)
            }
            ERROR(UNKNOWN_SOURCE_TYPE)
        }

        res_t cast_to_ref(const Type* to_type, const Type* from_type, data_t value) {
            switch (from_type->kind) {
                case qb::TypeKind::VOID:    ERROR(CAST_VOID_TO_REF)
                case qb::TypeKind::BOOL:    ERROR(CAST_BOOL_TO_REF)
                case qb::TypeKind::INT:     ERROR(CAST_INT_TO_REF)
                case qb::TypeKind::FLOAT:   ERROR(CAST_FLOAT_TO_REF)
                case qb::TypeKind::STRING:  ERROR(CAST_STRING_TO_REF)
                case qb::TypeKind::REF:     OK(value)
                case qb::TypeKind::VECTOR:  ERROR(CAST_VECTOR_TO_REF)
                case qb::TypeKind::MAP:     ERROR(CAST_MAP_TO_REF)
                case qb::TypeKind::STRUCT:  ERROR(CAST_STRUCT_TO_REF)
                case qb::TypeKind::EVENT:   ERROR(CAST_EVENT_TO_REF)
            }
            ERROR(UNKNOWN_SOURCE_TYPE)
        }

        res_t cast(const Type* to_type, const Type* from_type, data_t value, bool is_explicit = false) {
            if (to_type == from_type) {
                OK(value)
            }
            switch (to_type->kind) {
                case qb::TypeKind::VOID:    OK(nullptr)
                case qb::TypeKind::BOOL:    return cast_to_bool(to_type, from_type, value);
                case qb::TypeKind::INT:     return cast_to_int(to_type, from_type, value, is_explicit);
                case qb::TypeKind::FLOAT:   return cast_to_float(to_type, from_type, value);
                case qb::TypeKind::STRING:  return cast_to_string(to_type, from_type, value);
                case qb::TypeKind::REF:     return cast_to_ref(to_type, from_type, value);
                case qb::TypeKind::VECTOR:  ERROR(NOT_IMPLEMENTED)
                case qb::TypeKind::MAP:     ERROR(NOT_IMPLEMENTED)
                case qb::TypeKind::STRUCT:  ERROR(NOT_IMPLEMENTED)
                case qb::TypeKind::EVENT:   ERROR(NOT_IMPLEMENTED)
            }
            ERROR(UNKNOWN_TARGET_TYPE)
        }

    }

}