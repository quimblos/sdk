#include "operator.h"
#include <iostream>
#include <cmath>

#define QB_DEBUG_OPERATOR

qb::_operator::res_t qb::_operator::as_pointer(qb::Node* source) {
    switch (source->type) {
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_INVALID_CAST_VOID_TO_PTR };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_INVALID_CAST_ERROR_TO_PTR };
        case qb::Type::PTR:
            return { .code = QB_OPERATOR_R_OK, .value = ((qb::node::Pointer*) source) };
        case qb::Type::BOOL:
        case qb::Type::UINT8:
        case qb::Type::INT8:
        case qb::Type::UINT16:
        case qb::Type::INT16:
        case qb::Type::UINT32:
        case qb::Type::INT32:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Numeric<int32_t>*) source)->data };
        case qb::Type::FLOAT32:
            return { .code = QB_OPERATOR_R_INVALID_CAST_FLOAT_TO_PTR };
        case qb::Type::STRING:
            return { .code = QB_OPERATOR_R_INVALID_CAST_STRING_TO_PTR };
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_INVALID_CAST_ARRAY_TO_PTR };
    }
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::as_numeric(qb::Node* source) {
    switch (source->type) {
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_INVALID_CAST_VOID_TO_NUMERIC };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_INVALID_CAST_ERROR_TO_NUMERIC };
        case qb::Type::PTR:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Pointer*) source)->index };
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Numeric<bool>*) source)->data };
        case qb::Type::UINT8:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Numeric<uint8_t>*) source)->data };
        case qb::Type::INT8:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Numeric<int8_t>*) source)->data };
        case qb::Type::UINT16:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Numeric<uint16_t>*) source)->data };
        case qb::Type::INT16:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Numeric<int16_t>*) source)->data };
        case qb::Type::UINT32:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Numeric<uint32_t>*) source)->data };
        case qb::Type::INT32:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Numeric<int32_t>*) source)->data };
        case qb::Type::FLOAT32:
            return { .code = QB_OPERATOR_R_OK, .value = &((qb::node::Numeric<float>*) source)->data };
        case qb::Type::STRING:
            return { .code = QB_OPERATOR_R_INVALID_CAST_STRING_TO_NUMERIC };
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_INVALID_CAST_ARRAY_TO_NUMERIC };
    }
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::as_string(qb::Node* source) {
    switch (source->type) {
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_INVALID_CAST_VOID_TO_STRING };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_OK, .value = &(((qb::node::Error*) source)->data) };
        case qb::Type::PTR:
            return { .code = QB_OPERATOR_R_INVALID_CAST_PTR_TO_STRING };
        case qb::Type::BOOL:
        case qb::Type::UINT8:
        case qb::Type::INT8:
        case qb::Type::UINT16:
        case qb::Type::INT16:
        case qb::Type::UINT32:
        case qb::Type::INT32:
        case qb::Type::FLOAT32:
            return { .code = QB_OPERATOR_R_INVALID_CAST_NUMERIC_TO_STRING };
        case qb::Type::STRING:
            return { .code = QB_OPERATOR_R_OK, .value = &(((qb::node::String*) source)->data) };
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_INVALID_CAST_ARRAY_TO_STRING };
    }
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

// Cast/Copy

