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

    namespace data {

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

            // Reference(Reference& other) {
            //     this->assign(&other);
            // }
        
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

        // struct Map {
            
    
        //     // Map() {}

        //     // Map(const Type* type, std::map<std::string, byte_t*> data) {
        //     //     this->type_size = size_of(type);
        //     //     this->size = data.size();
        //     //     this->keys = new std::string [this->size];
        //     //     this->values = new byte_t[this->size * this->type_size];

        //     //     index_t i = 0;
        //     //     for (const auto& it : data) {
        //     //         this->keys[i].assign(it.first);
        //     //         data::assign(type, this->values + i*this->type_size, it.second);
        //     //     }
        //     // }
            
        //     // // Map(Map& other) {
        //     // //     this->assign(&other);
        //     // // }
        
        //     // ~Map() {
        //     //     delete[] this->keys;
        //     //     delete[] this->values;
        //     // }

        //     // void assign(Map* other) {
        //     //     delete[] this->keys;
        //     //     delete[] this->values;

        //     //     this->type_size = other->type_size;
        //     //     this->size = other->size;
        //     //     this->keys = new std::string [this->size];
        //     //     this->values = new uint8_t[other->size * other->type_size];

        //     //     memcpy(this->keys, other->keys, other->size * other->type_size);
        //     //     memcpy(this->values, other->values, other->size * other->type_size);
        //     // }

        //     // std::string to_str() const {
        //     //     std::stringstream ss;
        //     //     ss << '{';
        //     //     ss << "...";
        //     //     // for (index_t i = 0; i < this->size; i++) {
        //     //     //     ss << this->keys[i] << ":" << to_str(this->values + i*this->type_size) <<
        //     //     // }
        //     //     ss << '}';
        //     //     return ss.str();
        //     // }
        // };

    }

    namespace data {

        uint8_t size_of(const Type* type) {
            switch (type->kind) {
                case qb::TypeKind::VOID: return sizeof(void_t);
                case qb::TypeKind::BOOL: return sizeof(bool);
                case qb::TypeKind::INT: return type->flags.of_int.res * sizeof(uint8_t);
                case qb::TypeKind::FLOAT: return sizeof(float);
                case qb::TypeKind::STRING: return sizeof(std::string);
                case qb::TypeKind::OBJ: {
                    if (type->flags.of_obj.is_map)
                        return sizeof(std::map<std::string, void*>);
                    else
                        return sizeof(void*);
                }
                case qb::TypeKind::EVENT: return sizeof(void*);
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
                        //         // data::Map::init(data, this->types[port]->schema.of_map.type);
                        //     }
                        //     // auto map = std::map<std::string, bool>();
                        //     // memcpy((std::map<std::string, bool>*) data, &map, sizeof(map));
                        //     break;
                        // }
                        // case qb::TypeKind::EVENT: break;
                        // case qb::TypeKind::VECTOR: break;
                    }
                }
                case qb::TypeKind::REF: return sizeof(data::Reference);
            }
        }
          
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
                //         // data::Map::init(data, this->types[port]->schema.of_map.type);
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
                //         // data::Map::init(data, this->types[port]->schema.of_map.type);
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
                    new (target) data::Reference();
                    break;
                case qb::TypeKind::OBJ: {
                    if (type->flags.of_obj.is_map) {
                        init_map(type->schema.of_map.type, target);
                    }
                    break;
                }
                case qb::TypeKind::EVENT:
                    break;
                case qb::TypeKind::VECTOR:
                    init_vec(type->schema.of_map.type, target);
                    break;
            }
        }
        
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
                //         ((data::Map*) target)->assign((data::Map*) source);
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
                //         ((data::Map*) target)->assign((data::Map*) source);
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
                    ((data::Reference*) target)->assign((data::Reference*) source);
                    break;
                case qb::TypeKind::OBJ:
                case qb::TypeKind::EVENT:
                case qb::TypeKind::VECTOR:
                    // Cannot be assigned directly (for now)
                    break;
            }
        }
        
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
                //         // data::Map::init(data, this->types[port]->schema.of_map.type);
                //     }
                //     // auto map = std::map<std::string, bool>();
                //     // memcpy((std::map<std::string, bool>*) data, &map, sizeof(map));
                //     break;
                // }
                // case qb::TypeKind::EVENT: break;
                // case qb::TypeKind::VECTOR: break;
            }
        }

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
                //     // ((data::String*) (this->data + offset))->~String();
                //     break;
                // }
                // case qb::TypeKind::OBJ: {
                //     if (type->flags.of_obj.is_map) {
                //         ((data::Map*) (this->data + offset))->~Map();
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
                //     // ((data::String*) (this->data + offset))->~String();
                //     break;
                // }
                // case qb::TypeKind::OBJ: {
                //     if (type->flags.of_obj.is_map) {
                //         ((data::Map*) (this->data + offset))->~Map();
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
                    // ((data::String*) (this->data + offset))->~String();
                    break;
                }
                case qb::TypeKind::REF: {
                    ((data::Reference*) target)->~Reference();
                    break;
                }
                case qb::TypeKind::OBJ: {
                    if (type->flags.of_obj.is_map) {
                        free_map(type->schema.of_map.type, target);
                    }
                    break;
                }
                case qb::TypeKind::EVENT: break;
                case qb::TypeKind::VECTOR: 
                    free_vec(type->schema.of_map.type, target);
                    break;
            }
        }
    }

    class DataBlock {

        std::vector<const Type*> types;
        std::vector<uint16_t> offsets;
        byte_t* data;

        public:

            DataBlock(TypeChecker& checker, std::vector<type_t> tdxs):
                types(std::vector<const Type*>(tdxs.size())),
                offsets(std::vector<uint16_t>(tdxs.size()))
            {
                uint16_t offset = 0;
                for (uint16_t i = 0; i < tdxs.size(); i++) {
                    this->types[i] = checker.get(tdxs[i]);
                    this->offsets[i] = offset;
                    offset += data::size_of(this->types[i]);
                }

                this->data = new byte_t[offset];
                for (uint16_t i = 0; i < offset; i++) {
                    this->data[i] = 0;
                }

                this->init();
            }

            void init() {
                uint16_t ports = this->types.size();
                for (uint16_t port = 0; port < ports; port++) {
                    auto target = this->data + this->offsets[port];
                    auto type = this->types[port];
                    data::init(type, target);
                }
            }

            ~DataBlock() {
                for (uint16_t i = 0; i < this->types.size(); i++) {
                    auto target = this->data + this->offsets[i];
                    auto type = this->types[i];
                    data::free(type, target);
                }
                delete[] this->data;
            }

            template <typename T>
            void set(port_t port, T value) {                
                auto type = this->types[port];
                byte_t* target = this->data + this->offsets[port];
                data::assign(type, target, (byte_t*) &value);
            }

            template <typename T>
            void set_map(port_t port, std::string key, T value) {                
                auto type = this->types[port];
                byte_t* target = this->data + this->offsets[port];
                data::assign_map(type->schema.of_map.type, target, key, (byte_t*) &value);
            }

            template <typename T>
            void set_vec(port_t port, index_t index, T value) {                
                auto type = this->types[port];
                byte_t* target = this->data + this->offsets[port];
                data::assign_vec(type->schema.of_map.type, target, index, (byte_t*) &value);
            }

            void resize_vec(port_t port, index_t size) {
                auto type = this->types[port];
                byte_t* target = this->data + this->offsets[port];             
                data::resize_vec(type->schema.of_map.type, target, size);
            }

            template <typename T>
            T* get(port_t port) {
                return (T*)(this->data + this->offsets[port]);
            }

            template <typename T>
            std::map<std::string, T>* get_map(port_t port) {
                return (std::map<std::string, T>*)(this->data + this->offsets[port]);
            }

            template <typename T>
            std::vector<T>* get_vec(port_t port) {
                return (std::vector<T>*)(this->data + this->offsets[port]);
            }

    };


}