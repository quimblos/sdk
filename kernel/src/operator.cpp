#include "operator.h"

#define ASSERT_NO_ERROR(X) if (X.error != nullptr) return X;

qb::data_t qb::BOOL_TARGET_TYPE = {
    .type = qb::DataType::BOOL,
    .value = nullptr
};

qb::data_t qb::U32_TARGET_TYPE = {
    .type = qb::DataType::UINT32,
    .value = nullptr
};

void qb::_operator::clean_heap(qb::_operator::res_t* res) {
    if (res->data != nullptr) {
        if (res->data->heap) {
            qb::_operator::clean_data(res->data);
            delete res->data;
        }
    }
    if (res->error != nullptr) {
        delete res->error;
    }
}

void qb::_operator::clean_data(qb::data_t* data) {
    switch (data->type) {
        case qb::DataType::_NULL: break;
        case qb::DataType::ERROR: delete ((qb::data::Error*) data->value); break;
        case qb::DataType::BOOL: delete ((bool*) data->value); break;
        case qb::DataType::UINT8: delete ((uint8_t*) data->value); break;
        case qb::DataType::INT8: delete ((int8_t*) data->value); break;
        case qb::DataType::UINT16: delete ((uint16_t*) data->value); break;
        case qb::DataType::INT16: delete ((int16_t*) data->value); break;
        case qb::DataType::UINT32: delete ((uint32_t*) data->value); break;
        case qb::DataType::INT32: delete ((int32_t*) data->value); break;
        case qb::DataType::FLOAT32: delete ((float*) data->value); break;
        case qb::DataType::STRING: delete ((std::string*) data->value); break;
        case qb::DataType::VECTOR: {
            auto vec = (qb::data::Vector<void>*) data->value;
            switch (vec->item_type) {
                case qb::DataType::ERROR: delete (qb::data::Vector<qb::data::Error>*) data->value; break;
                case qb::DataType::BOOL: delete (qb::data::Vector<bool>*) data->value; break;
                case qb::DataType::UINT8: delete (qb::data::Vector<uint8_t>*) data->value; break;
                case qb::DataType::INT8: delete (qb::data::Vector<int8_t>*) data->value; break;
                case qb::DataType::UINT16: delete (qb::data::Vector<uint16_t>*) data->value; break;
                case qb::DataType::INT16: delete (qb::data::Vector<int16_t>*) data->value; break;
                case qb::DataType::UINT32: delete (qb::data::Vector<uint32_t>*) data->value; break;
                case qb::DataType::INT32: delete (qb::data::Vector<int32_t>*) data->value; break;
                case qb::DataType::FLOAT32: delete (qb::data::Vector<float>*) data->value; break;
                case qb::DataType::STRING: delete (qb::data::Vector<std::string>*) data->value; break;
                case qb::DataType::REF: delete (qb::data::Vector<qb::data::Reference>*) data->value; break;
            }
            break;
        };
        case qb::DataType::VECTOR_SLICE: {
            auto slice = (qb::data_slice_t*) data->value;
            if (slice->heap) {
                auto item_data = (qb::data_t*) data->value;
                clean_data(item_data);
            }
            delete slice;
            break;
        };
        case qb::DataType::REF: delete (qb::data::Reference*) data->value; break;
    }
}

// Vector / Slice helpers

bool match_shape(qb::data::Vector<void>* vec0, qb::data::Vector<void>* vec1) {
    if (vec0->dims != vec1->dims)
        return false;
    for (uint8_t i = 0; i < vec0->dims; i++) {
        if (vec0->shape[i] != vec1->shape[i])
            return false;
    }
    return true;
}
bool match_shape(qb::data_slice_t* vs, qb::data::Vector<void>* vec) {
    if (vs->slice->dims != vec->dims)
        return false;
    for (uint8_t i = 0; i < vs->slice->dims; i++) {
        qb::index_t vs_shape = vs->slice->start[i] - vs->slice->end[i];
        if (vs_shape != vec->shape[i])
            return false;
    }
    return true;
}
bool match_shape(qb::data_slice_t* vs0, qb::data_slice_t* vs1) {
    if (vs0->slice->dims != vs1->slice->dims)
        return false;
    for (uint8_t i = 0; i < vs0->slice->dims; i++) {
        qb::index_t vs0_shape = vs0->slice->start[i] - vs0->slice->end[i];
        qb::index_t vs1_shape = vs1->slice->start[i] - vs1->slice->end[i];
        if (vs0_shape != vs1_shape)
            return false;
    }
    return true;
}

