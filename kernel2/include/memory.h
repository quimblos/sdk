#pragma once
#include <vector>
#include <sstream>
#include "typeblock.h"

/*
    [Quimblos Memory Manager]
    Encapsulates memory management through quimblos types.
*/

namespace qb {
    
    enum EventLevel {
        TRACE = 0x00,
        DEBUG = 0x01,
        INFO = 0x02,
        WARN = 0x03,
        ERROR = 0x04,
    };

    // [ Memory Manipulation Methods ]

    namespace mem {

        // [ Memory Allocation ]

        // Blocks are statically allocated on construction
        // based on the sizes below.

        uint8_t size_of(const Type* type);

        // Dynamic allocation

        data_t alloc(const Type* type);

        // [ Value Manipulation ]

        // Initialize a memory address with the proper
        // C++ type for a given qb::Type.
          
        void init(const Type* type, data_t target);
        
        // Copy values to pre-allocated memory
        // The memory MUST have been previously
        // allocated (statically or dynamically).

        void copy(const Type* type, data_t target, data_t source);

        // [ Memory Manipulation ]

        // Free C++ allocated resources for a given address
        // of a given type.
        // This MUST be called for each data when destroying
        // the Block to avoid memory leaks.
        
        void free(const Type* type, data_t target);
    }

    // [ Data Structures ]

    namespace mem {

        struct Reference {
            block_t block = 0;
            port_t port = 0;
    
            Reference() {}

            Reference(block_t block, port_t port) {
                this->block = block;
                this->port = port;
            }

            void assign(Reference* other) {
                this->block = other->block;
                this->port = other->port;
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << '<';
                switch (this->block) {
                    case BLOCK_ENGINE:
                        ss << "engine.";
                        switch (this->port) {
                            case PORT_CONST_FALSE:
                                ss << "false";
                                break;
                            case PORT_CONST_TRUE:
                                ss << "true";
                                break;
                        }
                        break;
                    case BLOCK_NODE:
                        ss << "node." << +port;
                        break;
                    case BLOCK_THREAD:
                        ss << "const." << +port;
                        break;
                    case BLOCK_METHOD:
                        ss << "var." << +port;
                        break;
                }
                ss << '>';
                return ss.str();
            }

            Reference* copy() const {
                return new Reference(this->block, this->port);
            }
        };

        struct SlicedReference: public Reference {
            struct Slice {
                index_t start = 0;
                index_t end = 0;
            };

            uint8_t dims = 0;
            std::vector<Slice> shape;
    
            SlicedReference() {}

            SlicedReference(block_t block, port_t port, uint8_t dims, const std::vector<Slice>& shape) {
                this->block = block;
                this->port = port;
                this->dims = dims;
                this->shape = shape;
            }

            void assign(SlicedReference* other) {
                this->block = other->block;
                this->port = other->port;
                this->dims = other->dims;
                this->shape = other->shape;
            }

            void set(uint8_t i, index_t value) {
                if (i%2 == 0) {
                    this->shape[i/2].start = value;
                }
                else {
                    this->shape[i/2].end = value;
                }
            }


            const std::string to_str() const {
                std::stringstream ss;
                ss << Reference::to_str();
                for (uint8_t i = 0; i < this->dims; i++) {
                    ss << '[' << this->shape[i].start << ':' << this->shape[i].end << ']';
                }
                return ss.str();
            }
        };

        struct Event {
            const Type* type = nullptr;
            EventLevel level : 8 = EventLevel::INFO;
            byte_t code = 0x00;
            data_t data = nullptr;

            Event(const Type* type): type(type) {
                this->data = mem::alloc(type);
                mem::init(type, this->data);
            }

            ~Event() {
                if (this->data != nullptr) {
                    mem::free(this->type, this->data);
                    delete[] this->data;
                }
            }

            void assign(Event* other) {
                this->level = other->level;
                this->code = other->code;
                mem::copy(type, this->data, other->data);
            }

            void set(EventLevel level, byte_t code, data_t value) {
                this->level = level;
                this->code = code;
                mem::copy(this->type, this->data, value);
            }

            template <typename T>
            void __cpp_set(EventLevel level, byte_t code, T value) {
                this->level = level;
                this->code = code;
                mem::copy(this->type, this->data, (data_t) &value);
            }

            template <typename T>
            T* __cpp_get() {
                return (T*) this->data;
            }
        };

        struct Vector {
            const Type* item_type;
            const uint8_t item_size;
            std::vector<index_t> shape;
            std::vector<byte_t> data;
            
            Vector(const Type* item_type, const std::vector<index_t>& shape = {}):
                item_type(item_type),
                shape(shape),
                item_size(size_of(item_type))
            {
                index_t size = this->shape.size() ? 1 : 0;
                for (uint8_t i = 0; i < this->shape.size(); i++) {
                    size *= this->shape[i];
                }
                this->data = std::vector<byte_t>(size * this->item_size);
                if (size > 0) {
                    data_t start = &this->data.front();
                    for (index_t i = 0; i < size; i++) {
                        mem::init(this->item_type, start+i*this->item_size);
                    }
                }
            }
            
