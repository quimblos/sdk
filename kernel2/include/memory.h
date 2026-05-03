#pragma once
#include <vector>
#include <sstream>
#include "typesolver.h"

namespace qb {
    
    enum EventLevel {
        TRACE = 0x00,
        DEBUG = 0x01,
        INFO = 0x02,
        WARN = 0x03,
        ERROR = 0x04,
    };

    // [ Memory Manipulation ]

    namespace mem {

        // [ Memory Allocation ]

        // Blocks are statically allocated on construction
        // based on the sizes below.

        uint8_t size_of(const Type* type);

        // Dynamic allocation is used on Events and Pools.

        byte_t* alloc(const Type* type);

        // [ Value Manipulation ]

        // Initialize a memory address with the proper
        // C++ type for a given qb::Type.
          
        void init(const Type* type, byte_t* target);
        void init_vec(const Type* type, byte_t* target);
        void init_map(const Type* type, byte_t* target);  
        
        // Assign values to pre-allocated memory
        // The memory MUST have been previously
        // allocated (statically or dynamically).

        void assign(const Type* type, byte_t* target, byte_t* source);
        void assign_vec(const Type* type, byte_t* target, index_t index, byte_t* source);
        void assign_map(const Type* type, byte_t* target, std::string key, byte_t* source);
        void assign_event(const Type* type, byte_t* target, EventLevel level, byte_t code, byte_t* source);

        // [ Memory Manipulation ]

        // Allows resizing vectors in memory.
        // The Block memory remains contiguous, but
        // it contains a pointer to the actual data changes.
        
        void resize_vec(const Type* type, byte_t* target, index_t size);

        // Free C++ allocated resources for a given address
        // of a given type.
        // This MUST be called for each data when destroying
        // the Block to avoid memory leaks.
        
        void free(const Type* type, byte_t* target);
        void free_vec(const Type* type, byte_t* target);
        void free_map(const Type* type, byte_t* target);
    }

    // [ Custom (non-C++) Types ]

    namespace mem {

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
            EventLevel level : 8 = EventLevel::INFO;
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

        struct Struct {
            const Type* type;
            std::vector<index_t> pos;
            byte_t* data = nullptr;

            Struct(const Type* type):
                type(type)
            {
                port_t ports = type->schema.of_struct.n_fields;
                pos.resize(type->schema.of_struct.n_fields);

                index_t offset = 0;
                for (index_t i = 0; i < ports; i++) {
                    this->pos[i] = offset;
                    auto item_type = type->schema.of_struct.fields[i];
                    offset += mem::size_of(item_type);
                }

                this->data = new byte_t[offset];
                for (index_t i = 0; i < offset; i++) {
                    this->data[i] = 0;
                }

                this->init();
            }

            // [ Init / Free ]

            void init() {
                port_t ports = this->type->schema.of_struct.n_fields;
                for (port_t port = 0; port < ports; port++) {
                    auto target = this->data + this->pos[port];
                    auto type = this->type->schema.of_struct.fields[port];
                    mem::init(type, target);
                }
            }

            ~Struct() {
                port_t ports = this->type->schema.of_struct.n_fields;
                for (port_t i = 0; i < ports; i++) {
                    auto target = this->data + this->pos[i];
                    auto type = this->type->schema.of_struct.fields[i];
                    mem::free(type, target);
                }
                delete[] this->data;
            }

            // [ Value Manipulation ]

            template <typename T>
            void set(port_t port, T value) {                
                auto type = this->type->schema.of_struct.fields[port];
                byte_t* target = this->data + this->pos[port];
                mem::assign(type, target, (byte_t*) &value);
            }

            template <typename T>
            void set_vec(port_t port, index_t index, T value) {                
                auto type = this->type->schema.of_struct.fields[port];
                byte_t* target = this->data + this->pos[port];
                mem::assign_vec(type->schema.of_map.type, target, index, (byte_t*) &value);
            }
            
            template <typename T>
            void set_map(port_t port, std::string key, T value) {                
                auto type = this->type->schema.of_struct.fields[port];
                byte_t* target = this->data + this->pos[port];
                mem::assign_map(type->schema.of_map.type, target, key, (byte_t*) &value);
            }

            template <typename T>
            void set_event(port_t port, EventLevel level, byte_t code, T value) {
                auto type = this->type->schema.of_struct.fields[port];
                byte_t* target = this->data + this->pos[port];
                mem::assign_event(type->schema.of_map.type, target, level, code, (byte_t*) &value);
            }
    
            // [ Memory Manipulation ]

            void resize_vec(port_t port, index_t size) {
                auto type = this->type->schema.of_struct.fields[port];
                byte_t* target = this->data + this->pos[port];             
                mem::resize_vec(type->schema.of_map.type, target, size);
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

    // [ Memory Implementations ]

    namespace mem {

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
                        mem::free(item.type, item.value);
                    }
                }

                template <typename T>
                port_t add(Type* type, T value) {
                    auto target = mem::alloc(type);
                    mem::assign(type, target, (byte_t*) &value);
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
                    mem::assign(item.type, item.value, (byte_t*) &value);
                }

        };

        // [Block]
        // A statically allocated contiguous block of memory.

        class Block : public Struct {
        
            public:
    
                Block(TypeSolver& solver, std::vector<type_t> tdxs):
                    Struct(solver.get(solver.add_struct(tdxs))) {}
    
        };

    }


}