bool match_item_type(qb::data::Vector<void>* vec0, qb::data::Vector<void>* vec1) {
    return vec0->item_type == vec1->item_type;
}
bool match_item_type(qb::data_slice_t* vs, qb::data::Vector<void>* vec) {
    return vs->type == vec->item_type;
}
bool match_item_type(qb::data_slice_t* vs0, qb::data_slice_t* vs1) {
    return vs0->type == vs1->type;
}
bool match_item_type(qb::data_slice_t* vs, qb::type_t type) {
    return vs->type == type;
}

// Operators

qb::_operator::res_t qb::_operator::cast(qb::data_t* target, qb::data_t* source) {

    switch (target->type) {
        case qb::DataType::_NULL: {
            switch (source->type) {
                case qb::DataType::_NULL:
                    return { .data = source };
                default:
                    return ERROR("Cast to NULL only allowed from NULL.");
            }
        }
        case qb::DataType::ERROR: {
            switch (source->type) {
                case qb::DataType::ERROR:
                    return { .data = source };
                case qb::DataType::STRING:
                    return {
                        .data = new data_t({
                            .type = qb::DataType::ERROR,
                            .value = qb::data::error(0, *(std::string*) source->value),
                            .heap = true
                        })
                    };
                default:
                    return ERROR("Cast to ERROR only allowed from ERROR or STRING.");
            }
        }
        case qb::DataType::BOOL: {
            if (source->type == qb::DataType::BOOL) return { .data = source };
            bool* value = new bool;
            switch (source->type) {
                case qb::DataType::UINT8:
                    *value = (*(uint8_t*) source->value) != 0; break;
                case qb::DataType::INT8:
                    *value = (*(int8_t*) source->value) != 0; break;
                case qb::DataType::UINT16:
                    *value = (*(uint16_t*) source->value) != 0; break;
                case qb::DataType::INT16:
                    *value = (*(int16_t*) source->value) != 0; break;
                case qb::DataType::UINT32:
                    *value = (*(uint32_t*) source->value) != 0; break;
                case qb::DataType::INT32:
                    *value = (*(int32_t*) source->value) != 0; break;
                case qb::DataType::FLOAT32:
                    *value = (*(float*) source->value) != 0; break;
                default:
                    delete value;
                    return ERROR("Cast to NULL only allowed from NULL.");
            }
            return {
                .data = new data_t({
                    .type = qb::DataType::BOOL,
                    .value = value,
                    .heap = true
                })
            };
        }
        case qb::DataType::UINT8: {
            if (source->type == qb::DataType::UINT8) return { .data = source };
            uint8_t* value = new uint8_t;

            switch (source->type) {
                case qb::DataType::BOOL:
                    *value = (*(bool*) source->value) ? 1 : 0; break;
                default:
                    delete value;
                    return ERROR("Cast to UINT8 only allowed from BOOL or UINT8.");
            }
            return {
                .data = new data_t({
                    .type = qb::DataType::UINT8,
                    .value = value,
                    .heap = true
                })
            };
        }
        case qb::DataType::INT8: {
            if (source->type == qb::DataType::INT8) return { .data = source };
            int8_t* value = new int8_t;

            switch (source->type) {
                case qb::DataType::BOOL:
                    *value = (*(bool*) source->value) ? 1 : 0; break;
                case qb::DataType::UINT8:
                    *value = (*(uint8_t*) source->value); break;
                default:
                    delete value;
                    return ERROR("Cast to INT8 only allowed from BOOL, UINT8 or INT8.");
            }
            return {
                .data = new data_t({
                    .type = qb::DataType::INT8,
                    .value = value,
                    .heap = true
                })
            };
        }
        case qb::DataType::UINT16: {
            if (source->type == qb::DataType::UINT16) return { .data = source };
            uint16_t* value = new uint16_t;

            switch (source->type) {
                case qb::DataType::BOOL:
                    *value = (*(bool*) source->value) ? 1 : 0; break;
                case qb::DataType::UINT8:
                    *value = (*(uint8_t*) source->value); break;
                case qb::DataType::INT8:
                    *value = (*(int8_t*) source->value); break;
                default:
                    delete value;
                    return ERROR("Cast to UINT16 only allowed from BOOL, UINT8, INT8 or UINT16.");
            }
            return {
                .data = new data_t({
                    .type = qb::DataType::UINT16,
                    .value = value,
                    .heap = true
                })
            };
        }
        case qb::DataType::INT16: {
            if (source->type == qb::DataType::INT16) return { .data = source };
            int16_t* value = new int16_t;

            switch (source->type) {
                case qb::DataType::BOOL:
                    *value = (*(bool*) source->value) ? 1 : 0; break;
                case qb::DataType::UINT8:
                    *value = (*(uint8_t*) source->value); break;
                case qb::DataType::INT8:
                    *value = (*(int8_t*) source->value); break;
                case qb::DataType::UINT16:
                    *value = (*(uint16_t*) source->value); break;
                default:
                    delete value;
                    return ERROR("Cast to INT16 only allowed from BOOL, UINT8, INT8, UINT16 or INT16.");
            }
            return {
                .data = new data_t({
                    .type = qb::DataType::INT16,
                    .value = value,
                    .heap = true
                })
            };
        }
        case qb::DataType::UINT32: {
            if (source->type == qb::DataType::UINT32) return { .data = source };
            uint32_t* value = new uint32_t;

            switch (source->type) {
                case qb::DataType::BOOL:
                    *value = (*(bool*) source->value) ? 1 : 0; break;
                case qb::DataType::UINT8:
                    *value = (*(uint8_t*) source->value); break;
                case qb::DataType::INT8:
                    *value = (*(int8_t*) source->value); break;
                case qb::DataType::UINT16:
                    *value = (*(uint16_t*) source->value); break;
                case qb::DataType::INT16:
                    *value = (*(int16_t*) source->value); break;
                default:
                    delete value;
                    return ERROR("Cast to UINT32 only allowed from BOOL, UINT8, INT8, UINT16, INT16 or UINT32.");
            }

            return {
                .data = new data_t({
                    .type = qb::DataType::UINT32,
                    .value = value,
                    .heap = true
                })
            };
        }
        case qb::DataType::INT32: {
            if (source->type == qb::DataType::INT32) return { .data = source };
            int32_t* value = new int32_t;

            switch (source->type) {
                case qb::DataType::BOOL:
                    *value = (*(bool*) source->value) ? 1 : 0; break;
                case qb::DataType::UINT8:
                    *value = (*(uint8_t*) source->value); break;
                case qb::DataType::INT8:
                    *value = (*(int8_t*) source->value); break;
                case qb::DataType::UINT16:
                    *value = (*(uint16_t*) source->value); break;
                case qb::DataType::INT16:
                    *value = (*(int16_t*) source->value); break;
                case qb::DataType::UINT32:
                    *value = (*(uint32_t*) source->value); break;
                default:
                    delete value;
                    return ERROR("Cast to INT32 only allowed from BOOL, UINT8, INT8, UINT16, INT16, UINT32 or INT32.");
            }

            return {
                .data = new data_t({
                    .type = qb::DataType::INT32,
                    .value = value,
                    .heap = true
                })
            };
        }
        case qb::DataType::FLOAT32: {
            if (source->type == qb::DataType::FLOAT32) return { .data = source };
            float* value = new float;

            switch (source->type) {
                case qb::DataType::BOOL:
                    *value = (*(bool*) source->value) ? 1 : 0; break;
                case qb::DataType::UINT8:
                    *value = (*(uint8_t*) source->value); break;
                case qb::DataType::INT8:
                    *value = (*(int8_t*) source->value); break;
                case qb::DataType::UINT16:
                    *value = (*(uint16_t*) source->value); break;
                case qb::DataType::INT16:
                    *value = (*(int16_t*) source->value); break;
                case qb::DataType::UINT32:
                    *value = (*(uint32_t*) source->value); break;
                case qb::DataType::INT32:
                    *value = (*(int32_t*) source->value); break;
                case qb::DataType::FLOAT32:
                    *value = (*(float*) source->value); break;
                default:
                    delete value;
                    return ERROR("Cast to FLOAT32 only allowed from BOOL, UINT8, INT8, UINT16, INT16, UINT32, INT32 or FLOAT32.");
            }

            return {
                .data = new data_t({
                    .type = qb::DataType::FLOAT32,
                    .value = value,
                    .heap = true
                })
            };
        }
        case qb::DataType::STRING: {
            if (source->type == qb::DataType::STRING) return { .data = source };
            std::string* value = new std::string();

            switch (source->type) {
                case qb::DataType::ERROR:
                    *value = std::string(((data::Error*)source->value)->message);
                case qb::DataType::BOOL:
                    *value = std::to_string((*(bool*) source->value) ? 1 : 0); break;
                case qb::DataType::UINT8:
                    *value = std::to_string((*(uint8_t*) source->value)); break;
                case qb::DataType::INT8:
                    *value = std::to_string((*(int8_t*) source->value)); break;
                case qb::DataType::UINT16:
                    *value = std::to_string((*(uint16_t*) source->value)); break;
                case qb::DataType::INT16:
                    *value = std::to_string((*(int16_t*) source->value)); break;
                case qb::DataType::UINT32:
                    *value = std::to_string((*(uint32_t*) source->value)); break;
                case qb::DataType::INT32:
                    *value = std::to_string((*(int32_t*) source->value)); break;
                case qb::DataType::FLOAT32:
                    *value = std::to_string((*(float*) source->value)); break;
                default:
                    delete value;
                    return ERROR("Cast to STRING only allowed from ERROR, BOOL, UINT8, INT8, UINT16, INT16, UINT32, INT32, FLOAT32 or STRING.");
            }

            return {
                .data = new data_t({
                    .type = qb::DataType::STRING,
                    .value = value,
                    .heap = true
                })
            };
        }
        case qb::DataType::VECTOR: {
            switch (source->type) {
                case qb::DataType::VECTOR: {
                    auto target_vec = (qb::data::Vector<void>*) target->value;
                    auto source_vec = (qb::data::Vector<void>*) source->value;
                    if (!match_shape(target_vec, source_vec))
                        return ERROR("Cast to VECTOR only allowed from VECTOR with same shape.");
                    if (!match_item_type(target_vec, source_vec)) {
                        return ERROR("Cast to VECTOR only allowed from VECTOR with same item type.");
                    }
                    return { .data = source };
                }
                default:
                    return ERROR("Cast to VECTOR only allowed from VECTOR.");
            }
        }
        case qb::DataType::VECTOR_SLICE: {
            auto target_vs = (qb::data_slice_t*) target->value;
            switch (source->type) {
                case qb::DataType::VECTOR: {
                    auto source_vec = (qb::data::Vector<void>*) source->value;
                    if (!match_shape(target_vs, source_vec))
                        return ERROR("Cast to VECTOR_SLICE only allowed from VECTOR with same shape.");
                    if (!match_item_type(target_vs, source_vec))
                        return ERROR("Cast to VECTOR_SLICE only allowed from VECTOR with same item type.");
                    return {
                        .data = new data_t({
                            .type = qb::DataType::VECTOR_SLICE,
                            .value = new qb::data_slice_t({
                                .type = qb::DataType::VECTOR,
                                .value = source->value,
                                .heap = source->heap,
                                .slice = source_vec->full_slice()
                            }),
                            .heap = true
                        })
                    };
                }
                case qb::DataType::VECTOR_SLICE: {
                    auto source_vs = (qb::data_slice_t*) source->value;
                    if (!match_shape(target_vs, source_vs))
                        return ERROR("Cast to VECTOR_SLICE only allowed from VECTOR_SLICE with same shape.");
                    if (!match_item_type(target_vs, source_vs))
                        return ERROR("Cast to VECTOR_SLICE only allowed from VECTOR_SLICE with same item type.");
                    return {
                        .data = source
                    };
                }
                case qb::DataType::ERROR:
                case qb::DataType::BOOL:
                case qb::DataType::UINT8:
                case qb::DataType::INT8:
                case qb::DataType::UINT16:
                case qb::DataType::INT16:
                case qb::DataType::UINT32:
                case qb::DataType::INT32:
                case qb::DataType::FLOAT32:
                case qb::DataType::STRING:
                case qb::DataType::REF:
                    if (!match_item_type(target_vs, source->type))
                        return ERROR("Cast to VECTOR_SLICE only allowed from primitive of same type as the slice items.");
                    return {
                        .data = new data_t({
                            .type = qb::DataType::VECTOR_SLICE,
                            .value = new qb::data_slice_t({
                                .type = source->type,
                                .value = source->value,
                                .heap = source->heap,
                                .slice = target_vs->slice->copy()
                            }),
                            .heap = true
                        })
                    };
                default:
                    return ERROR("Cast to VECTOR_SLICE failed.");
            }
        }
        case qb::DataType::REF: {
            switch (source->type) {
                case qb::DataType::REF: {
                    return { .data = source };
                }
                default:
                    return ERROR("Cast to REF only allowed from REF.");
            }
        }
        default:
            return { .data = new data_t(UNRESOLVED_DATA) };
    }
}

