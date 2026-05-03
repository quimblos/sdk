#include "memory.h"

// (Forward Declarations)

void qb::mem::free(const qb::Type* type, qb::byte_t* target);

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
        case qb::TypeKind::VECTOR: {
            switch (type->schema.of_map.type->kind) {
                case qb::TypeKind::VOID: return sizeof(std::vector<void_t>);
                case qb::TypeKind::BOOL: return sizeof(std::vector<bool>);
                case qb::TypeKind::INT:
                    switch (type->flags.of_int.res) {
                        case 1: return sizeof(std::vector<uint8_t>);
                        case 2: return sizeof(std::vector<uint16_t>);
                        case 4: return sizeof(std::vector<uint32_t>);
                    }
                case qb::TypeKind::FLOAT: return sizeof(std::vector<float>);
                case qb::TypeKind::STRING: return sizeof(std::vector<std::string>);
                case qb::TypeKind::REF: return sizeof(std::vector<qb::mem::Reference>);
                // case qb::TypeKind::OBJ: {
                //     if (this->types[port]->flags.of_obj.is_map) {
                //         // mem::Map::init(data, this->types[port]->schema.of_map.type);
                //     }
                //     // auto map = std::map<std::string, bool>();
                //     // memcpy((std::map<std::string, bool>*) data, &map, sizeof(map));
                //     break;
                // }
                // case qb::TypeKind::EVENT: break;
                // case qb::TypeKind::VECTOR: break;
            }
        }
        case qb::TypeKind::OBJ: {
            if (type->flags.of_obj.is_map)
                return sizeof(std::map<std::string, void*>);
            else
                return sizeof(qb::mem::Struct);
        }
        case qb::TypeKind::EVENT: return sizeof(Event);
    }
}

// Dynamic allocation is used on Events and Pools.

qb::byte_t* qb::mem::alloc(const qb::Type* type) {
    auto size = size_of(type);
    auto target = new qb::byte_t[size];
}

// [ Value Manipulation ]

// Initialize a memory address with the proper
// C++ type for a given qb::qb::Type.
    
void qb::mem::init(const qb::Type* type, qb::byte_t* target) {
    switch (type->kind) {
        case qb::TypeKind::VOID:
        case qb::TypeKind::BOOL:
        case qb::TypeKind::INT:
        case qb::TypeKind::FLOAT:
            break;
        case qb::TypeKind::STRING: {
            new (target) std::string();
            break;
        }
        case qb::TypeKind::REF:
            new (target) qb::mem::Reference();
            break;
        case qb::TypeKind::VECTOR:
            init_vec(type->schema.of_map.type, target);
            break;
        case qb::TypeKind::OBJ: {
            if (type->flags.of_obj.is_map) {
                init_map(type->schema.of_map.type, target);
            }
            else {
                new (target) Struct(type);
            }
            break;
        }
        case qb::TypeKind::EVENT: {
            auto event = new (target) Event();
            // Free previous event data
            if (event->data != nullptr) free(type, event->data);
            // Allocate new event data
            auto data_type = type->schema.of_map.type;
            event->data = alloc(data_type);
            // Initialize new event data
            init(data_type, event->data);
        }
            break;
    }
}

void qb::mem::init_vec(const qb::Type* type, qb::byte_t* target) {
    switch (type->kind) {
        case qb::TypeKind::VOID:
            break;
        case qb::TypeKind::BOOL:
            new (target) std::vector<bool>();
            break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1:
                    new (target) std::vector<uint8_t>();
                    break;
                case 2:
                    new (target) std::vector<uint16_t>();
                    break;
                case 4:
                    new (target) std::vector<uint32_t>();
                    break;
            }
            break;
        case qb::TypeKind::FLOAT:
            new (target) std::vector<float>();
            break;
        case qb::TypeKind::STRING:
            new (target) std::vector<std::string>();
            break;
        case qb::TypeKind::REF:
            new (target) std::vector<qb::mem::Reference>();
            break;
        // case qb::TypeKind::OBJ: {
        //     if (this->types[port]->flags.of_obj.is_map) {
        //         // mem::Map::init(data, this->types[port]->schema.of_map.type);
        //     }
        //     // auto map = std::map<std::string, bool>();
        //     // memcpy((std::map<std::string, bool>*) data, &map, sizeof(map));
        //     break;
        // }
        // case qb::TypeKind::EVENT: break;
        // case qb::TypeKind::VECTOR: break;
    }
}

