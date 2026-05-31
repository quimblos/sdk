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

// Assign raw values (from code) to pre-allocated memory.

bool qb::mem::copy_raw(const qb::Type* type, qb::data_t target, const qb::byte_t* bytes, const qb::code_addr_t length, qb::code_addr_t* out_parsed_bytes) {
    switch (type->kind) {
        case qb::TypeKind::VOID: {
            *out_parsed_bytes = 0;
            return true;
        }
        case qb::TypeKind::BOOL: {
            if (length < 1) return false;
            *(bool*) target = *(bool*) bytes;
            *out_parsed_bytes = 1;
            return true;
        }
        case qb::TypeKind::INT: {
            if (length < type->flags.of_int.res) return false;
            switch (type->flags.of_int.res) {
                case 1: *(uint8_t*)  target = *(uint8_t*) bytes; *out_parsed_bytes = 1; break;
                case 2: *(uint16_t*) target = parse_u16(bytes); *out_parsed_bytes = 2; break;
                case 4: *(uint32_t*) target = parse_u32(bytes); *out_parsed_bytes = 4; break;
            }
            return true;
        }
        case qb::TypeKind::FLOAT: {
            if (length < 4) return false;
            *(float*) target = *(float*) bytes;
            *out_parsed_bytes = 4;
            return true;
        }
        case qb::TypeKind::STRING: {
            uint16_t str_len = parse_u16(bytes);
            if (length < str_len+2) return false;
            ((std::string*) target)->assign(std::string((char*) bytes+2, str_len));
            *out_parsed_bytes = str_len;
            return true;
        }
        case qb::TypeKind::REF: {
            if (length < 2) return false;
            ((qb::mem::Reference*) target)->block = bytes[0];
            ((qb::mem::Reference*) target)->port = bytes[1];
            *out_parsed_bytes = 2;
            return true;
        }
        case qb::TypeKind::REF_SLICE: {
            if (length < 3) return false;
            uint8_t dims = bytes[2];
            if (length < 3 + dims * 4) return false;
            ((qb::mem::SlicedReference*) target)->block = bytes[0];
            ((qb::mem::SlicedReference*) target)->port = bytes[1];
            ((qb::mem::SlicedReference*) target)->shape.resize(dims);
            for (uint8_t d = 0; d < dims; d++) {
                ((qb::mem::SlicedReference*) target)->shape[0] = {
                    .start = parse_u16(bytes+3+d*2),
                    .end = parse_u16(bytes+4+d*2)
                };
            }
            *out_parsed_bytes = 3+dims*4;
            return true;
        }
        case qb::TypeKind::VECTOR: {
            uint16_t dims = bytes[2];
            if (length < 3+dims*2) return false;
            auto shape = std::vector<index_t>(dims);
            for (uint8_t i = 0; i < dims; i++) {
                shape[i] = parse_u16(bytes+3+i*2);
            }
            auto vec = (qb::mem::Vector*) target;
            vec->resize(shape);
            auto child_type = type->schema.of_map.type;
            code_addr_t addr = 3+dims*2;
            for (code_addr_t i = 0; i < vec->size(); i++) {
                code_addr_t out = 0;
                bool res = copy_raw(child_type, vec->get(i), bytes+addr, length-addr, &out);
                if (!res) return false;
                addr += out;
            }
            return true;
        }            
        case qb::TypeKind::MAP:       break; // Cannot be assigned directly (for now)
        case qb::TypeKind::STRUCT:    break; // Cannot be assigned directly (for now)
        case qb::TypeKind::EVENT:     break; // Cannot be assigned directly (for now)
    }
    return false;
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