qb::_operator::res_t qb::_operator::assign(qb::data_t* target, qb::data_t* source) {

    auto cast_res = cast(target, source);
    ASSERT_NO_ERROR(cast_res);

    switch (target->type) {
        case qb::DataType::_NULL: break;
        case qb::DataType::ERROR: *((qb::data::Error*) target->value) = *((qb::data::Error*) cast_res.data->value); break;
        case qb::DataType::BOOL: *((bool*) target->value) = *((bool*) cast_res.data->value); break;
        case qb::DataType::UINT8: *((uint8_t*) target->value) = *((uint8_t*) cast_res.data->value); break;
        case qb::DataType::INT8: *((int8_t*) target->value) = *((int8_t*) cast_res.data->value); break;
        case qb::DataType::UINT16: *((uint16_t*) target->value) = *((uint16_t*) cast_res.data->value); break;
        case qb::DataType::INT16: *((int16_t*) target->value) = *((int16_t*) cast_res.data->value); break;
        case qb::DataType::UINT32: *((uint32_t*) target->value) = *((uint32_t*) cast_res.data->value); break;
        case qb::DataType::INT32: *((int32_t*) target->value) = *((int32_t*) cast_res.data->value); break;
        case qb::DataType::FLOAT32: *((float*) target->value) = *((float*) cast_res.data->value); break;
        case qb::DataType::STRING: *((std::string*) target->value) = *((std::string*) cast_res.data->value); break;
        case qb::DataType::VECTOR: /* TODO */ break;
        case qb::DataType::REF: /* TODO */ break;
    }

    qb::_operator::clean_heap(&cast_res);
    return {
        .data = target
    };
}

