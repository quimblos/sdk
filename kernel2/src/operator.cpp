#include "operator.h"

#define ASSERT_NO_ERROR(X) if (X.error != nullptr) return X;

void qb::_operator::clean_heap(qb::_operator::res_t* res) {
    if (res->data != nullptr) {
        if (res->data->heap) {
            qb::_operator::delete_data(res->data);
        }
    }
    if (res->error != nullptr) {
        delete res->error;
    }
}

void qb::_operator::delete_data(qb::data_t* data) {
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
        case qb::DataType::ARRAY: /* TODO */ break;
        case qb::DataType::REF: /* TODO */ break;
    }
    delete data;
}

qb::_operator::res_t qb::_operator::cast(qb::type_t type, qb::data_t* source) {

    switch (type) {
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
        case qb::DataType::ARRAY: {
            return ERROR("Cast to ARRAY not allowed.");
        }
        case qb::DataType::REF: {
            return ERROR("Cast to REF not allowed.");
        }
        default:
            return { .data = new data_t(UNRESOLVED) };
    }
}

qb::_operator::res_t qb::_operator::assign(qb::data_t* target, qb::data_t* source) {

    auto cast_res = cast(target->type, source);
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
        case qb::DataType::ARRAY: /* TODO */ break;
        case qb::DataType::REF: /* TODO */ break;
    }

    qb::_operator::clean_heap(&cast_res);
    return {
        .data = target
    };
}

qb::_operator::res_t qb::_operator::compare(qb::data_t* target, qb::data_t* source) {

    auto cast_res = cast(target->type, source);
    ASSERT_NO_ERROR(cast_res);

    uint8_t diff = 0;

    switch (target->type) {
        case qb::DataType::_NULL: break;
        case qb::DataType::ERROR: return ERROR("Comparing ERRORs is not allowed.");
        case qb::DataType::BOOL: diff = (*((bool*) cast_res.data->value) ? 1 : 0) - (*((bool*) target->value) ? 1 : 0); break;
        case qb::DataType::UINT8: diff = *((uint8_t*) cast_res.data->value) - *((uint8_t*) target->value); break;
        case qb::DataType::INT8: diff = *((int8_t*) cast_res.data->value) - *((int8_t*) target->value); break;
        case qb::DataType::UINT16: diff = *((uint16_t*) cast_res.data->value) - *((uint16_t*) target->value); break;
        case qb::DataType::INT16: diff = *((int16_t*) cast_res.data->value) - *((int16_t*) target->value); break;
        case qb::DataType::UINT32: diff = *((uint32_t*) cast_res.data->value) - *((uint32_t*) target->value); break;
        case qb::DataType::INT32: diff = *((int32_t*) cast_res.data->value) - *((int32_t*) target->value); break;
        case qb::DataType::FLOAT32: diff = *((float*) cast_res.data->value) - *((float*) target->value); break;
        case qb::DataType::STRING:
            diff = (*((std::string*) target->value) == *((std::string*) cast_res.data->value)) ? 0 : -1;
            break;
        case qb::DataType::ARRAY: return ERROR("Comparing ARRAYs is not allowed.");
        case qb::DataType::REF: {
            auto ref_source = (qb::data::Reference*) cast_res.data->value;
            auto ref_target = (qb::data::Reference*) target->value;
            diff = (ref_source->deref == ref_target->deref) \
                && (ref_source->device == ref_target->device) \
                && (ref_source->port == ref_target->port) \
                && (ref_source->index == ref_target->index);
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