qb::_operator::cast_res_t qb::_operator::cast_to(qb::Node* target, qb::Node* source, index_t index) {

    // Step 1: Find target type

    bool is_array_item;
    type_t type;

    // Resolve target value type
    if (type != qb::Type::ARRAY) {
        // Index > 0 only allowed for arrays
        
        if (index > 0) return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_NOT_INDEXABLE };

        is_array_item = false;
        type = target->type;
    }
    // Resolve target array item type
    else {
        auto array = (qb::node::Array<void*>*) target;
        
        // Out of bounds
        if (index >= array->length) return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_OUT_OF_BOUNDS };

        is_array_item = true;
        type = array->item_type;
    }

    // Step 2: Cast according to type

    void* value = nullptr;
    switch (type) {
        case qb::Type::VOID:
            break;
        case qb::Type::ERROR:
        {
            auto res = qb::_operator::as_string(source);
            if (res.code > 0) return { .code = res.code };
            value = res.value;
            break;
        }
        case qb::Type::PTR:
        {
            auto res = qb::_operator::as_pointer(source);
            if (res.code > 0) return { .code = res.code };
            value = res.value;
            break;
        }
        case qb::Type::BOOL:
        case qb::Type::UINT8:
        case qb::Type::INT8:
        case qb::Type::UINT16:
        case qb::Type::INT16:
        case qb::Type::UINT32:
        case qb::Type::INT32:
        case qb::Type::FLOAT32:
        {
            auto res = qb::_operator::as_numeric(source);
            if (res.code > 0) return { .code = res.code };
            value = res.value;
            break;
        }
        case qb::Type::STRING:
        {
            auto res = qb::_operator::as_string(source);
            if (res.code > 0) return { .code = res.code };
            value = res.value;
            break;
        }
        // Arrays can't be resolved directly
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_INVALID_CAST_TO_ARRAY };
    }

    // Unknown type
    if (value == nullptr) {
        return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
    }

    // Step 3: Get pointer to target for manipulation

    void* ptr = nullptr;
    if (!is_array_item) {
        switch (type) {
            case qb::Type::VOID: break;
            case qb::Type::ERROR: ptr = &((qb::node::Error*) target)->data; break;
            case qb::Type::PTR: ptr = nullptr; break;
            case qb::Type::BOOL: ptr = &((qb::node::Numeric<bool>*) target)->data; break;
            case qb::Type::UINT8: ptr = &((qb::node::Numeric<uint8_t>*) target)->data; break;
            case qb::Type::INT8: ptr = &((qb::node::Numeric<int8_t>*) target)->data; break;
            case qb::Type::UINT16: ptr = &((qb::node::Numeric<uint16_t>*) target)->data; break;
            case qb::Type::INT16: ptr = &((qb::node::Numeric<uint16_t>*) target)->data; break;
            case qb::Type::UINT32: ptr = &((qb::node::Numeric<uint32_t>*) target)->data; break;
            case qb::Type::INT32: ptr = &((qb::node::Numeric<uint32_t>*) target)->data; break;
            case qb::Type::FLOAT32: ptr = &((qb::node::Numeric<float>*) target)->data; break;
            case qb::Type::STRING:  ptr = &((qb::node::String*) target)->data; break;
            case qb::Type::ARRAY: break;
        }
    }
    else {
        switch (type) {
            case qb::Type::VOID: break;
            case qb::Type::ERROR: break;
            case qb::Type::PTR: break;
            case qb::Type::BOOL: ptr = ((qb::node::Array<bool>*) target)->items+index; break;
            case qb::Type::UINT8: ptr = ((qb::node::Array<uint8_t>*) target)->items+index; break;
            case qb::Type::INT8: ptr = ((qb::node::Array<int8_t>*) target)->items+index; break;
            case qb::Type::UINT16: ptr = ((qb::node::Array<uint16_t>*) target)->items+index; break;
            case qb::Type::INT16: ptr = ((qb::node::Array<uint16_t>*) target)->items+index; break;
            case qb::Type::UINT32: ptr = ((qb::node::Array<uint32_t>*) target)->items+index; break;
            case qb::Type::INT32: ptr = ((qb::node::Array<uint32_t>*) target)->items+index; break;
            case qb::Type::FLOAT32: ptr = ((qb::node::Array<float>*) target)->items+index; break;
            case qb::Type::STRING: ptr = ((qb::node::Array<std::string>*) target)->items+index; break;
            case qb::Type::ARRAY: break;
        }
    }

    return {
        .code = QB_OPERATOR_R_OK,
        .type = type,
        .value = value,
        .target = ptr
    };
}