qb::_operator::res_t qb::_operator::compare(qb::data_t* target, qb::data_t* source) {

    auto cast_res = cast(target, source);
    ASSERT_NO_ERROR(cast_res);

    int8_t diff = 0;

    switch (target->type) {
        case qb::DataType::_NULL: break;
        case qb::DataType::ERROR: return ERROR("Comparing ERRORs is not allowed.");
        case qb::DataType::BOOL: {
            bool l = *((bool*) target->value);
            bool r = *((bool*) cast_res.data->value);
            if (l != r) diff = r ? 1 : -1;
            break;
        }
        case qb::DataType::UINT8: {
            uint8_t l = *((uint8_t*) target->value);
            uint8_t r = *((uint8_t*) cast_res.data->value);
            if (r > l) diff = 1;
            else if (l > r) diff = -1;
            break;
        }
        case qb::DataType::INT8: {
            int8_t l = *((int8_t*) target->value);
            int8_t r = *((int8_t*) cast_res.data->value);
            if (r > l) diff = 1;
            else if (l > r) diff = -1;
            break;
        }
        case qb::DataType::UINT16: {
            uint16_t l = *((uint16_t*) target->value);
            uint16_t r = *((uint16_t*) cast_res.data->value);
            if (r > l) diff = 1;
            else if (l > r) diff = -1;
            break;
        }
        case qb::DataType::INT16: {
            int16_t l = *((int16_t*) target->value);
            int16_t r = *((int16_t*) cast_res.data->value);
            if (r > l) diff = 1;
            else if (l > r) diff = -1;
            break;
        }
        case qb::DataType::UINT32: {
            uint32_t l = *((uint32_t*) target->value);
            uint32_t r = *((uint32_t*) cast_res.data->value);
            if (r > l) diff = 1;
            else if (l > r) diff = -1;
            break;
        }
        case qb::DataType::INT32: {
            int32_t l = *((int32_t*) target->value);
            int32_t r = *((int32_t*) cast_res.data->value);
            if (r > l) diff = 1;
            else if (l > r) diff = -1;
            break;
        }
        case qb::DataType::FLOAT32: {
            float l = *((float*) target->value);
            float r = *((float*) cast_res.data->value);
            if (r > l) diff = 1;
            else if (l > r) diff = -1;
            break;
        }
        case qb::DataType::STRING: {
            std::string l = *((std::string*) target->value);
            std::string r = *((std::string*) cast_res.data->value);
            if (l != r) diff = -1;
            break;
        }
        case qb::DataType::VECTOR: return ERROR("Comparing VECTORs is not allowed.");
        case qb::DataType::REF: {
            auto l = (qb::data::Reference*) target->value;
            auto r = (qb::data::Reference*) cast_res.data->value;
            diff = (l->device == r->device)
                && (l->port == r->port)
                && (l->flags == r->flags)
                && (
                    (l->slice == nullptr && r->slice == nullptr)
                    || (
                        (l->slice != nullptr && r->slice != nullptr)
                        && (l->slice->dims == r->slice->dims)
                        && (*(l->slice->start) == *(r->slice->start))
                        && (*(l->slice->end) == *(r->slice->end))
                    )
                );
            break;
        }
    }

    qb::_operator::clean_heap(&cast_res);
    return {
        .data = new qb::data_t({
            .type = qb::DataType::UINT8,
            .value = (void*) new uint8_t(diff),
            .heap = true
        })
    };
}

