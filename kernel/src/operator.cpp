#include "operator.h"
#include <iostream>

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
        case qb::Type::FLOAT32:
            return { .code = QB_OPERATOR_R_INVALID_CAST_NUMERIC_TO_PTR };
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
            return { .code = QB_OPERATOR_R_INVALID_CAST_PTR_TO_NUMERIC };
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

qb::_operator::res_t qb::_operator::cast_to(type_t type, qb::Node* source) {
    switch (type) {
        case qb::Type::VOID:
            return { .code = QB_OPERATOR_R_OK, .value = nullptr };
        case qb::Type::ERROR:
            return qb::_operator::as_string(source);
        case qb::Type::PTR:
            return qb::_operator::as_pointer(source);
        case qb::Type::BOOL:
        case qb::Type::UINT8:
        case qb::Type::INT8:
        case qb::Type::UINT16:
        case qb::Type::INT16:
        case qb::Type::UINT32:
        case qb::Type::INT32:
        case qb::Type::FLOAT32:
            return qb::_operator::as_numeric(source);
        case qb::Type::STRING:
            return qb::_operator::as_string(source);
        // Arrays can't be resolved directly
        case qb::Type::ARRAY:
            return { .code = QB_OPERATOR_R_INVALID_CAST_TO_ARRAY };
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

qb::_operator::res_t qb::_operator::copy_to(qb::Node* target, qb::Node* source, index_t index) {
    
    bool is_array;
    type_t type;

    // Resolve target value type
    if (target->type != qb::Type::ARRAY) {
        // Index > 0 only allowed for arrays
        
        if (index > 0) return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_NOT_INDEXABLE };

        is_array = false;
        type = target->type;
    }
    // Resolve target array item type
    else {
        auto array = (qb::node::Array<void*>*) target;
        
        // Out of bounds
        if (index >= array->length) return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_OUT_OF_BOUNDS };

        is_array = true;
        type = array->item_type;
    }

    // Cast to target type
    auto cast = qb::_operator::cast_to(type, source);
    if (cast.code > 0) return cast;
    
    // Value
    if (!is_array) {
        switch (type) {
            // Assigning to void doesn't make sense, it's always void
            case qb::Type::VOID:
                return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_VALUE_VOID };

            case qb::Type::ERROR:
                ((qb::node::Error*) target)->data = std::string(*(std::string*) cast.value);
                return { .code = QB_OPERATOR_R_OK };
            
            case qb::Type::PTR:
            {
                // Guaranteed to be a pointer, by 'as_pointer'
                auto ref = (qb::node::Pointer*) cast.value;
                ((qb::node::Pointer*) target)->device = ref->device;
                ((qb::node::Pointer*) target)->port = ref->port;
                ((qb::node::Pointer*) target)->index = ref->index;
                ((qb::node::Pointer*) target)->data = ref->data;
                return { .code = QB_OPERATOR_R_OK };
            }
            case qb::Type::BOOL:
                ((qb::node::Numeric<bool>*) target)->data = *(bool*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::UINT8:
                ((qb::node::Numeric<uint8_t>*) target)->data = *(uint8_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::INT8:
                ((qb::node::Numeric<int8_t>*) target)->data = *(int8_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::UINT16:
                ((qb::node::Numeric<uint16_t>*) target)->data = *(uint16_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::INT16:
                ((qb::node::Numeric<uint16_t>*) target)->data = *(uint16_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::UINT32:
                ((qb::node::Numeric<uint32_t>*) target)->data = *(uint32_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::INT32:
                ((qb::node::Numeric<uint32_t>*) target)->data = *(uint32_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::FLOAT32:
                ((qb::node::Numeric<float>*) target)->data = *(float*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::STRING: 
                ((qb::node::String*) target)->data = std::string(*(std::string*) cast.value);
                return { .code = QB_OPERATOR_R_OK };
    
            // Array assignment not supported, assign each item instead
            case qb::Type::ARRAY:
                return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_VALUE_ARRAY };
        }
    }
    // Array Item
    else {
        switch (type) {
            // Invalid array types (unreachable)
            case qb::Type::VOID:
            case qb::Type::ERROR:
            case qb::Type::PTR:
                return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_UNKNOWN_ARRAY };
    
            case qb::Type::BOOL:
                ((qb::node::Array<bool>*) target)->items[index] = *(bool*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::UINT8:
                ((qb::node::Array<uint8_t>*) target)->items[index] = *(uint8_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::INT8:
                ((qb::node::Array<int8_t>*) target)->items[index] = *(int8_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::UINT16:
                ((qb::node::Array<uint16_t>*) target)->items[index] = *(uint16_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::INT16:
                ((qb::node::Array<uint16_t>*) target)->items[index] = *(uint16_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::UINT32:
                ((qb::node::Array<uint32_t>*) target)->items[index] = *(uint32_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::INT32:
                ((qb::node::Array<uint32_t>*) target)->items[index] = *(uint32_t*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::FLOAT32:
                ((qb::node::Array<float>*) target)->items[index] = *(float*) cast.value;
                return { .code = QB_OPERATOR_R_OK };
            case qb::Type::STRING: 
                ((qb::node::Array<std::string>*) target)->items[index] = std::string(*(std::string*) cast.value);
                return { .code = QB_OPERATOR_R_OK };
    
            // Invalid array type (unreachable)
            case qb::Type::ARRAY:
                return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_UNKNOWN_ARRAY };
        }
    }

    // Unknown type
    return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
}

// qb::_operator::res_t qb::_operator::copy_to(qb::Node* target, qb::Node* source) {
    
//     // Target MUST be a pointer
//     if (target->type != qb::Type::PTR) return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_NOT_POINTER };
//     auto pointer = (qb::node::Pointer*) target;

//     // Unresolved pointer, fail
//     if (pointer->data == nullptr) return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_NOT_RESOLVED };

//     bool is_array;
//     type_t type;

//     // Resolve target value type
//     if (pointer->data->type != qb::Type::ARRAY) {
//         // Index > 0 only allowed for arrays
        
//         if (pointer->index > 0) return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_NOT_INDEXABLE };

//         is_array = false;
//         type = pointer->data->type;
//     }
//     // Resolve target array item type
//     else {
//         auto array = (qb::node::Array<void*>*) pointer->data;
        
//         // Out of bounds
//         if (pointer->index >= array->length) return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_OUT_OF_BOUNDS };

//         is_array = true;
//         type = array->item_type;
//     }

//     // Cast to target type
//     auto cast = qb::_operator::cast_to(type, source);
//     if (cast.code > 0) return cast;
    
//     // Value
//     if (!is_array) {
//         switch (type) {
//             // Assigning to void doesn't make sense, it's always void
//             case qb::Type::VOID:
//                 return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_VALUE_VOID };

//             case qb::Type::ERROR:
//                 ((qb::node::Error*) pointer->data)->data = std::string(*(std::string*) cast.value);
//                 return { .code = QB_OPERATOR_R_OK };
            
//             case qb::Type::PTR:
//             {
//                 // Guaranteed to be a pointer, by 'as_pointer'
//                 auto ref = (qb::node::Pointer*) cast.value;
//                 ((qb::node::Pointer*) pointer->data)->device = ref->device;
//                 ((qb::node::Pointer*) pointer->data)->port = ref->port;
//                 ((qb::node::Pointer*) pointer->data)->index = ref->index;
//                 ((qb::node::Pointer*) pointer->data)->data = ref->data;
//                 return { .code = QB_OPERATOR_R_OK };
//             }
//             case qb::Type::BOOL:
//                 ((qb::node::Numeric<bool>*) pointer->data)->data = *(bool*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::UINT8:
//                 ((qb::node::Numeric<uint8_t>*) pointer->data)->data = *(uint8_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::INT8:
//                 ((qb::node::Numeric<int8_t>*) pointer->data)->data = *(int8_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::UINT16:
//                 ((qb::node::Numeric<uint16_t>*) pointer->data)->data = *(uint16_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::INT16:
//                 ((qb::node::Numeric<uint16_t>*) pointer->data)->data = *(uint16_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::UINT32:
//                 ((qb::node::Numeric<uint32_t>*) pointer->data)->data = *(uint32_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::INT32:
//                 ((qb::node::Numeric<uint32_t>*) pointer->data)->data = *(uint32_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::FLOAT32:
//                 ((qb::node::Numeric<float>*) pointer->data)->data = *(float*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::STRING: 
//                 ((qb::node::String*) pointer->data)->data = std::string(*(std::string*) cast.value);
//                 return { .code = QB_OPERATOR_R_OK };
    
//             // Array assignment not supported, assign each item instead
//             case qb::Type::ARRAY:
//                 return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_VALUE_ARRAY };
//         }
//     }
//     // Array Item
//     else {
//         switch (type) {
//             // Invalid array types (unreachable)
//             case qb::Type::VOID:
//             case qb::Type::ERROR:
//             case qb::Type::PTR:
//                 return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_UNKNOWN_ARRAY };
    
//             case qb::Type::BOOL:
//                 ((qb::node::Array<bool>*) pointer->data)->items[pointer->index] = *(bool*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::UINT8:
//                 ((qb::node::Array<uint8_t>*) pointer->data)->items[pointer->index] = *(uint8_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::INT8:
//                 ((qb::node::Array<int8_t>*) pointer->data)->items[pointer->index] = *(int8_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::UINT16:
//                 ((qb::node::Array<uint16_t>*) pointer->data)->items[pointer->index] = *(uint16_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::INT16:
//                 ((qb::node::Array<uint16_t>*) pointer->data)->items[pointer->index] = *(uint16_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::UINT32:
//                 ((qb::node::Array<uint32_t>*) pointer->data)->items[pointer->index] = *(uint32_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::INT32:
//                 ((qb::node::Array<uint32_t>*) pointer->data)->items[pointer->index] = *(uint32_t*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::FLOAT32:
//                 ((qb::node::Array<float>*) pointer->data)->items[pointer->index] = *(float*) cast.value;
//                 return { .code = QB_OPERATOR_R_OK };
//             case qb::Type::STRING: 
//                 ((qb::node::Array<std::string>*) pointer->data)->items[pointer->index] = std::string(*(std::string*) cast.value);
//                 return { .code = QB_OPERATOR_R_OK };
    
//             // Invalid array type (unreachable)
//             case qb::Type::ARRAY:
//                 return { .code = QB_OPERATOR_R_COPY_FAILED_TARGET_UNKNOWN_ARRAY };
//         }
//     }

//     // Unknown type
//     return { .code = QB_OPERATOR_R_UNKNOWN_TYPE };
// }