void qb::mem::init_map(const qb::Type* type, qb::byte_t* target) {
    switch (type->kind) {
        case qb::TypeKind::VOID:
            break;
        case qb::TypeKind::BOOL:
            new (target) std::map<std::string, bool>();
            break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1:
                    new (target) std::map<std::string, uint8_t>();
                    break;
                case 2:
                    new (target) std::map<std::string, uint16_t>();
                    break;
                case 4:
                    new (target) std::map<std::string, uint32_t>();
                    break;
            }
            break;
        case qb::TypeKind::FLOAT:
            new (target) std::map<std::string, float>();
            break;
        case qb::TypeKind::STRING:
            new (target) std::map<std::string, std::string>();
            break;
        case qb::TypeKind::REF:
            new (target) std::map<std::string, qb::mem::Reference>();
            break;
        // case qb::TypeKind::OBJ: {
        //     if (this->types[port]->flags.of_obj.is_map) {
        //         // mem::Map::init(data, this->types[port]->schema.of_map.type);
        //     }
        //     // auto map = std::map<std::string, bool>();
        //     // memcpy((std::map<std::string, bool>*) data, &map, sizeof(map));
        //     break;
        // }
        // case qb::TypeKind::EVENT: break;
        // case qb::TypeKind::VECTOR: break;
    }
}

// Assign values to pre-allocated memory
// The memory MUST have been previously
// allocated (statically or dynamically).

void qb::mem::assign(const qb::Type* type, qb::byte_t* target, qb::byte_t* source) {
    switch (type->kind) {
        case qb::TypeKind::VOID:
            break;
        case qb::TypeKind::BOOL:
            *(bool*) target = *(bool*) source;
            break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1:
                    *(uint8_t*) target = *(uint8_t*) source;
                    break;
                case 2:
                    *(uint16_t*) target = *(uint16_t*) source;
                    break;
                case 4:
                    *(uint32_t*) target = *(uint32_t*) source;
                    break;
            }
            break;
        case qb::TypeKind::FLOAT:
            *(float*) target = *(float*) source;
            break;
        case qb::TypeKind::STRING:
            ((std::string*) target)->assign(*(std::string*) source);
            break;
        case qb::TypeKind::REF:
            ((qb::mem::Reference*) target)->assign((qb::mem::Reference*) source);
            break;
        case qb::TypeKind::VECTOR:
            // Cannot be assigned directly (for now)
            break;
        case qb::TypeKind::OBJ:
            break;
        case qb::TypeKind::EVENT:
            ((qb::mem::Event*) target)->assign((qb::mem::Event*) source);
            break;
    }
}

void qb::mem::assign_vec(const qb::Type* type, qb::byte_t* target, index_t index, qb::byte_t* source) {
    switch (type->kind) {
        case qb::TypeKind::VOID:
            break;
        case qb::TypeKind::BOOL:
            (*(std::vector<bool>*) target)[index] = *(bool*) source;
            break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1:
                    (*(std::vector<uint8_t>*) target)[index] = *(uint8_t*) source;
                    break;
                case 2:
                    (*(std::vector<uint16_t>*) target)[index] = *(uint16_t*) source;
                    break;
                case 4:
                    (*(std::vector<uint32_t>*) target)[index] = *(uint32_t*) source;
                    break;
            }
            break;
        case qb::TypeKind::FLOAT:
            (*(std::vector<float>*) target)[index] = *(float*) source;
            break;
        case qb::TypeKind::STRING:
            (*(std::vector<std::string>*) target)[index] = *(std::string*) source;
            break;
        case qb::TypeKind::REF:
            (*(std::vector<qb::mem::Reference>*) target)[index] = *(qb::mem::Reference*) source;
            break;
        // case qb::TypeKind::STRING:
        //     ((std::string*) target)->assign(*(std::string*) source);
        //     // ((std::string*) target)->assign(*(std::string*) source);
        //     break;
        // case qb::TypeKind::OBJ:
        //     if (type->flags.of_obj.is_map) {
        //         ((mem::Map*) target)->assign((mem::Map*) source);
        //     }
        //     else {
        //         *(void**) target = *(void**) source;
        //     }
        //     break;
        // case qb::TypeKind::EVENT:
        //     *(void**) target = *(void**) source;
        //     break;
        // case qb::TypeKind::VECTOR:
        //     *(void**) target = *(void**) source;
        //     break;
    }
}