qb::_operator::res_t qb::_operator::arithmetic_bool(qb::InstructionType type, qb::data_t* target, qb::data_t* source) {

    bool v_target = false;
    if (type != qb::InstructionType::NOT) {
        auto cast_target_res = cast(&qb::BOOL_TARGET_TYPE, target);
        ASSERT_NO_ERROR(cast_target_res);
        v_target = *(bool*) cast_target_res.data->value;
        qb::_operator::clean_heap(&cast_target_res);
    }

    auto cast_source_res = cast(&qb::BOOL_TARGET_TYPE, source);
    ASSERT_NO_ERROR(cast_source_res);
    bool v_source = *(bool*) cast_source_res.data->value;
    qb::_operator::clean_heap(&cast_source_res);

    bool out = false;
    switch (type) {
        case qb::InstructionType::NOT:
            out = !v_source;
            break;
        case qb::InstructionType::AND:
            out = v_target && v_source;
            break;
        case qb::InstructionType::OR:
            out = v_target || v_source;
            break;
    }
    
    qb::_operator::assign(target, new data_t({
        .type = qb::DataType::BOOL,
        .value = new bool(out),
        .heap = true
    }));

    return {
        .data = target
    };
}

qb::_operator::res_t qb::_operator::arithmetic(qb::InstructionType type, qb::data_t* target, qb::data_t* source) {

    auto cast_res = cast(target, source);
    ASSERT_NO_ERROR(cast_res);
    
    switch (type) {
        case qb::InstructionType::ADD: {
            switch (target->type) {
                case qb::DataType::_NULL: return ERROR("Arithmetics with NULL not allowed.");
                case qb::DataType::ERROR: return ERROR("Arithmetics with ERROR not allowed.");
                case qb::DataType::BOOL: *((bool*) target->value) += *((bool*) cast_res.data->value); break;
                case qb::DataType::UINT8: *((uint8_t*) target->value) += *((uint8_t*) cast_res.data->value); break;
                case qb::DataType::INT8: *((int8_t*) target->value) += *((int8_t*) cast_res.data->value); break;
                case qb::DataType::UINT16: *((uint16_t*) target->value) += *((uint16_t*) cast_res.data->value); break;
                case qb::DataType::INT16: *((int16_t*) target->value) += *((int16_t*) cast_res.data->value); break;
                case qb::DataType::UINT32: *((uint32_t*) target->value) += *((uint32_t*) cast_res.data->value); break;
                case qb::DataType::INT32: *((int32_t*) target->value) += *((int32_t*) cast_res.data->value); break;
                case qb::DataType::FLOAT32: *((float*) target->value) += *((float*) cast_res.data->value); break;
                case qb::DataType::STRING: *((std::string*) target->value) += *((std::string*) cast_res.data->value); break;
                case qb::DataType::VECTOR: /* TODO */ break;
                case qb::DataType::REF: /* TODO */ break;
            }
            break;
        }            
        case qb::InstructionType::SUB: {
            switch (target->type) {
                case qb::DataType::_NULL: return ERROR("Arithmetics with NULL not allowed.");
                case qb::DataType::ERROR: return ERROR("Arithmetics with ERROR not allowed.");
                case qb::DataType::BOOL: *((bool*) target->value) -= *((bool*) cast_res.data->value); break;
                case qb::DataType::UINT8: *((uint8_t*) target->value) -= *((uint8_t*) cast_res.data->value); break;
                case qb::DataType::INT8: *((int8_t*) target->value) -= *((int8_t*) cast_res.data->value); break;
                case qb::DataType::UINT16: *((uint16_t*) target->value) -= *((uint16_t*) cast_res.data->value); break;
                case qb::DataType::INT16: *((int16_t*) target->value) -= *((int16_t*) cast_res.data->value); break;
                case qb::DataType::UINT32: *((uint32_t*) target->value) -= *((uint32_t*) cast_res.data->value); break;
                case qb::DataType::INT32: *((int32_t*) target->value) -= *((int32_t*) cast_res.data->value); break;
                case qb::DataType::FLOAT32: *((float*) target->value) -= *((float*) cast_res.data->value); break;
                case qb::DataType::STRING: return ERROR("Subtraction with STRING not allowed.");
                case qb::DataType::VECTOR: /* TODO */ break;
                case qb::DataType::REF: /* TODO */ break;
            }
            break;
        }            
        case qb::InstructionType::MULT: {
            switch (target->type) {
                case qb::DataType::_NULL: return ERROR("Arithmetics with NULL not allowed.");
                case qb::DataType::ERROR: return ERROR("Arithmetics with ERROR not allowed.");
                case qb::DataType::BOOL: *((bool*) target->value) *= *((bool*) cast_res.data->value); break;
                case qb::DataType::UINT8: *((uint8_t*) target->value) *= *((uint8_t*) cast_res.data->value); break;
                case qb::DataType::INT8: *((int8_t*) target->value) *= *((int8_t*) cast_res.data->value); break;
                case qb::DataType::UINT16: *((uint16_t*) target->value) *= *((uint16_t*) cast_res.data->value); break;
                case qb::DataType::INT16: *((int16_t*) target->value) *= *((int16_t*) cast_res.data->value); break;
                case qb::DataType::UINT32: *((uint32_t*) target->value) *= *((uint32_t*) cast_res.data->value); break;
                case qb::DataType::INT32: *((int32_t*) target->value) *= *((int32_t*) cast_res.data->value); break;
                case qb::DataType::FLOAT32: *((float*) target->value) *= *((float*) cast_res.data->value); break;
                case qb::DataType::STRING: return ERROR("Multiplication with STRING not allowed.");
                case qb::DataType::VECTOR: /* TODO */ break;
                case qb::DataType::REF: /* TODO */ break;
            }
            break;
        }            
        case qb::InstructionType::DIV: {
            switch (target->type) {
                case qb::DataType::_NULL: return ERROR("Arithmetics with NULL not allowed.");
                case qb::DataType::ERROR: return ERROR("Arithmetics with ERROR not allowed.");
                case qb::DataType::BOOL: *((bool*) target->value) /= *((bool*) cast_res.data->value); break;
                case qb::DataType::UINT8: *((uint8_t*) target->value) /= *((uint8_t*) cast_res.data->value); break;
                case qb::DataType::INT8: *((int8_t*) target->value) /= *((int8_t*) cast_res.data->value); break;
                case qb::DataType::UINT16: *((uint16_t*) target->value) /= *((uint16_t*) cast_res.data->value); break;
                case qb::DataType::INT16: *((int16_t*) target->value) /= *((int16_t*) cast_res.data->value); break;
                case qb::DataType::UINT32: *((uint32_t*) target->value) /= *((uint32_t*) cast_res.data->value); break;
                case qb::DataType::INT32: *((int32_t*) target->value) /= *((int32_t*) cast_res.data->value); break;
                case qb::DataType::FLOAT32: *((float*) target->value) /= *((float*) cast_res.data->value); break;
                case qb::DataType::STRING: return ERROR("Division with STRING not allowed.");
                case qb::DataType::VECTOR: /* TODO */ break;
                case qb::DataType::REF: /* TODO */ break;
            }
            break;
        }            
        case qb::InstructionType::MOD: {
            switch (target->type) {
                case qb::DataType::_NULL: return ERROR("Arithmetics with NULL not allowed.");
                case qb::DataType::ERROR: return ERROR("Arithmetics with ERROR not allowed.");
                case qb::DataType::BOOL: *((bool*) target->value) %= *((bool*) cast_res.data->value); break;
                case qb::DataType::UINT8: *((uint8_t*) target->value) %= *((uint8_t*) cast_res.data->value); break;
                case qb::DataType::INT8: *((int8_t*) target->value) %= *((int8_t*) cast_res.data->value); break;
                case qb::DataType::UINT16: *((uint16_t*) target->value) %= *((uint16_t*) cast_res.data->value); break;
                case qb::DataType::INT16: *((int16_t*) target->value) %= *((int16_t*) cast_res.data->value); break;
                case qb::DataType::UINT32: *((uint32_t*) target->value) %= *((uint32_t*) cast_res.data->value); break;
                case qb::DataType::INT32: *((int32_t*) target->value) %= *((int32_t*) cast_res.data->value); break;
                case qb::DataType::FLOAT32: *((float*) target->value) = std::fmod(*((float*) target->value), *((float*) cast_res.data->value)); break;
                case qb::DataType::STRING: return ERROR("Modulo with STRING not allowed.");
                case qb::DataType::VECTOR: /* TODO */ break;
                case qb::DataType::REF: /* TODO */ break;
            }
            break;
        }            
        case qb::InstructionType::POW: {
            switch (target->type) {
                case qb::DataType::_NULL: return ERROR("Arithmetics with NULL not allowed.");
                case qb::DataType::ERROR: return ERROR("Arithmetics with ERROR not allowed.");
                case qb::DataType::BOOL: *((bool*) target->value) = std::pow(*((bool*) target->value), *((bool*) cast_res.data->value)); break;
                case qb::DataType::UINT8: *((uint8_t*) target->value) = std::pow(*((uint8_t*) target->value), *((uint8_t*) cast_res.data->value)); break;
                case qb::DataType::INT8: *((int8_t*) target->value) = std::pow(*((int8_t*) target->value), *((int8_t*) cast_res.data->value)); break;
                case qb::DataType::UINT16: *((uint16_t*) target->value) = std::pow(*((uint16_t*) target->value), *((uint16_t*) cast_res.data->value)); break;
                case qb::DataType::INT16: *((int16_t*) target->value) = std::pow(*((int16_t*) target->value), *((int16_t*) cast_res.data->value)); break;
                case qb::DataType::UINT32: *((uint32_t*) target->value) = std::pow(*((uint32_t*) target->value), *((uint32_t*) cast_res.data->value)); break;
                case qb::DataType::INT32: *((int32_t*) target->value) = std::pow(*((int32_t*) target->value), *((int32_t*) cast_res.data->value)); break;
                case qb::DataType::FLOAT32: *((float*) target->value) = std::pow(*((float*) target->value), *((float*) cast_res.data->value)); break;
                case qb::DataType::STRING: return ERROR("Exponential with STRING not allowed.");
                case qb::DataType::VECTOR: /* TODO */ break;
                case qb::DataType::REF: /* TODO */ break;
            }
            break;
        }            
    }

    qb::_operator::clean_heap(&cast_res);
    return {
        .data = target
    };
}