            ~Vector() {
                index_t size = this->data.size() / this->item_size;
                if (size > 0) {
                    data_t start = &this->data.front();
                    for (index_t i = 0; i < size; i++) {
                        mem::free(this->item_type, start+i*this->item_size);
                    }
                }
            }

            void resize(const std::vector<index_t>& shape) {
                index_t size = shape.size() ? 1 : 0;
                for (uint8_t i = 0; i < shape.size(); i++) {
                    size *= shape[i];
                }
                
                this->shape.resize(size);
                for (uint8_t i = 0; i < shape.size(); i++) {
                    this->shape[i] = shape[i];
                }

                auto old_size = this->data.size();
                if (size == old_size) return;

                if (size < old_size) {
                    data_t start = &this->data.front();
                    for (index_t i = size; i < old_size; i++) {
                        mem::free(this->item_type, start+i*this->item_size);
                    }
                }
                this->data.resize(size * this->item_size);
                
                if (size > old_size) {
                    data_t start = &this->data.front();
                    for (index_t i = old_size; i < size; i++) {
                        mem::init(this->item_type, start+i*this->item_size);
                    }
                }
            }

            index_t size() {
                return this->data.size() / this->item_size;
            }

            data_t at(index_t index) {
                return &this->data.front() + index * this->item_size;
            }

            template <typename T>
            T* __cpp_get(index_t index) {
                return (T*) (&this->data.front() + index * this->item_size);
            }

            template <typename T>
            void __cpp_set(index_t index, T value) {
                auto ptr = &this->data.front() + index * this->item_size;
                mem::copy(this->item_type, ptr, (data_t) &value);
            }
        };

        struct Map {
            const Type* item_type;
            const uint8_t item_size;
            std::map<std::string, data_t> data;
            
            Map(const Type* item_type):
                item_type(item_type),
                item_size(size_of(item_type))
            {}

            ~Map() {
                for (const auto& it : this->data) {
                    mem::free(this->item_type, it.second);
                    delete[] it.second;
                }
            }

            index_t size() {
                return this->data.size();
            }

            data_t at(std::string key) {
                if (!this->data.contains(key)) return nullptr;
                return this->data.at(key);
            }

            template <typename T>
            T* __cpp_get(std::string key) {
                if (!this->data.contains(key)) return nullptr;
                return (T*) this->data.at(key);
            }

            template <typename T>
            void __cpp_set(std::string key, T value) {
                this->data.insert({key, new byte_t[this->item_size]});
                auto ptr = this->at(key);
                mem::init(this->item_type, ptr);
                mem::copy(this->item_type, ptr, (data_t) &value);
            }
        };

        struct Struct {
            const Type* type;
            std::vector<index_t> pos;
            data_t data = nullptr;

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

            void set(port_t port, data_t value) {                
                auto type = this->type->schema.of_struct.fields[port];
                data_t target = this->data + this->pos[port];
                mem::copy(type, target, value);
            }

            template <typename T>
            void __cpp_set(port_t port, T value) {                
                auto type = this->type->schema.of_struct.fields[port];
                data_t target = this->data + this->pos[port];
                mem::copy(type, target, (data_t) &value);
            }
                
            void clear() {
                port_t ports = this->type->schema.of_struct.n_fields;
                for (port_t port = 0; port < ports; port++) {
                    auto target = this->data + this->pos[port];
                    auto type = this->type->schema.of_struct.fields[port];
                    mem::free(type, target);
                    mem::init(type, target);
                }
            }

            // [ Read ]

            data_t get(port_t port) {
                return this->data + this->pos[port];
            }

            template <typename T>
            T* __cpp_get(port_t port) {
                return (T*)(this->data + this->pos[port]);
            }

        };

        struct Code;
        struct Function {
            const Code* code;
        };

    }

    // [ Memory Implementations ]

    namespace mem {

        // [Pool]
        // A dinamycally allocated set of values.

        class Pool {
            struct Item {
                Type* type;
                data_t value;
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
                    mem::copy(type, target, (data_t) &value);
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
                    mem::copy(item.type, item.value, (data_t) &value);
                }

        };

        // [Block]
        // A statically allocated contiguous block of memory.

        class Block {
        
            public:
                TypeBlock types;
                Struct data;
                                
                Block(const TypeDef& type_def)
                    : data(this->types.get(this->types.add_from_def(type_def))) {}
    
                const Type* type_of(port_t port) const {
                    return this->data.type->schema.of_struct.fields[port];
                }

                void clear() {
                    this->data.clear();
                }
        };

    }


}