void qb::mem::assign_map(const qb::Type* type, qb::byte_t* target, std::string key, qb::byte_t* source) {
    switch (type->kind) {
        case qb::TypeKind::VOID:
            break;
        case qb::TypeKind::BOOL:
            ((std::map<std::string, bool>*) target)->insert_or_assign(key, *(bool*) source);
            break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1:
                    ((std::map<std::string, uint8_t>*) target)->insert_or_assign(key, *(uint8_t*) source);
                    break;
                case 2:
                    ((std::map<std::string, uint16_t>*) target)->insert_or_assign(key, *(uint16_t*) source);
                    break;
                case 4:
                    ((std::map<std::string, uint32_t>*) target)->insert_or_assign(key, *(uint32_t*) source);
                    break;
            }
            break;
        case qb::TypeKind::FLOAT:
            ((std::map<std::string, float>*) target)->insert_or_assign(key, *(float*) source);
            break;
        case qb::TypeKind::STRING:
            ((std::map<std::string, std::string>*) target)->insert_or_assign(key, *(std::string*) source);
            break;
        case qb::TypeKind::REF:
            ((std::map<std::string, qb::mem::Reference>*) target)->insert_or_assign(key, *(qb::mem::Reference*) source);
            break;
        // case qb::TypeKind::STRING:
        //     ((std::string*) target)->assign(*(std::string*) source);
        //     // ((std::string*) target)->assign(*(std::string*) source);
        //     break;
        // case qb::TypeKind::OBJ:
        //     if (type->flags.of_obj.is_map) {
        //         ((mem::Map*) target)->assign((mem::Map*) source);
        //     }
        //     else {
        //         *(void**) target = *(void**) source;
        //     }
        //     break;
        // case qb::TypeKind::EVENT:
        //     *(void**) target = *(void**) source;
        //     break;
        // case qb::TypeKind::VECTOR:
        //     *(void**) target = *(void**) source;
        //     break;
    }
}

void qb::mem::assign_event(const qb::Type* type, qb::byte_t* target, qb::EventLevel level, qb::byte_t code, qb::byte_t* source) {
    auto event = (qb::mem::Event*) target;
    event->level = level;
    event->code = code;
    assign(type, event->data, source);
}


// [ Memory Manipulation ]

// Allows resizing vectors in memory.
// The Block memory remains contiguous, but
// it contains a pointer to the actual data changes.

void qb::mem::resize_vec(const qb::Type* type, qb::byte_t* target, index_t size) {
    switch (type->kind) {
        case qb::TypeKind::VOID:
            break;
        case qb::TypeKind::BOOL:
            ((std::vector<bool>*) target)->resize(size);
            break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1:
                    ((std::vector<uint8_t>*) target)->resize(size);
                    break;
                case 2:
                    ((std::vector<uint16_t>*) target)->resize(size);
                    break;
                case 4:
                    ((std::vector<uint32_t>*) target)->resize(size);
                    break;
            }
            break;
        case qb::TypeKind::FLOAT:
            ((std::vector<float>*) target)->resize(size);
            break;
        case qb::TypeKind::STRING:
            ((std::vector<std::string>*) target)->resize(size);
            break;
        case qb::TypeKind::REF:
            ((std::vector<qb::mem::Reference>*) target)->resize(size);
            break;
        // case qb::TypeKind::OBJ: {
        //     if (this->types[port]->flags.of_obj.is_map) {
        //         // mem::Map::init(data, this->types[port]->schema.of_map.type);
        //     }
        //     // auto map = std::map<std::string, bool>();
        //     // memcpy((std::map<std::string, bool>*) data, &map, sizeof(map));
        //     break;
        // }
        // case qb::TypeKind::EVENT: break;
        // case qb::TypeKind::VECTOR: break;
    }
}

