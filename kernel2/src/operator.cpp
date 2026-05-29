#include "operator.h"

#define OP_ERROR(CODE) return { \
    .code = qb::op::res_t::Code::CODE, \
};

#define OP_OK(OUT) return { \
    .code = qb::op::res_t::Code::OK, \
    .temp = false, \
    .out = OUT \
};

#define OP_OK_TEMP(T, OUT...) return { \
    .code = qb::op::res_t::Code::OK, \
    .temp = true, \
    .out = (data_t) new T(OUT) \
};

// snippets

#define CAST_INT_TO(TO_TYPE, VALUE) \
    if (from_type->flags.of_int.is_unsigned) { \
        switch (from_type->flags.of_int.res) { \
            case 1:             OP_OK_TEMP(TO_TYPE, *(uint8_t*) VALUE) \
            case 2:             OP_OK_TEMP(TO_TYPE, *(uint16_t*) VALUE) \
            case 4:             OP_OK_TEMP(TO_TYPE, *(uint32_t*) VALUE) \
        } break; \
    } \
    else { \
        switch (from_type->flags.of_int.res) { \
            case 1:             OP_OK_TEMP(TO_TYPE, *(int8_t*) VALUE) \
            case 2:             OP_OK_TEMP(TO_TYPE, *(int16_t*) VALUE) \
            case 4:             OP_OK_TEMP(TO_TYPE, *(int32_t*) VALUE) \
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


// [delete temp]

void qb::op::delete_temp(const Type* type, data_t value) {
    switch (type->kind) {
        case qb::TypeKind::VOID:      return;
        case qb::TypeKind::BOOL:      delete (bool*) value; break;
        case qb::TypeKind::INT:       {
            if (type->flags.of_int.is_unsigned) {
                switch (type->flags.of_int.res) {
                    case 1: delete (uint8_t*) value; break;
                    case 2: delete (uint16_t*) value; break;
                    case 4: delete (uint32_t*) value; break;
                }
            }
            else {
                switch (type->flags.of_int.res) {
                    case 1: delete (int8_t*) value; break;
                    case 2: delete (int16_t*) value; break;
                    case 4: delete (int32_t*) value; break;
                }
            }
            break;
        }
        case qb::TypeKind::FLOAT:     delete (float*) value; break;
        case qb::TypeKind::STRING:    delete (std::string*) value; break;
        case qb::TypeKind::REF:       delete (qb::mem::Reference*) value; break;
        case qb::TypeKind::REF_SLICE: delete (qb::mem::SlicedReference*) value; break;
        case qb::TypeKind::VECTOR:    delete (qb::mem::Vector*) value; break;
        case qb::TypeKind::MAP:       delete (qb::mem::Map*) value; break;
        case qb::TypeKind::STRUCT:    delete (qb::mem::Struct*) value; break;
        case qb::TypeKind::EVENT:     delete (qb::mem::Event*) value; break;
    }
}

// [cast: bool <- ?]

qb::op::res_t qb::op::cast_to_bool(const Type* from_type, data_t value) {
    switch (from_type->kind) {
        case qb::TypeKind::VOID:      OP_OK_TEMP(bool, false)
        case qb::TypeKind::BOOL:      OP_OK(value)
        case qb::TypeKind::INT:       CAST_INT_TO(bool, value > 0)
        case qb::TypeKind::FLOAT:     OP_OK_TEMP(bool, *(float*) value > 0)
        case qb::TypeKind::STRING:    OP_OK_TEMP(bool, ((std::string*) value)->size() > 0)
        case qb::TypeKind::REF:       OP_OK_TEMP(bool, true)
        case qb::TypeKind::REF_SLICE: OP_OK_TEMP(bool, true)
        case qb::TypeKind::VECTOR:    OP_OK_TEMP(bool, ((std::vector<void_t>*) value)->size() > 0)
        case qb::TypeKind::MAP:       OP_OK_TEMP(bool, ((std::map<std::string, void_t>*) value)->size() > 0)
        case qb::TypeKind::STRUCT:    OP_OK_TEMP(bool, true)
        case qb::TypeKind::EVENT:     OP_OK_TEMP(bool, ((mem::Event*) value)->code > 0)
    }
    OP_ERROR(UNKNOWN_SOURCE_TYPE)
}

// [cast: int <- ?]

qb::op::res_t qb::op::cast_to_int(const Type* to_type, const Type* from_type, data_t value, bool is_explicit) {
    switch (from_type->kind) {
        case qb::TypeKind::VOID:    OP_ERROR(CAST_VOID_TO_INT)
        case qb::TypeKind::BOOL: {
            CAST_TO_INT(OP_OK_TEMP, *(bool*) value ? 1 : 0)
        }   
        case qb::TypeKind::INT: {
            if (to_type->flags.value == from_type->flags.value) {
                OP_OK(value)
            }
            // Implicit casting doesn't allow data loss
            if (!is_explicit) {
                // Casting (u <- i) fails due to possible data loss
                if (to_type->flags.of_int.is_unsigned && !from_type->flags.of_int.is_unsigned) {
                    OP_ERROR(CAST_INT_TO_UINT)
                }
                // Casting (16|8 <- 32) or (8 <- 16) fails due to possible data loss
                if (to_type->flags.of_int.res < from_type->flags.of_int.res) {
                    OP_ERROR(CAST_INT_SIZE)
                }
                // Casting (i8 <- u8) or (i8|16 <- u16) or (i8|16|32 <- u32) fails due to possible data loss
                if (!to_type->flags.of_int.is_unsigned && from_type->flags.of_int.is_unsigned
                    && to_type->flags.of_int.res <= from_type->flags.of_int.res
                ) {
                    OP_ERROR(CAST_INT_UINT_SIZE)
                }
            }
            CAST_TO_INT(CAST_INT_TO, value)
        }   
        case qb::TypeKind::FLOAT: {
            // Implicit casting doesn't allow data loss
            if (!is_explicit) {
                OP_ERROR(CAST_IMPLICIT_FLOAT_TO_INT)
            }
            // Explicit: round to nearest int
            CAST_TO_INT(OP_OK_TEMP, *(float*) value)
        }   
        case qb::TypeKind::STRING: {
            // Implicit casting doesn't allow string-to-int conversion
            if (!is_explicit) {
                OP_ERROR(CAST_IMPLICIT_STRING_TO_INT)
            }
            // Explicit: parse as int
            auto str = (std::string*) value;
            int32_t out{};
    #if __cpp_lib_to_chars >= 202306L
            if (std::from_chars(str->data(), str->data() + str->size(), out))
    #else
            if (std::from_chars(str->data(), str->data() + str->size(), out).ec == std::errc{})
    #endif
                CAST_TO_INT(OP_OK_TEMP, out)
            else
                OP_ERROR(CAST_NAN_STRING_TO_INT)
        }
        case qb::TypeKind::REF: {
            if (!is_explicit) {
                OP_ERROR(CAST_IMPLICIT_REF_TO_INT)
            }
            CAST_TO_INT(OP_OK_TEMP, ((qb::mem::Reference*) value)->port)
        }
        case qb::TypeKind::REF_SLICE: {
            if (!is_explicit) {
                OP_ERROR(CAST_IMPLICIT_REF_TO_INT)
            }
            CAST_TO_INT(OP_OK_TEMP, ((qb::mem::SlicedReference*) value)->port)
        }
        case qb::TypeKind::VECTOR: {
            if (!is_explicit) {
                OP_ERROR(CAST_IMPLICIT_VEC_TO_INT)
            }
            CAST_TO_INT(OP_OK_TEMP, ((qb::mem::Vector*) value)->size())
        }
        case qb::TypeKind::MAP: {
            if (!is_explicit) {
                OP_ERROR(CAST_IMPLICIT_MAP_TO_INT)
            }
            CAST_TO_INT(OP_OK_TEMP, ((qb::mem::Map*) value)->size())
        }
        case qb::TypeKind::STRUCT:  OP_ERROR(CAST_STRUCT_TO_INT)
        case qb::TypeKind::EVENT: {
            if (!is_explicit) {
                OP_ERROR(CAST_IMPLICIT_EVENT_TO_INT)
            }
            CAST_TO_INT(OP_OK_TEMP, ((qb::mem::Event*) value)->code)
        }
    }
    OP_ERROR(UNKNOWN_SOURCE_TYPE)
}

// [cast: float <- ?]

qb::op::res_t qb::op::cast_to_float(const Type* from_type, data_t value) {
    switch (from_type->kind) {
        case qb::TypeKind::VOID:      OP_ERROR(CAST_VOID_TO_FLOAT)
        case qb::TypeKind::BOOL:      OP_OK_TEMP(float, *(bool*) value ? 1 : 0)
        case qb::TypeKind::INT:       CAST_INT_TO(float, value)
        case qb::TypeKind::FLOAT:     OP_OK(value)
        case qb::TypeKind::STRING:    OP_ERROR(CAST_STRING_TO_FLOAT)
        case qb::TypeKind::REF:       OP_ERROR(CAST_REF_TO_FLOAT)
        case qb::TypeKind::REF_SLICE: OP_ERROR(CAST_REF_TO_FLOAT)
        case qb::TypeKind::VECTOR:    OP_ERROR(CAST_VECTOR_TO_FLOAT)
        case qb::TypeKind::MAP:       OP_ERROR(CAST_MAP_TO_FLOAT)
        case qb::TypeKind::STRUCT:    OP_ERROR(CAST_STRUCT_TO_FLOAT)
        case qb::TypeKind::EVENT:     OP_ERROR(CAST_EVENT_TO_FLOAT)
    }
    OP_ERROR(UNKNOWN_SOURCE_TYPE)
}

// [cast: string <- ?]

qb::op::res_t qb::op::cast_to_string(const Type* from_type, data_t value, bool is_explicit) {
    if (is_explicit) {
        switch (from_type->kind) {
            case qb::TypeKind::VOID:      OP_OK_TEMP(std::string, from_type->flags.of_void.is_void ? "void" : "null")
            case qb::TypeKind::BOOL:      OP_OK_TEMP(std::string, *(bool*) value ? "true" : "false")
            case qb::TypeKind::INT:       {
                if (from_type->flags.of_int.is_unsigned) {
                    switch (from_type->flags.of_int.res) {
                        case 1: OP_OK_TEMP(std::string, std::to_string(*(uint8_t*) value));
                        case 2: OP_OK_TEMP(std::string, std::to_string(*(uint16_t*) value));
                        case 4: OP_OK_TEMP(std::string, std::to_string(*(uint32_t*) value));
                    }
                }
                else {
                    switch (from_type->flags.of_int.res) {
                        case 1: OP_OK_TEMP(std::string, std::to_string(*(int8_t*) value));
                        case 2: OP_OK_TEMP(std::string, std::to_string(*(int16_t*) value));
                        case 4: OP_OK_TEMP(std::string, std::to_string(*(int32_t*) value));
                    }
                }
            }
            case qb::TypeKind::FLOAT:     OP_OK_TEMP(std::string, std::to_string(*(float*) value));
            case qb::TypeKind::STRING:    OP_OK(value)
            case qb::TypeKind::REF:       {
                auto ref = (qb::mem::Reference*) value;
                std::stringstream ss;
                ss << '@' << +ref->block << ':' << +ref->port;
                OP_OK_TEMP(std::string, ss.str());
            }
            case qb::TypeKind::REF_SLICE: {
                auto ref = (qb::mem::SlicedReference*) value;
                std::stringstream ss;
                ss << '@' << +ref->block << ':' << +ref->port;
                for (const auto& it: ref->shape) {
                    ss << '[' << it.start << '~' << it.end << ']';
                }
                OP_OK_TEMP(std::string, ss.str());
            }
            case qb::TypeKind::VECTOR: {
                auto vec = (qb::mem::Vector*) value;
                std::stringstream ss;
                ss << '[';
                auto n = vec->shape.size();
                for (index_t i = 0; i < n; i++) {
                    auto res = qb::op::cast_to_string(vec->item_type, vec->get(i), true);
                    if (res.code != qb::op::res_t::Code::OK) {
                        ss << "??";
                    }
                    else {
                        ss << *(std::string*)(res.out);
                        if (res.temp) delete (std::string*) res.out;
                    }
                    if (i < n-1) ss << ',';
                }
                ss << ']';
                OP_OK_TEMP(std::string, ss.str());
            }
            case qb::TypeKind::MAP:       OP_ERROR(CAST_MAP_TO_STRING)
            case qb::TypeKind::STRUCT:    OP_ERROR(CAST_STRUCT_TO_STRING)
            case qb::TypeKind::EVENT:     OP_ERROR(CAST_EVENT_TO_STRING)
        }
    }
    else {
        switch (from_type->kind) {
            case qb::TypeKind::VOID:      OP_ERROR(CAST_VOID_TO_STRING)
            case qb::TypeKind::BOOL:      OP_ERROR(CAST_BOOL_TO_STRING)
            case qb::TypeKind::INT:       OP_ERROR(CAST_INT_TO_STRING)
            case qb::TypeKind::FLOAT:     OP_ERROR(CAST_FLOAT_TO_STRING)
            case qb::TypeKind::STRING:    OP_OK(value)
            case qb::TypeKind::REF:       OP_ERROR(CAST_REF_TO_STRING)
            case qb::TypeKind::REF_SLICE: OP_ERROR(CAST_REF_TO_STRING)
            case qb::TypeKind::VECTOR:    OP_ERROR(CAST_VECTOR_TO_STRING)
            case qb::TypeKind::MAP:       OP_ERROR(CAST_MAP_TO_STRING)
            case qb::TypeKind::STRUCT:    OP_ERROR(CAST_STRUCT_TO_STRING)
            case qb::TypeKind::EVENT:     OP_ERROR(CAST_EVENT_TO_STRING)
        }
    }
    OP_ERROR(UNKNOWN_SOURCE_TYPE)
}

// [cast: ref <- ?]

qb::op::res_t qb::op::cast_to_ref(const Type* from_type, data_t value) {
    switch (from_type->kind) {
        case qb::TypeKind::VOID:      OP_ERROR(CAST_VOID_TO_REF)
        case qb::TypeKind::BOOL:      OP_ERROR(CAST_BOOL_TO_REF)
        case qb::TypeKind::INT:       OP_ERROR(CAST_INT_TO_REF)
        case qb::TypeKind::FLOAT:     OP_ERROR(CAST_FLOAT_TO_REF)
        case qb::TypeKind::STRING:    OP_ERROR(CAST_STRING_TO_REF)
        case qb::TypeKind::REF:       OP_OK(value)
        case qb::TypeKind::REF_SLICE: {
            auto ref_slice = (qb::mem::SlicedReference*) value;
            OP_OK_TEMP(qb::mem::Reference, ref_slice->block, ref_slice->port)
        }
        case qb::TypeKind::VECTOR:    OP_ERROR(CAST_VECTOR_TO_REF)
        case qb::TypeKind::MAP:       OP_ERROR(CAST_MAP_TO_REF)
        case qb::TypeKind::STRUCT:    OP_ERROR(CAST_STRUCT_TO_REF)
        case qb::TypeKind::EVENT:     OP_ERROR(CAST_EVENT_TO_REF)
    }
    OP_ERROR(UNKNOWN_SOURCE_TYPE)
}

qb::op::res_t qb::op::cast_to_ref_slice(const Type* from_type, data_t value) {
    switch (from_type->kind) {
        case qb::TypeKind::VOID:      OP_ERROR(CAST_VOID_TO_REF)
        case qb::TypeKind::BOOL:      OP_ERROR(CAST_BOOL_TO_REF)
        case qb::TypeKind::INT:       OP_ERROR(CAST_INT_TO_REF)
        case qb::TypeKind::FLOAT:     OP_ERROR(CAST_FLOAT_TO_REF)
        case qb::TypeKind::STRING:    OP_ERROR(CAST_STRING_TO_REF)
        case qb::TypeKind::REF:       {
            auto ref = (qb::mem::Reference*) value;
            OP_OK_TEMP(qb::mem::SlicedReference, ref->block, ref->port, {})
        }
        case qb::TypeKind::REF_SLICE: OP_OK(value)
        case qb::TypeKind::VECTOR:    OP_ERROR(CAST_VECTOR_TO_REF)
        case qb::TypeKind::MAP:       OP_ERROR(CAST_MAP_TO_REF)
        case qb::TypeKind::STRUCT:    OP_ERROR(CAST_STRUCT_TO_REF)
        case qb::TypeKind::EVENT:     OP_ERROR(CAST_EVENT_TO_REF)
    }
    OP_ERROR(UNKNOWN_SOURCE_TYPE)
}

// [cast]

qb::op::res_t qb::op::cast(const Type* to_type, const Type* from_type, data_t value, bool is_explicit) {
    if (to_type == from_type) {
        OP_OK(value)
    }
    switch (to_type->kind) {
        case qb::TypeKind::VOID:      OP_OK(nullptr)
        case qb::TypeKind::BOOL:      return cast_to_bool(from_type, value);
        case qb::TypeKind::INT:       return cast_to_int(to_type, from_type, value, is_explicit);
        case qb::TypeKind::FLOAT:     return cast_to_float(from_type, value);
        case qb::TypeKind::STRING:    return cast_to_string(from_type, value, is_explicit);
        case qb::TypeKind::REF:       return cast_to_ref(from_type, value);
        case qb::TypeKind::REF_SLICE: return cast_to_ref_slice(from_type, value);
        case qb::TypeKind::VECTOR:    OP_ERROR(NOT_IMPLEMENTED)
        case qb::TypeKind::MAP:       OP_ERROR(NOT_IMPLEMENTED)
        case qb::TypeKind::STRUCT:    OP_ERROR(NOT_IMPLEMENTED)
        case qb::TypeKind::EVENT:     OP_ERROR(NOT_IMPLEMENTED)
    }
    OP_ERROR(UNKNOWN_TARGET_TYPE)
}

// [assign]

qb::op::res_t qb::op::assign(mem::Block& t_block, port_t t_port, mem::Block& s_block, port_t s_port, bool explicit_cast) {
    if (t_port >= t_block.data.size())
        OP_ERROR(ASSIGN_PORT_OUT_OF_BOUNDS)

    // Assignment to const not allowed
    if (t_block.data.type->schema.of_struct.is_const[t_port])
        OP_ERROR(ASSIGN_TO_CONST)

    auto t_type = t_block.type_of(t_port);
    
    // Assignment to void does nothing
    if (t_type->kind == qb::TypeKind::VOID)
        OP_OK(nullptr)

    // Get source type and data
    auto s_type = s_block.type_of(s_port);
    auto s_data = s_block.data.get(s_port);

    // Attempt cast
    auto res = qb::op::cast(t_type, s_type, s_data, explicit_cast);
    if (res.code > 0) return res;

    // Copy value
    t_block.data.set(t_port, res.out);

    // Delete temporary value (created by cast)
    if (res.temp) qb::op::delete_temp(t_type, res.out);
}

#define OP_MATH(OP) { \
    switch (t_type->kind) { \
        case qb::TypeKind::INT: \
            if (t_type->flags.of_int.is_unsigned) { \
                switch (t_type->flags.of_int.res) { \
                    case 1: { \
                        uint8_t out = (*(uint8_t*) t_data) OP (*(uint8_t*) res.out); \
                        t_block.data.set(t_port, (data_t) &out); \
                        break; \
                    } \
                    case 2: { \
                        uint16_t out = (*(uint16_t*) t_data) OP (*(uint16_t*) res.out); \
                        t_block.data.set(t_port, (data_t) &out); \
                        break; \
                    } \
                    case 4: { \
                        uint32_t out = (*(uint32_t*) t_data) OP (*(uint32_t*) res.out); \
                        t_block.data.set(t_port, (data_t) &out); \
                        break; \
                    } \
                } \
            } \
            else { \
                switch (t_type->flags.of_int.res) { \
                    case 1: { \
                        int8_t out = (*(int8_t*) t_data) OP (*(int8_t*) res.out); \
                        t_block.data.set(t_port, (data_t) &out); \
                        break; \
                    } \
                    case 2: { \
                        int16_t out = (*(int16_t*) t_data) OP (*(int16_t*) res.out); \
                        t_block.data.set(t_port, (data_t) &out); \
                        break; \
                    } \
                    case 4: { \
                        int32_t out = (*(int32_t*) t_data) OP (*(int32_t*) res.out); \
                        t_block.data.set(t_port, (data_t) &out); \
                        break; \
                    } \
                } \
            } \
            break; \
        case qb::TypeKind::FLOAT: { \
            float out = (*(float*) t_data) OP (*(float*) res.out); \
            t_block.data.set(t_port, (data_t) &out); \
            break; \
        } \
    } \
}

#define OP_COMPARE(OP) { \
    switch (l_type->kind) { \
        case qb::TypeKind::INT: \
            if (l_type->flags.of_int.is_unsigned) { \
                switch (l_type->flags.of_int.res) { \
                    case 1: { \
                        compare_res = (*(uint8_t*) l_data) OP (*(uint8_t*) res.out); \
                        break; \
                    } \
                    case 2: { \
                        compare_res = (*(uint16_t*) l_data) OP (*(uint16_t*) res.out); \
                        break; \
                    } \
                    case 4: { \
                        compare_res = (*(uint32_t*) l_data) OP (*(uint32_t*) res.out); \
                        break; \
                    } \
                } \
            } \
            else { \
                switch (l_type->flags.of_int.res) { \
                    case 1: { \
                        compare_res = (*(int8_t*) l_data) OP (*(int8_t*) res.out); \
                        break; \
                    } \
                    case 2: { \
                        compare_res = (*(int16_t*) l_data) OP (*(int16_t*) res.out); \
                        break; \
                    } \
                    case 4: { \
                        compare_res = (*(int32_t*) l_data) OP (*(int32_t*) res.out); \
                        break; \
                    } \
                } \
            } \
            break; \
        case qb::TypeKind::FLOAT: { \
            compare_res = (*(float*) l_data) OP (*(float*) res.out); \
            break; \
        } \
    } \
}