qb::_operator::res_t qb::_operator::copy_to(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Assigning to void doesn't make sense, it's always void
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_VALUE_VOID };

        case qb::Type::ERROR:
            *((std::string*) cast.target) = std::string(*(std::string*) cast.value);
            return { .code = QB_OPERATOR_R_OK };
        
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                auto ref = (qb::node::Pointer*) cast.value;
                ((qb::node::Pointer*) target)->device = ref->device;
                ((qb::node::Pointer*) target)->port = ref->port;
                ((qb::node::Pointer*) target)->index = ref->index;
                // ((qb::node::Pointer*) target)->data = ref->data;
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                ((qb::node::Pointer*) target)->index = *(index_t*) cast.value;
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            *((bool*) cast.target) = *(bool*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT8:
            *((uint8_t*) cast.target) = *(uint8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT8:
            *((int8_t*) cast.target) = *(int8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT16:
            *((uint16_t*) cast.target) = *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT16:
            *((uint16_t*) cast.target) = *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT32:
            *((uint32_t*) cast.target) = *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT32:
            *((uint32_t*) cast.target) = *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::FLOAT32:
            *((float*) cast.target) = *(float*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::STRING: 
            *((std::string*) cast.target) = std::string(*(std::string*) cast.value);
            return { .code = QB_OPERATOR_R_OK };

        // Array assignment not supported, assign each item instead
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

// Math

qb::_operator::res_t qb::_operator::add_to(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Math with void/error doesn't make sense
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR };
        //
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                // Math with pointer as source is ambiguous, not implemented
                return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                ((qb::node::Pointer*) target)->index += *(index_t*) cast.value;
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
        case qb::Type::UINT8:
            *((uint8_t*) cast.target) += *(uint8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT8:
            *((int8_t*) cast.target) += *(int8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT16:
            *((uint16_t*) cast.target) += *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT16:
            *((uint16_t*) cast.target) += *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT32:
            *((uint32_t*) cast.target) += *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT32:
            *((uint32_t*) cast.target) += *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::FLOAT32:
            *((float*) cast.target) += *(float*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        
        // Special case of String append
        case qb::Type::STRING: 
            *((std::string*) cast.target) += std::string(*(std::string*) cast.value);
            return { .code = QB_OPERATOR_R_OK };

        // Array math not supported, operate on each item instead
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::sub_to(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Math with void/error doesn't make sense
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR };
        //
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                // Math with pointer as source is ambiguous, not implemented
                return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                ((qb::node::Pointer*) target)->index -= *(index_t*) cast.value;
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
        case qb::Type::UINT8:
            *((uint8_t*) cast.target) -= *(uint8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT8:
            *((int8_t*) cast.target) -= *(int8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT16:
            *((uint16_t*) cast.target) -= *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT16:
            *((uint16_t*) cast.target) -= *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT32:
            *((uint32_t*) cast.target) -= *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT32:
            *((uint32_t*) cast.target) -= *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::FLOAT32:
            *((float*) cast.target) -= *(float*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        // String math not supported, operate on each item instead
        case qb::Type::STRING: 
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_STRING };
        // Array math not supported, operate on each item instead
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::mult_to(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Math with void/error doesn't make sense
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR };
        //
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                // Math with pointer as source is ambiguous, not implemented
                return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                ((qb::node::Pointer*) target)->index *= *(index_t*) cast.value;
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
        case qb::Type::UINT8:
            *((uint8_t*) cast.target) *= *(uint8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT8:
            *((int8_t*) cast.target) *= *(int8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT16:
            *((uint16_t*) cast.target) *= *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT16:
            *((uint16_t*) cast.target) *= *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT32:
            *((uint32_t*) cast.target) *= *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT32:
            *((uint32_t*) cast.target) *= *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::FLOAT32:
            *((float*) cast.target) *= *(float*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        // String math not supported, operate on each item instead
        case qb::Type::STRING: 
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_STRING };
        // Array math not supported, operate on each item instead
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::div_to(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Math with void/error doesn't make sense
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR };
        //
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                // Math with pointer as source is ambiguous, not implemented
                return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                ((qb::node::Pointer*) target)->index /= *(index_t*) cast.value;
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
        case qb::Type::UINT8:
            *((uint8_t*) cast.target) /= *(uint8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT8:
            *((int8_t*) cast.target) /= *(int8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT16:
            *((uint16_t*) cast.target) /= *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT16:
            *((uint16_t*) cast.target) /= *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT32:
            *((uint32_t*) cast.target) /= *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT32:
            *((uint32_t*) cast.target) /= *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::FLOAT32:
            *((float*) cast.target) /= *(float*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        // String math not supported, operate on each item instead
        case qb::Type::STRING: 
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_STRING };
        // Array math not supported, operate on each item instead
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::mod_to(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Math with void/error doesn't make sense
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR };
        //
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                // Math with pointer as source is ambiguous, not implemented
                return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                ((qb::node::Pointer*) target)->index %= *(index_t*) cast.value;
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
        case qb::Type::UINT8:
            *((uint8_t*) cast.target) %= *(uint8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT8:
            *((int8_t*) cast.target) %= *(int8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT16:
            *((uint16_t*) cast.target) %= *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT16:
            *((uint16_t*) cast.target) %= *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT32:
            *((uint32_t*) cast.target) %= *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT32:
            *((uint32_t*) cast.target) %= *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::FLOAT32:
            *((float*) cast.target) = std::fmod(*((float*) cast.target), *(float*) cast.value);
            return { .code = QB_OPERATOR_R_OK };
        // String math not supported, operate on each item instead
        case qb::Type::STRING: 
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_STRING };
        // Array math not supported, operate on each item instead
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::pow_to(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Math with void/error doesn't make sense
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR };
        //
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                // Math with pointer as source is ambiguous, not implemented
                return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                ((qb::node::Pointer*) target)->index = std::pow(((qb::node::Pointer*) target)->index, *(index_t*) cast.value);
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
        case qb::Type::UINT8:
            *((uint8_t*) cast.target) = std::pow(*((uint8_t*) cast.target), *(uint8_t*) cast.value);
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT8:
            *((int8_t*) cast.target) = std::pow(*((int8_t*) cast.target), *(int8_t*) cast.value);
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT16:
            *((uint16_t*) cast.target) = std::pow(*((uint16_t*) cast.target), *(uint16_t*) cast.value);
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT16:
            *((uint16_t*) cast.target) = std::pow(*((uint16_t*) cast.target), *(uint16_t*) cast.value);
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::UINT32:
            *((uint32_t*) cast.target) = std::pow(*((uint32_t*) cast.target), *(uint32_t*) cast.value);
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::INT32:
            *((uint32_t*) cast.target) = std::pow(*((uint32_t*) cast.target), *(uint32_t*) cast.value);
            return { .code = QB_OPERATOR_R_OK };
        case qb::Type::FLOAT32:
            *((float*) cast.target) = std::pow(*((float*) cast.target), *(float*) cast.value);
            return { .code = QB_OPERATOR_R_OK };
        // String math not supported, operate on each item instead
        case qb::Type::STRING: 
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_STRING };
        // Array math not supported, operate on each item instead
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::is_equal_to(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Compare with void/error doesn't make sense
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR };
        //
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                auto ref = (qb::node::Pointer*) cast.value;
                bool match = (
                    ((qb::node::Pointer*) target)->device == ref->device
                    && ((qb::node::Pointer*) target)->port == ref->port
                    && ((qb::node::Pointer*) target)->index == ref->index
                );
                // ((qb::node::Pointer*) target)->data = ref->data;
                return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                bool match = ((qb::node::Pointer*) target)->index == *(index_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
        case qb::Type::UINT8:
        {
            bool match = *((uint8_t*) cast.target) == *(uint8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::INT8:
        {
            bool match = *((int8_t*) cast.target) == *(int8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::UINT16:
        {
            bool match = *((uint16_t*) cast.target) == *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::INT16:
        {
            bool match = *((uint16_t*) cast.target) == *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::UINT32:
        {
            bool match = *((uint32_t*) cast.target) == *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::INT32:
        {
            bool match = *((uint32_t*) cast.target) == *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::FLOAT32:
        {
            bool match = *((float*) cast.target) == *(float*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::STRING:
        {
            auto comp = ((std::string*) cast.target)->compare(*(std::string*) cast.value);
            return { .code = QB_OPERATOR_R_OK, .value = (void*) (comp == 0) };
        }
        // Array comparison is ambiguous or costly, not implemented
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::is_less_than(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Compare with void/error doesn't make sense
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR };
        //
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_PTR };
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                bool match = ((qb::node::Pointer*) target)->index < *(index_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
        case qb::Type::UINT8:
        {
            bool match = *((uint8_t*) cast.target) < *(uint8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::INT8:
        {
            bool match = *((int8_t*) cast.target) < *(int8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::UINT16:
        {
            bool match = *((uint16_t*) cast.target) < *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::INT16:
        {
            bool match = *((uint16_t*) cast.target) < *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::UINT32:
        {
            bool match = *((uint32_t*) cast.target) < *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::INT32:
        {
            bool match = *((uint32_t*) cast.target) < *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::FLOAT32:
        {
            bool match = *((float*) cast.target) < *(float*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        // String comparison is ambiguous, not implemented
        case qb::Type::STRING:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_STRING };
        // Array comparison is ambiguous or costly, not implemented
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::is_greater_than(qb::Node* target, qb::Node* source, index_t index) {
    
    // Cast to target type
    auto cast = qb::_operator::cast_to(target, source, index);
    if (cast.code > 0) return { .code = cast.code };
    
    // Value
    switch (cast.type) {
        // Compare with void/error doesn't make sense
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_VOID };
        case qb::Type::ERROR:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ERROR };
        //
        case qb::Type::PTR:
        {
            if (source->type == Type::PTR) {
                return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_PTR };
            }
            else {
                // Guaranteed to be an integer, by `as_pointer`;
                bool match = ((qb::node::Pointer*) target)->index > *(index_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
            }
            return { .code = QB_OPERATOR_R_OK };
        }
        case qb::Type::BOOL:
            return { .code = QB_OPERATOR_R_MATH_FAILED_SOURCE_VALUE_PTR };
        case qb::Type::UINT8:
        {
            bool match = *((uint8_t*) cast.target) > *(uint8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::INT8:
        {
            bool match = *((int8_t*) cast.target) > *(int8_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::UINT16:
        {
            bool match = *((uint16_t*) cast.target) > *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::INT16:
        {
            bool match = *((uint16_t*) cast.target) > *(uint16_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::UINT32:
        {
            bool match = *((uint32_t*) cast.target) > *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::INT32:
        {
            bool match = *((uint32_t*) cast.target) > *(uint32_t*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        case qb::Type::FLOAT32:
        {
            bool match = *((float*) cast.target) > *(float*) cast.value;
            return { .code = QB_OPERATOR_R_OK, .value = (void*) match };
        }
        // String comparison is ambiguous, not implemented
        case qb::Type::STRING:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_STRING };
        // Array comparison is ambiguous or costly, not implemented
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_MATH_FAILED_TARGET_VALUE_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}
