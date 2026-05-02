#pragma once
#include <new>
#include <vector>
#include <sstream>
#include "_def.h"
#include "types.h"

namespace qb {

    typedef uint8_t void_t;
    typedef uint8_t block_t;
    typedef uint16_t port_t;
    typedef uint16_t index_t;
    typedef uint8_t byte_t;

    // [ Custom (non-C++) Types ]

    namespace memory {

        uint8_t size_of(const Type* type);
        void assign(const Type* type, byte_t* target, byte_t* source);

        struct Reference {
            block_t block = 0;
            port_t port = 0;
            bool deref : 1 = false;
            bool slice : 1 = false;
            uint8_t _ : 6 = 0;
    
            Reference() {}

            Reference(block_t block, port_t port, bool deref, bool slice) {
                this->block = block;
                this->port = port;
                this->deref = deref;
                this->slice = slice;
            }

            void assign(Reference* other) {
                this->block = other->block;
                this->port = other->port;
                this->deref = other->deref;
                this->slice = other->slice;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << '@' << +this->block << '.' << +this->port;
                return ss.str();
            }
        };

        struct Event {
            enum Level {
                TRACE = 0x00,
                DEBUG = 0x01,
                INFO = 0x02,
                WARN = 0x03,
                ERROR = 0x04,
            } level : 8 = Level::INFO;
            byte_t code = 0x00;
            byte_t* data = nullptr;

            Event() {}

            ~Event() {
                if (this->data != nullptr)
                    delete[] this->data;
            }

            void assign(Event* other) {
                if (this->data != nullptr)
                    delete[] this->data;
                this->level = other->level;
                this->code = other->code;
                this->data = other->data;
            }
        };

    }

    // [ Memory Manipulation ]

    namespace memory {

        // (Forward Declarations)

        void free(const Type* type, byte_t* target);

        // [ Memory Allocation ]

        // Blocks are statically allocated on construction
        // based on the sizes below.

        uint8_t size_of(const Type* type) {
            switch (type->kind) {
                case qb::TypeKind::VOID: return sizeof(void_t);
                case qb::TypeKind::BOOL: return sizeof(bool);
                case qb::TypeKind::INT: return type->flags.of_int.res * sizeof(uint8_t);
                case qb::TypeKind::FLOAT: return sizeof(float);
                case qb::TypeKind::STRING: return sizeof(std::string);
                case qb::TypeKind::REF: return sizeof(memory::Reference);
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
                        case qb::TypeKind::REF: return sizeof(std::vector<Reference>);
                        // case qb::TypeKind::OBJ: {
                        //     if (this->types[port]->flags.of_obj.is_map) {
                        //         // memory::Map::init(data, this->types[port]->schema.of_map.type);
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
                        return sizeof(void*);
                }
                case qb::TypeKind::EVENT: return sizeof(Event);
            }
        }

        // Dynamic allocation is used on Events and Pools.

        byte_t* alloc(const Type* type) {
            auto size = size_of(type);
            auto target = new byte_t[size];
        }

        // [ Value Manipulation ]

        // Initialize a memory address with the proper
        // C++ type for a given qb::Type.
          
