#include "memory.h"

// [ Memory Allocation ]

// Blocks are statically allocated on construction
// based on the sizes below.

uint8_t qb::mem::size_of(const qb::Type* type) {
    switch (type->kind) {
        case qb::TypeKind::VOID: return sizeof(void_t);
        case qb::TypeKind::BOOL: return sizeof(bool);
        case qb::TypeKind::INT: return type->flags.of_int.res * sizeof(uint8_t);
        case qb::TypeKind::FLOAT: return sizeof(float);
        case qb::TypeKind::STRING: return sizeof(std::string);
        case qb::TypeKind::REF: return sizeof(qb::mem::Reference);
        case qb::TypeKind::REF_SLICE: return sizeof(qb::mem::SlicedReference);
        case qb::TypeKind::VECTOR: return sizeof(qb::mem::Vector);
        case qb::TypeKind::MAP: return sizeof(qb::mem::Map);
        case qb::TypeKind::STRUCT: return sizeof(qb::mem::Struct);
        case qb::TypeKind::EVENT: return sizeof(qb::mem::Event);
    }
    return 0;
}

// Dynamic allocation is used on Events and Pools.

qb::data_t qb::mem::alloc(const qb::Type* type) {
    auto size = size_of(type);
    return new qb::byte_t[size];
}

// [ Value Manipulation ]

// Initialize a memory address with the proper
// C++ type for a given qb::qb::Type.
    
void qb::mem::init(const qb::Type* type, qb::data_t target) {
    switch (type->kind) {
        case qb::TypeKind::VOID:    break;
        case qb::TypeKind::BOOL:    *target = false; break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1:             *(uint8_t*) target = 0; break;
                case 2:             *(uint16_t*) target = 0; break;
                case 4:             *(uint32_t*) target = 0; break;
            }
            break;
        case qb::TypeKind::FLOAT:     *target = (float) 0; break;
        case qb::TypeKind::STRING:    new (target) std::string(); break;
        case qb::TypeKind::REF:       new (target) qb::mem::Reference(); break;
        case qb::TypeKind::REF_SLICE: new (target) qb::mem::SlicedReference(); break;
        case qb::TypeKind::VECTOR:    new (target) qb::mem::Vector(type->schema.of_map.type, {}); break;
        case qb::TypeKind::MAP:       new (target) qb::mem::Map(type->schema.of_map.type); break;
        case qb::TypeKind::STRUCT:    new (target) qb::mem::Struct(type); break;
        case qb::TypeKind::EVENT:     new (target) qb::mem::Event(type->schema.of_map.type); break;
    }
}

// Assign values to pre-allocated memory
// The memory MUST have been previously
// allocated (statically or dynamically).

void qb::mem::copy(const qb::Type* type, qb::data_t target, qb::data_t source) {
    switch (type->kind) {
        case qb::TypeKind::VOID:    break;
        case qb::TypeKind::BOOL:    *(bool*) target = *(bool*) source; break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1: *(uint8_t*)  target = *(uint8_t*) source; break;
                case 2: *(uint16_t*) target = *(uint16_t*) source; break;
                case 4: *(uint32_t*) target = *(uint32_t*) source; break;
            }
            break;
        case qb::TypeKind::FLOAT:     *(float*) target = *(float*) source; break;
        case qb::TypeKind::STRING:    ((std::string*) target)->assign(*(std::string*) source); break;
        case qb::TypeKind::REF:       ((qb::mem::Reference*) target)->assign((qb::mem::Reference*) source); break;
        case qb::TypeKind::REF_SLICE: ((qb::mem::SlicedReference*) target)->assign((qb::mem::SlicedReference*) source); break;
        case qb::TypeKind::VECTOR:    break; // Cannot be assigned directly (for now)
        case qb::TypeKind::MAP:       break; // Cannot be assigned directly (for now)
        case qb::TypeKind::STRUCT:    break; // Cannot be assigned directly (for now)
        case qb::TypeKind::EVENT:     ((qb::mem::Event*) target)->assign((qb::mem::Event*) source); break;
    }
}

// // [ Memory Manipulation ]

// Free C++ allocated resources for a given address
// of a given type.
// This MUST be called for each data when destroying
// the Block to avoid memory leaks.

void qb::mem::free(const qb::Type* type, qb::data_t target) {
    switch (type->kind) {
        case qb::TypeKind::VOID:      break;
        case qb::TypeKind::BOOL:      break;
        case qb::TypeKind::INT:       break;
        case qb::TypeKind::FLOAT:     break;
        case qb::TypeKind::STRING:    ((std::string*) target)->~basic_string(); break;
        case qb::TypeKind::REF:       ((qb::mem::Reference*) target)->~Reference(); break;
        case qb::TypeKind::REF_SLICE: ((qb::mem::SlicedReference*) target)->~SlicedReference(); break;
        case qb::TypeKind::VECTOR:    ((qb::mem::Vector*) target)->~Vector(); break;
        case qb::TypeKind::MAP:       ((qb::mem::Map*) target)->~Map(); break;
        case qb::TypeKind::STRUCT:    ((qb::mem::Struct*) target)->~Struct(); break;
        case qb::TypeKind::EVENT:     ((qb::mem::Event*) target)->~Event(); break;
    }
}