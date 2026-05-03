#pragma once
#include <string>
#include <sstream>
#include "memory.h"

namespace qb {

    // template <typename T>
    // std::string print_vec(const Type* type, data_t value) {
    //     auto vec = (std::vector<T>*) value;
    //     std::stringstream ss;
    //     ss << '[';
    //     for (index_t i = 0; i < vec.size(); i++) {
    //         switch (type->kind) {
    //             case qb::TypeKind::VOID:
    //             case qb::TypeKind::BOOL:
    //             case qb::TypeKind::INT:
    //             case qb::TypeKind::FLOAT:
    //             case qb::TypeKind::STRING:
    //                 ss << vec[i];
    //                 break;
    //             case qb::TypeKind::REF:
    //                 ss << vec[i].to_str();
    //                 break;
    //             case qb::TypeKind::VECTOR:
    //                 ss << print_vec(type->schema.of_map.type, vec);
    //                 break;
    //             case qb::TypeKind::OBJ:
    //                 break;
    //             case qb::TypeKind::EVENT:
    //                 break;
    //         }
    //         if (i < vec.size()-1) ss << ',';
    //     }
    //     ss << ']';
    //     return ss;
    // }

    // std::string print(const Type* type, data_t value) {
    //     std::stringstream ss;
    //     switch (type->kind) {
    //         case TypeKind::VOID: {
    //             if (type->flags.of_void.is_void) ss << "void"; 
    //             else if (type->flags.of_void.is_null) ss << "null";
    //             else ss << "void<?>";
    //             break;
    //         }
    //         case TypeKind::BOOL: {
    //             ss << *(bool*) value;
    //             break;
    //         }
    //         case TypeKind::INT: {
    //             if (type->flags.of_int.is_unsigned) {
    //                 switch (type->flags.of_int.res) {
    //                     case 1: ss << *(uint8_t*) value; break;
    //                     case 2: ss << *(uint16_t*) value; break;
    //                     case 4: ss << *(uint32_t*) value; break;
    //                 }
    //             }
    //             else {
    //                 switch (type->flags.of_int.res) {
    //                     case 1: ss << *(int8_t*) value; break;
    //                     case 2: ss << *(int16_t*) value; break;
    //                     case 4: ss << *(int32_t*) value; break;
    //                 }
    //             }
    //             break;
    //         }
    //         case TypeKind::FLOAT: {
    //             ss << *(float*) value;
    //             break;
    //         }
    //         case TypeKind::STRING: {
    //             ss << *(std::string*) value;
    //             break;
    //         }
    //         case TypeKind::REF: {
    //             ss << ((mem::Reference*) value)->to_str();
    //             break;
    //         }
    //         case TypeKind::VECTOR: {
    //             ss << *(std::string*) value;
    //             switch (type->schema.of_map.type->kind) {
    //                 case qb::TypeKind::VOID: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //                 case qb::TypeKind::BOOL: ss << print_vec<bool>(type->schema.of_map.type, value); break;
    //                 case qb::TypeKind::INT: {
    //                     switch (type->flags.of_int.res) {
    //                         if (type->flags.of_int.is_unsigned) {
    //                             case 1: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //                             case 2: ss << print_vec<uint16_t>(type->schema.of_map.type, value); break;
    //                             case 4: ss << print_vec<uint32_t>(type->schema.of_map.type, value); break;
    //                         }
    //                         else {
    //                             case 1: ss << print_vec<int8_t>(type->schema.of_map.type, value); break;
    //                             case 2: ss << print_vec<int16_t>(type->schema.of_map.type, value); break;
    //                             case 4: ss << print_vec<int32_t>(type->schema.of_map.type, value); break;
    //                         }
    //                     }
    //                     break;
    //                 }
    //                 case qb::TypeKind::FLOAT: ss << print_vec<float>(type->schema.of_map.type, value); break;
    //                 case qb::TypeKind::STRING: ss << print_vec<std::string>(type->schema.of_map.type, value); break;
    //                 case qb::TypeKind::REF: ss << print_vec<mem::Reference>(type->schema.of_map.type, value); break;
    //                 // case qb::TypeKind::VECTOR: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //                 // case qb::TypeKind::OBJ: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //                 // case qb::TypeKind::EVENT: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //             }
    //             break;
    //         }
    //         // case TypeKind::OBJ: {
    //         //     if (type->flags.of_obj.is_map) {
    //         //         ss << *(std::string*) value;
    //         //         switch (type->schema.of_map.type->kind) {
    //         //             case qb::TypeKind::VOID: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //         //             case qb::TypeKind::BOOL: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //         //             case qb::TypeKind::INT: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //         //             case qb::TypeKind::FLOAT: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //         //             case qb::TypeKind::STRING: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //         //             case qb::TypeKind::REF: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //         //             case qb::TypeKind::VECTOR: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //         //             case qb::TypeKind::OBJ: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //         //             case qb::TypeKind::EVENT: ss << print_vec<uint8_t>(type->schema.of_map.type, value); break;
    //         //         }
    //         //         break;
    //         //     }
    //         //     else {
    //         //         ss << "struct{";
    //         //         auto n = type->schema.of_struct.n_fields;
    //         //         for (size_t i = 0; i < n; i++) {
    //         //             ss << type->schema.of_struct.fields[i]->to_str();
    //         //             if (i < n-1) ss << ",";
    //         //         }
    //         //         ss << "}";
    //         //     }
    //         //     break;
    //         // }
    //         // case TypeKind::EVENT: {
    //         //     ss << "event{" << type->schema.of_map.type->to_str() << "}";
    //         //     break;
    //         // }
    //     }
    //     return ss.str();
    // }

}