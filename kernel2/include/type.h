#pragma once
#include <vector>
#include <sstream>
#include "config.h"

namespace qb {

    enum TypeKind {
        VOID = 0x00,    
        BOOL = 0x01,    
        INT = 0x02,     
        FLOAT = 0x03,
        STRING = 0x04,
        REF = 0x05,
        REF_SLICE = 0x06,
        VECTOR = 0x10,  // no built-in types, schema of_map
        MAP = 0x11,     // no built-in types, schema of_map
        STRUCT = 0x12,  // no built-in types, schema of_struct
        EVENT = 0x13,   // no built-in types, schema of_map
        FN = 0x20,      // no built-in types, schema of_map
    };
        
    union TypeFlags {
        struct {
            const bool is_void : 1 = false;
            const bool is_null : 1 = false;
            const uint8_t _ : 6 = 0;
        } of_void;

        struct {
            const bool is_unsigned : 1 = false;
            const uint8_t res : 7 = 0;
        } of_int;

        struct {
            // const bool is_template : 1 = false;
            const uint8_t _ : 8 = 0;
        } of_string;

        uint8_t value;
    };

    struct Type;
    struct Code;
    union TypeSchema {
        struct _ref {
            const Type* type;
        } of_map;
        struct _struct {
            const port_t n_fields;
            const Type** fields;
            ~_struct() {
                delete[] this->fields;
            }
        } of_struct;
        struct _fn {
            const Type* return_type;
            const Code* code;
        } of_fn;
        ~TypeSchema() {}
    };

    struct Type {
        const TypeKind kind;
        const TypeFlags flags;
        const TypeSchema schema;

        ~Type() {
            switch (this->kind) {
                case qb::TypeKind::VOID:
                case qb::TypeKind::BOOL:
                case qb::TypeKind::INT:
                case qb::TypeKind::FLOAT:
                case qb::TypeKind::STRING:
                    break;
                case qb::TypeKind::REF:
                    // this->schema.of_map.~_ref();
                    break;
                case qb::TypeKind::VECTOR:
                    this->schema.of_map.~_ref();
                    break;
                case qb::TypeKind::MAP:
                    this->schema.of_map.~_ref();
                    break;
                case qb::TypeKind::STRUCT:
                    this->schema.of_struct.~_struct();
                    break;
                case qb::TypeKind::EVENT:
                    // this->schema.of_map.~_ref();
                    break;
                case qb::TypeKind::FN:
                    break;
            }
        }

        std::string to_str() const {
            std::stringstream ss;
            switch (this->kind) {
                case TypeKind::VOID: {
                    if (this->flags.of_void.is_void) ss << "void"; 
                    else if (this->flags.of_void.is_null) ss << "null";
                    else ss << "void<?>";
                    break;
                }
                case TypeKind::BOOL: {
                    ss << "bool";
                    break;
                }
                case TypeKind::INT: {
                    if (this->flags.of_int.is_unsigned) ss << "u";
                    else ss << "i";
                    ss << this->flags.of_int.res * 8;
                    break;
                }
                case TypeKind::FLOAT: {
                    ss << "f32";
                    break;
                }
                case TypeKind::STRING: {
                    ss << "string";
                    break;
                }
                case TypeKind::REF: {
                    ss << "ref";
                    break;
                }
                case TypeKind::REF_SLICE: {
                    ss << "ref[#]";
                    break;
                }
                case TypeKind::VECTOR: {
                    ss << "vec[" << this->schema.of_map.type->to_str() << "]";
                    break;
                }
                case TypeKind::MAP: {
                    ss << "map{" << this->schema.of_map.type->to_str() << "}";
                    break;
                }
                case TypeKind::STRUCT: {
                    ss << "struct{";
                    auto n = this->schema.of_struct.n_fields;
                    for (size_t i = 0; i < n; i++) {
                    ss << this->schema.of_struct.fields[i]->to_str();
                    if (i < n-1) ss << ",";
                    }
                    ss << "}";
                    break;
                }
                case TypeKind::EVENT: {
                    ss << "event{" << this->schema.of_map.type->to_str() << "}";
                    break;
                }
                case TypeKind::FN: {
                    ss << "fn:" << this->schema.of_fn.return_type->to_str();
                    break;
                }
            }
            return ss.str();
        }

    };

}