        void init_map(const Type* type, byte_t* target) {
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
                    new (target) std::map<std::string, Reference>();
                    break;
                // case qb::TypeKind::OBJ: {
                //     if (this->types[port]->flags.of_obj.is_map) {
                //         // memory::Map::init(data, this->types[port]->schema.of_map.type);
                //     }
                //     // auto map = std::map<std::string, bool>();
                //     // memcpy((std::map<std::string, bool>*) data, &map, sizeof(map));
                //     break;
                // }
                // case qb::TypeKind::EVENT: break;
                // case qb::TypeKind::VECTOR: break;
            }
        }
          
        void init_vec(const Type* type, byte_t* target) {
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
                    new (target) std::vector<Reference>();
                    break;
                // case qb::TypeKind::OBJ: {
                //     if (this->types[port]->flags.of_obj.is_map) {
                //         // memory::Map::init(data, this->types[port]->schema.of_map.type);
                //     }
                //     // auto map = std::map<std::string, bool>();
                //     // memcpy((std::map<std::string, bool>*) data, &map, sizeof(map));
                //     break;
                // }
                // case qb::TypeKind::EVENT: break;
                // case qb::TypeKind::VECTOR: break;
            }
        }
  
        void init(const Type* type, byte_t* target) {
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
                    new (target) memory::Reference();
                    break;
                case qb::TypeKind::VECTOR:
                    init_vec(type->schema.of_map.type, target);
                    break;
                case qb::TypeKind::OBJ: {
                    if (type->flags.of_obj.is_map) {
                        init_map(type->schema.of_map.type, target);
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
        
        // Assign values to pre-allocated memory
        // The memory MUST have been previously
        // allocated (statically or dynamically).

        void assign_map(const Type* type, byte_t* target, std::string key, byte_t* source) {
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
                    ((std::map<std::string, Reference>*) target)->insert_or_assign(key, *(Reference*) source);
                    break;
                // case qb::TypeKind::STRING:
                //     ((std::string*) target)->assign(*(std::string*) source);
                //     // ((std::string*) target)->assign(*(std::string*) source);
                //     break;
                // case qb::TypeKind::OBJ:
                //     if (type->flags.of_obj.is_map) {
                //         ((memory::Map*) target)->assign((memory::Map*) source);
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
        
        void assign_vec(const Type* type, byte_t* target, index_t index, byte_t* source) {
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
                    (*(std::vector<Reference>*) target)[index] = *(Reference*) source;
                    break;
                // case qb::TypeKind::STRING:
                //     ((std::string*) target)->assign(*(std::string*) source);
                //     // ((std::string*) target)->assign(*(std::string*) source);
                //     break;
                // case qb::TypeKind::OBJ:
                //     if (type->flags.of_obj.is_map) {
                //         ((memory::Map*) target)->assign((memory::Map*) source);
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
        
        void assign_event(const Type* type, byte_t* target, memory::Event::Level level, byte_t code, byte_t* source) {
            auto event = (memory::Event*) target;
            event->level = level;
            event->code = code;
            assign(type, event->data, source);
        }

        void assign(const Type* type, byte_t* target, byte_t* source) {
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
                    ((memory::Reference*) target)->assign((memory::Reference*) source);
                    break;
                case qb::TypeKind::VECTOR:
                    // Cannot be assigned directly (for now)
                    break;
                case qb::TypeKind::OBJ:
                    break;
                case qb::TypeKind::EVENT:
                    ((memory::Event*) target)->assign((memory::Event*) source);
                    break;
            }
        }

        // [ Memory Manipulation ]

        // Allows resizing vectors in memory.
        // The Block memory remains contiguous, but
        // it contains a pointer to the actual data changes.
        
        void resize_vec(const Type* type, byte_t* target, index_t size) {
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
                    ((std::vector<Reference>*) target)->resize(size);
                    break;
                // case qb::TypeKind::OBJ: {
                //     if (this->types[port]->flags.of_obj.is_map) {
                //         // memory::Map::init(data, this->types[port]->schema.of_map.type);
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

        void free_map(const Type* type, byte_t* target) {
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
                    ((std::map<std::string, Reference>*) target)->~map();
                    break;
                // case qb::TypeKind::STRING: {
                //     // ((memory::String*) (this->data + offset))->~String();
                //     break;
                // }
                // case qb::TypeKind::OBJ: {
                //     if (type->flags.of_obj.is_map) {
                //         ((memory::Map*) (this->data + offset))->~Map();
                //     }
                //     break;
                // }
                // case qb::TypeKind::EVENT: break;
                // case qb::TypeKind::VECTOR: break;
            }
        }
        
        void free_vec(const Type* type, byte_t* target) {
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
                    ((std::vector<Reference>*) target)->~vector();
                    break;
                // case qb::TypeKind::STRING: {
                //     // ((memory::String*) (this->data + offset))->~String();
                //     break;
                // }
                // case qb::TypeKind::OBJ: {
                //     if (type->flags.of_obj.is_map) {
                //         ((memory::Map*) (this->data + offset))->~Map();
                //     }
                //     break;
                // }
                // case qb::TypeKind::EVENT: break;
                // case qb::TypeKind::VECTOR: break;
            }
        }
        
        void free(const Type* type, byte_t* target) {
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
                    ((memory::Reference*) target)->~Reference();
                    break;
                }
                case qb::TypeKind::VECTOR: 
                    free_vec(type->schema.of_map.type, target);
                    break;
                case qb::TypeKind::OBJ: {
                    if (type->flags.of_obj.is_map) {
                        free_map(type->schema.of_map.type, target);
                    }
                    break;
                }
                case qb::TypeKind::EVENT: {
                    auto event = (memory::Event*) target;
                    if (event->data != nullptr) {
                        free(type->schema.of_map.type, event->data);
                    }
                    event->~Event();
                    break;
                }
            }
        }
    }

    // [ Memory Implementations ]

    namespace memory {

        // [Pool]
        // A dinamycally allocated set of values.

        class Pool {
            struct Item {
                Type* type;
                byte_t* value;
            };

            std::map<port_t, Item> data;

            public:

                ~Pool() {
                    for (const auto& it : this->data) {
                        auto item = it.second;
                        memory::free(item.type, item.value);
                    }
                }

                template <typename T>
                port_t add(Type* type, T value) {
                    auto target = memory::alloc(type);
                    memory::assign(type, target, (byte_t*) &value);
                    port_t port = this->data.size();
                    this->data.insert(port, {
                        .type = type,
                        .value = value
                    });
                    return port;
                }

                template <typename T>
                void set(port_t port, T value) {                
                    auto item = this->data[port];
                    memory::assign(item.type, item.value, (byte_t*) &value);
                }

        };

        // [Block]
        // A statically allocated contiguous block of memory.

        class Block {
    
            std::vector<const Type*> types;
            std::vector<uint16_t> pos;
            byte_t* data;
    
            public:
    
                Block(TypeChecker& checker, std::vector<type_t> tdxs):
                    types(std::vector<const Type*>(tdxs.size())),
                    pos(std::vector<uint16_t>(tdxs.size()))
                {
                    uint16_t offset = 0;
                    for (uint16_t i = 0; i < tdxs.size(); i++) {
                        this->types[i] = checker.get(tdxs[i]);
                        this->pos[i] = offset;
                        offset += memory::size_of(this->types[i]);
                    }
    
                    this->data = new byte_t[offset];
                    for (uint16_t i = 0; i < offset; i++) {
                        this->data[i] = 0;
                    }
    
                    this->init();
                }
    
                // [ Init / Free ]

                void init() {
                    uint16_t ports = this->types.size();
                    for (uint16_t port = 0; port < ports; port++) {
                        auto target = this->data + this->pos[port];
                        auto type = this->types[port];
                        memory::init(type, target);
                    }
                }
    
                ~Block() {
                    for (uint16_t i = 0; i < this->types.size(); i++) {
                        auto target = this->data + this->pos[i];
                        auto type = this->types[i];
                        memory::free(type, target);
                    }
                    delete[] this->data;
                }
    
                // [ Value Manipulation ]

                template <typename T>
                void set(port_t port, T value) {                
                    auto type = this->types[port];
                    byte_t* target = this->data + this->pos[port];
                    memory::assign(type, target, (byte_t*) &value);
                }
    
                template <typename T>
                void set_vec(port_t port, index_t index, T value) {                
                    auto type = this->types[port];
                    byte_t* target = this->data + this->pos[port];
                    memory::assign_vec(type->schema.of_map.type, target, index, (byte_t*) &value);
                }
                
                template <typename T>
                void set_map(port_t port, std::string key, T value) {                
                    auto type = this->types[port];
                    byte_t* target = this->data + this->pos[port];
                    memory::assign_map(type->schema.of_map.type, target, key, (byte_t*) &value);
                }
    
                template <typename T>
                void set_event(port_t port, memory::Event::Level level, byte_t code, T value) {
                    auto type = this->types[port];
                    byte_t* target = this->data + this->pos[port];
                    memory::assign_event(type->schema.of_map.type, target, level, code, (byte_t*) &value);
                }
        
                // [ Memory Manipulation ]

                void resize_vec(port_t port, index_t size) {
                    auto type = this->types[port];
                    byte_t* target = this->data + this->pos[port];             
                    memory::resize_vec(type->schema.of_map.type, target, size);
                }

                // [ Read ]
    
                template <typename T>
                T* get(port_t port) {
                    return (T*)(this->data + this->pos[port]);
                }
    
                template <typename T>
                std::vector<T>* get_vec(port_t port) {
                    return (std::vector<T>*)(this->data + this->pos[port]);
                }

                template <typename T>
                std::map<std::string, T>* get_map(port_t port) {
                    return (std::map<std::string, T>*)(this->data + this->pos[port]);
                }
    
                template <typename T>
                T* get_event(port_t port) {
                    auto event = (Event*)(this->data + this->pos[port]);
                    return (T*)event->data;
                }
    
    
        };

    }


}