// Free C++ allocated resources for a given address
// of a given type.
// This MUST be called for each data when destroying
// the Block to avoid memory leaks.

void qb::mem::free(const qb::Type* type, qb::byte_t* target) {
    switch (type->kind) {
        case qb::TypeKind::VOID: break;
        case qb::TypeKind::BOOL: break;
        case qb::TypeKind::INT: break;
        case qb::TypeKind::FLOAT: break;
        case qb::TypeKind::STRING: {
            ((std::string*) target)->~basic_string();
            break;
        }
        case qb::TypeKind::REF: {
            ((qb::mem::Reference*) target)->~Reference();
            break;
        }
        case qb::TypeKind::VECTOR: 
            free_vec(type->schema.of_map.type, target);
            break;
        case qb::TypeKind::OBJ: {
            if (type->flags.of_obj.is_map) {
                free_map(type->schema.of_map.type, target);
            }
            else {
                ((qb::mem::Struct*) target)->~Struct();
            }
            break;
        }
        case qb::TypeKind::EVENT: {
            auto event = (qb::mem::Event*) target;
            if (event->data != nullptr) {
                free(type->schema.of_map.type, event->data);
            }
            event->~Event();
            break;
        }
    }
}

void qb::mem::free_vec(const qb::Type* type, qb::byte_t* target) {
    switch (type->kind) {
        // case qb::TypeKind::VOID: break;
        case qb::TypeKind::BOOL:
            ((std::vector<bool>*) target)->~vector();
            break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1:
                    ((std::vector<uint8_t>*) target)->~vector();
                    break;
                case 2:
                    ((std::vector<uint16_t>*) target)->~vector();
                    break;
                case 4:
                    ((std::vector<uint32_t>*) target)->~vector();
                    break;
            }
            break;
        case qb::TypeKind::FLOAT:
            ((std::vector<float>*) target)->~vector();
            break;
        case qb::TypeKind::STRING:
            ((std::vector<std::string>*) target)->~vector();
            break;
        case qb::TypeKind::REF:
            ((std::vector<qb::mem::Reference>*) target)->~vector();
            break;
        // case qb::TypeKind::STRING: {
        //     // ((mem::String*) (this->data + offset))->~String();
        //     break;
        // }
        // case qb::TypeKind::OBJ: {
        //     if (type->flags.of_obj.is_map) {
        //         ((mem::Map*) (this->data + offset))->~Map();
        //     }
        //     break;
        // }
        // case qb::TypeKind::EVENT: break;
        // case qb::TypeKind::VECTOR: break;
    }
}

void qb::mem::free_map(const qb::Type* type, qb::byte_t* target) {
    switch (type->kind) {
        // case qb::TypeKind::VOID: break;
        case qb::TypeKind::BOOL:
            ((std::map<std::string, bool>*) target)->~map();
            break;
        case qb::TypeKind::INT:
            switch (type->flags.of_int.res) {
                case 1:
                    ((std::map<std::string, uint8_t>*) target)->~map();
                    break;
                case 2:
                    ((std::map<std::string, uint16_t>*) target)->~map();
                    break;
                case 4:
                    ((std::map<std::string, uint32_t>*) target)->~map();
                    break;
            }
            break;
        case qb::TypeKind::FLOAT:
            ((std::map<std::string, float>*) target)->~map();
            break;
        case qb::TypeKind::STRING:
            ((std::map<std::string, std::string>*) target)->~map();
            break;
        case qb::TypeKind::REF:
            ((std::map<std::string, qb::mem::Reference>*) target)->~map();
            break;
        // case qb::TypeKind::STRING: {
        //     // ((mem::String*) (this->data + offset))->~String();
        //     break;
        // }
        // case qb::TypeKind::OBJ: {
        //     if (type->flags.of_obj.is_map) {
        //         ((mem::Map*) (this->data + offset))->~Map();
        //     }
        //     break;
        // }
        // case qb::TypeKind::EVENT: break;
        // case qb::TypeKind::VECTOR: break;
    }
}