qb::op::res_t qb::op::math(qb::instruction::Math::Flags::Op op, mem::Block& t_block, port_t t_port, mem::Block& s_block, port_t s_port) {
    auto t_type = t_block.type_of(t_port);
    
    // Guard operation by type
    auto is_bool_math = op < 0x10;
    if (is_bool_math) {
        if (t_type->kind != qb::TypeKind::BOOL) 
            OP_ERROR(MATH_BOOL_TARGET)
    }
    else {
        if (t_type->kind != qb::TypeKind::INT && t_type->kind != qb::TypeKind::FLOAT)
            OP_ERROR(MATH_DEC_TARGET)
    }

    // Get source type and data
    auto s_type = s_block.type_of(s_port);
    auto s_data = s_block.data.get(s_port);

    // Attempt cast
    auto res = qb::op::cast(t_type, s_type, s_data);
    if (res.code > 0) return res;

    // Get target data
    auto t_data = t_block.data.get(t_port);

    // Run operation
    if (is_bool_math) {
        bool out;
        switch (op) {
            case qb::instruction::Math::Flags::Op::NOT: out = !(*(bool*) res.out); break;
            case qb::instruction::Math::Flags::Op::AND: out = (*(bool*) t_data) && (*(bool*) res.out); break;
            case qb::instruction::Math::Flags::Op::OR:  out = (*(bool*) t_data) || (*(bool*) res.out); break;
        }
        t_block.data.set(t_port, (data_t) &out);
    }
    else {
        switch (op) {
            case qb::instruction::Math::Flags::Op::ADD:  OP_MATH(+); break;
            case qb::instruction::Math::Flags::Op::SUB:  OP_MATH(-); break;
            case qb::instruction::Math::Flags::Op::MULT: OP_MATH(*); break;
            case qb::instruction::Math::Flags::Op::DIV:  OP_MATH(/); break;
            // case qb::instruction::Math::Flags::MOD:  OP_MATH(%); break;
            // case qb::instruction::Math::Flags::POW:  OP_MATH(); break;
            // case qb::instruction::Math::Flags::LN:
        }
    }

    // Delete temporary value (created by cast)
    if (res.temp) qb::op::delete_temp(t_type, res.out);

    OP_OK(t_data)
}

qb::op::res_t qb::op::compare(qb::instruction::CompareOp op, mem::Block& l_block, port_t l_port, mem::Block& r_block, port_t r_port) {
    auto l_type = l_block.type_of(l_port);
    
    // Get source type and data
    auto r_type = r_block.type_of(r_port);
    auto r_data = r_block.data.get(r_port);

    // Attempt cast
    auto res = qb::op::cast(l_type, r_type, r_data);
    if (res.code > 0) return res;

    // Get target data
    auto l_data = l_block.data.get(l_port);

    bool compare_res = false;
    switch (op) {
        case qb::instruction::CompareOp::EQ: OP_COMPARE(==); break;
        case qb::instruction::CompareOp::GT: OP_COMPARE(>); break;
        case qb::instruction::CompareOp::LT: OP_COMPARE(<); break;
    }

    // Delete temporary value (created by cast)
    if (res.temp) qb::op::delete_temp(l_type, res.out);

    OP_OK_TEMP(bool, compare_res)
}