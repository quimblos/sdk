#pragma once
#include <stdint.h>
#include <memory>
#include <bitset>
#include <iostream>

#define HEADER_QUIMBLOS 'q', 'b', 0x00, 0x00

namespace qb {

    typedef void data_t;
    typedef uint8_t code_t;
    typedef uint16_t code_addr_t;

    /**
     * Data
     */

    enum DataType {
        
        // 0 bytes
        UNKNOWN = 0x00, // Only exists during parse
        VOID = 0x01,

        PTR = 0x02,     // 2 bytes -> points to a specific dev+port
        PTR_IDX = 0x04, // 4 bytes -> points to a specific dev+port+idx

        BOOL = 0x0A,    // 1 byte

        // 1 byte
        UINT8 = 0x10,
        INT8 = 0x11,
        UINT8_ARR = 0x1A,
        INT8_ARR = 0x1B,

        // 2 bytes
        UINT16 = 0x20,
        INT16 = 0x21,
        UINT16_ARR = 0x2A,
        INT16_ARR = 0x2B,
        
        // 4 bytes
        UINT32 = 0x40,
        INT32 = 0x41,
        UINT32_ARR = 0x4A,
        INT32_ARR = 0x4B,
        
        FLOAT32 = 0x50,
        FLOAT32_ARR = 0x5A,
        
        // n bytes
        STRING_SHORT = 0xF0,    // up to 0xFF chars
        STRING = 0xFF,          // up to 0xFFFF chars
    };

    struct Data;

    struct Pointer {
        qb::Data* data;
    };

    struct IndexedPointer {
        qb::Data* data;
        uint16_t index;
    };
    
    struct Data {
        DataType type;
        data_t* value = nullptr;

        Data* ref() {
            return new Data({
                .type = this->type,
                .value = this->value
            });
        }

        void purge() {
            // std::cout << "[delete]" << std::endl;
            if (this->value == nullptr) return;
            switch (this->type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                    break;
                case DataType::PTR:
                    delete (qb::Pointer*) this->value; break;
                case DataType::PTR_IDX:
                    delete (qb::IndexedPointer*) this->value; break;
                case DataType::BOOL:
                case DataType::UINT8:
                case DataType::INT8:
                    delete (uint8_t*) this->value; break;
                case DataType::UINT8_ARR:
                case DataType::INT8_ARR:
                    delete (std::vector<uint8_t>*) this->value; break;
                case DataType::UINT16:
                case DataType::INT16:
                    delete (uint16_t*) this->value; break;
                case DataType::UINT16_ARR:
                case DataType::INT16_ARR:
                    delete (std::vector<uint16_t>*) this->value; break;
                case DataType::UINT32:
                case DataType::INT32:
                    delete (uint32_t*) this->value; break;
                case DataType::UINT32_ARR:
                case DataType::INT32_ARR:
                    delete (std::vector<uint32_t>*) this->value; break;
                case DataType::FLOAT32:
                    delete (float*) this->value; break;
                case DataType::FLOAT32_ARR:
                    delete (std::vector<float>*) this->value; break;
                case DataType::STRING_SHORT:
                case DataType::STRING:
                    delete (std::string*) this->value; break;
            }
            // this->value = nullptr;
        }

        void log() {
            switch (this->type) {
                case DataType::UNKNOWN:
                    std::cout << "{unknown}"; break;
                case DataType::VOID:
                    std::cout << "{void}"; break;
                case DataType::PTR:
                    std::cout << "&";
                    ((Pointer*)this->value)->data->log();
                    break;
                case DataType::PTR_IDX:
                    std::cout << "&";
                    ((IndexedPointer*)this->value)->data->log();
                    break;
                case DataType::BOOL:
                    std::cout << "{bool:" << (this->as_bool()) << "}"; break;
                case DataType::UINT8:
                    std::cout << "{u8:" << +(this->as_u8()) << "}"; break;
                case DataType::INT8:
                    std::cout << "{i8:" << +(this->as_i8()) << "}"; break;
                case DataType::UINT8_ARR:
                    std::cout << "{u8[]:" << "..." << "}"; break;
                case DataType::INT8_ARR:
                    std::cout << "{i8[]:" << "..." << "}"; break;
                case DataType::UINT16:
                    std::cout << "{u16:" << this->as_u16() << "}"; break;
                case DataType::INT16:
                    std::cout << "{i16:" << this->as_i16() << "}"; break;
                case DataType::UINT16_ARR:
                    std::cout << "{u16[]:" << "..." << "}"; break;
                case DataType::INT16_ARR:
                    std::cout << "{i16[]:" << "..." << "}"; break;
                case DataType::UINT32:
                    std::cout << "{u32:" << this->as_u32() << "}"; break;
                case DataType::INT32:
                    std::cout << "{i32:" << this->as_i32() << "}"; break;
                case DataType::UINT32_ARR:
                    std::cout << "{u32[]:" << "..." << "}"; break;
                case DataType::INT32_ARR:
                    std::cout << "{i32[]:" << "..." << "}"; break;
                case DataType::FLOAT32:
                    std::cout << "{f32:" << this->as_f32() << "}"; break;
                case DataType::FLOAT32_ARR:
                    std::cout << "{f32[]:" << "..." << "}"; break;
                case DataType::STRING_SHORT:
                    std::cout << "{str*:" << this->as_string() << "}"; break;
                case DataType::STRING:
                    std::cout << "{str:" << this->as_string() << "}"; break;
            }
        }

        private:

        qb::Data* resolve(qb::Data& other, qb::Data* index = nullptr) {
            if (index == nullptr) {
                if (other.type == DataType::PTR || other.type == DataType::PTR_IDX) {
                    auto ptr = (qb::IndexedPointer*) other.value;
                    return ptr->data;
                }
                if (other.type != this->type) {
                    return nullptr;
                }
            }
            else {
                // TODO
            }
            return &other;
        }

        // set

        private:

        template <typename T>
        void _set(qb::Data& other, qb::Data* index) {
            ((T*) this->value)[0] = ((T*) other.value)[0];
        }
        
        template <typename T>
        void _set_arr(qb::Data& other, qb::Data* index) {
            auto arr = ((std::vector<T>*) this->value)[0];
            auto arr_other = ((std::vector<T>*) other.value)[0];
            for (uint16_t i = 0; i < arr.size() && arr_other.size(); i++) {
                arr[i] = arr_other[i];
            }
        }

        public:
        void set(qb::Data& _other, qb::Data* index = nullptr) {
            qb::Data other = *this->resolve(_other, index);
            switch (other.type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                    break;
                case DataType::PTR:
                    ((Pointer*)this->value)->data = ((Pointer*)other.value)->data;
                    break;
                case DataType::PTR_IDX:
                    ((IndexedPointer*)this->value)->data = ((IndexedPointer*)other.value)->data;
                    ((IndexedPointer*)this->value)->index = ((IndexedPointer*)other.value)->index;
                    break;
                case DataType::BOOL:
                case DataType::UINT8:
                    this->_set<uint8_t>(other, index); break;
                case DataType::INT8:
                    this->_set<int8_t>(other, index); break;
                case DataType::UINT8_ARR:
                    this->_set_arr<uint8_t>(other, index); break;
                case DataType::INT8_ARR:
                    this->_set_arr<int8_t>(other, index); break;
                case DataType::UINT16:
                    this->_set<uint16_t>(other, index); break;
                case DataType::INT16:
                    this->_set<int16_t>(other, index); break;
                case DataType::UINT16_ARR:
                    this->_set_arr<uint16_t>(other, index); break;
                case DataType::INT16_ARR:
                    this->_set_arr<int16_t>(other, index); break;
                case DataType::UINT32:
                    this->_set<uint32_t>(other, index); break;
                case DataType::INT32:
                    this->_set<int32_t>(other, index); break;
                case DataType::UINT32_ARR:
                    this->_set_arr<uint32_t>(other, index); break;
                case DataType::INT32_ARR:
                    this->_set_arr<int32_t>(other, index); break;
                case DataType::FLOAT32:
                    this->_set<float>(other, index); break;
                case DataType::FLOAT32_ARR:
                    this->_set_arr<float>(other, index); break;
                case DataType::STRING_SHORT:
                case DataType::STRING:
                    *((std::string*) this->value) = std::string(*((std::string*) other.value));
                    break;
            }
        }

        // reset

        private:

        template <typename T>
        void _reset() {
            ((T*) this->value)[0] = 0;
        }

        template <typename T>
        void _reset_arr() {
            auto arr = ((std::vector<T>*) this->value)[0];
            for (uint16_t i = 0; i < arr.size(); i++) {
                arr[i] = 0;
            }
        }

        public:

        void reset() {
            switch (this->type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                case DataType::PTR:
                case DataType::PTR_IDX:
                    break;
                case DataType::BOOL:
                case DataType::UINT8:
                    this->_reset<uint8_t>(); break;
                case DataType::INT8:
                    this->_reset<int8_t>(); break;
                case DataType::UINT8_ARR:
                    this->_reset_arr<uint8_t>(); break;
                case DataType::INT8_ARR:
                    this->_reset_arr<int8_t>(); break;
                case DataType::UINT16:
                    this->_reset<uint16_t>(); break;
                case DataType::INT16:
                    this->_reset<int16_t>(); break;
                case DataType::UINT16_ARR:
                    this->_reset_arr<uint16_t>(); break;
                case DataType::INT16_ARR:
                    this->_reset_arr<int16_t>(); break;
                case DataType::UINT32:
                    this->_reset<uint32_t>(); break;
                case DataType::INT32:
                    this->_reset<int32_t>(); break;
                case DataType::UINT32_ARR:
                    this->_reset_arr<uint32_t>(); break;
                case DataType::INT32_ARR:
                    this->_reset_arr<int32_t>(); break;
                case DataType::FLOAT32:
                    this->_reset<float>(); break;
                case DataType::FLOAT32_ARR:
                    this->_reset_arr<float>(); break;
                case DataType::STRING_SHORT:
                case DataType::STRING:
                    *((std::string*) this->value) = std::string("");
                    break;
            }
        }

        // compare

        private:

        template <typename T>
        int8_t _compare(qb::Data& other) {
            T diff = ((T*) other.value)[0] - ((T*) this->value)[0];
            return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
        }

        public:

        int8_t compare(qb::Data& _other) {
            qb::Data other = *this->resolve(other);
            switch (other.type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                    return 0;
                case DataType::PTR:
                    // Pointer to Pointer
                    return this->_compare<uint16_t>(other);
                case DataType::PTR_IDX:
                    // Pointer to Pointer
                    return this->_compare<uint32_t>(other);
                case DataType::BOOL:
                case DataType::UINT8:
                    return this->_compare<uint8_t>(other);
                case DataType::INT8:
                    return this->_compare<int8_t>(other);
                case DataType::UINT8_ARR:
                case DataType::INT8_ARR:
                    return 0;
                case DataType::UINT16:
                    return this->_compare<uint16_t>(other);
                case DataType::INT16:
                    return this->_compare<int16_t>(other);
                case DataType::UINT16_ARR:
                case DataType::INT16_ARR:
                    return 0;
                case DataType::UINT32:
                    return this->_compare<uint32_t>(other);
                case DataType::INT32:
                    return this->_compare<int32_t>(other);
                case DataType::UINT32_ARR:
                case DataType::INT32_ARR:
                    return 0;
                case DataType::FLOAT32:
                    return this->_compare<float>(other);
                case DataType::FLOAT32_ARR:
                    return 0;
                case DataType::STRING:
                case DataType::STRING_SHORT:
                    return 0;
            }
            return 0;
        }

        // add | sub | mult | div | mod

        private:

        template <typename T>
        void _add(qb::Data& other) {
            ((T*) this->value)[0] += ((T*) other.value)[0];
        }
        template <typename T>
        void _sub(qb::Data& other) {
            ((T*) this->value)[0] -= ((T*) other.value)[0];
        }
        template <typename T>
        void _mult(qb::Data& other) {
            ((T*) this->value)[0] *= ((T*) other.value)[0];
        }
        template <typename T>
        void _div(qb::Data& other) {
            ((T*) this->value)[0] /= ((T*) other.value)[0];
        }
        template <typename T>
        void _mod(qb::Data& other) {
            ((T*) this->value)[0] %= ((T*) other.value)[0];
        }

        public:

        void add(qb::Data& _other) {
            qb::Data other = *this->resolve(other);
            switch (other.type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                case DataType::PTR:
                case DataType::PTR_IDX:
                case DataType::BOOL:
                    break;
                case DataType::UINT8:
                    this->_add<uint8_t>(other); break;
                case DataType::INT8:
                    this->_add<int8_t>(other); break;
                case DataType::UINT8_ARR:
                case DataType::INT8_ARR:
                    break;
                case DataType::UINT16:
                    this->_add<uint16_t>(other); break;
                case DataType::INT16:
                    this->_add<int16_t>(other); break;
                case DataType::UINT16_ARR:
                case DataType::INT16_ARR:
                    break;
                case DataType::UINT32:
                    this->_add<uint32_t>(other); break;
                case DataType::INT32:
                    this->_add<int32_t>(other); break;
                case DataType::UINT32_ARR:
                case DataType::INT32_ARR:
                    break;
                case DataType::FLOAT32:
                    this->_add<float>(other);
                    break;
                case DataType::FLOAT32_ARR:
                case DataType::STRING_SHORT:
                case DataType::STRING:
                    break;
            }
        }

        void sub(qb::Data& _other) {
            qb::Data other = *this->resolve(other);
            switch (this->type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                case DataType::PTR:
                case DataType::PTR_IDX:
                case DataType::BOOL:
                    break;
                case DataType::UINT8:
                    this->_sub<uint8_t>(other); break;
                case DataType::INT8:
                    this->_sub<int8_t>(other); break;
                case DataType::UINT8_ARR:
                case DataType::INT8_ARR:
                    break;
                case DataType::UINT16:
                    this->_sub<uint16_t>(other); break;
                case DataType::INT16:
                    this->_sub<int16_t>(other); break;
                case DataType::UINT16_ARR:
                case DataType::INT16_ARR:
                    break;
                case DataType::UINT32:
                    this->_sub<uint32_t>(other); break;
                case DataType::INT32:
                    this->_sub<int32_t>(other); break;
                case DataType::UINT32_ARR:
                case DataType::INT32_ARR:
                    break;
                case DataType::FLOAT32:
                    this->_sub<float>(other);
                    break;
                case DataType::FLOAT32_ARR:
                case DataType::STRING_SHORT:
                case DataType::STRING:
                    break;
            }
        }

        void mult(qb::Data& _other) {
            qb::Data other = *this->resolve(other);
            switch (this->type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                case DataType::PTR:
                case DataType::PTR_IDX:
                case DataType::BOOL:
                    break;
                case DataType::UINT8:
                    this->_mult<uint8_t>(other); break;
                case DataType::INT8:
                    this->_mult<int8_t>(other); break;
                case DataType::UINT8_ARR:
                case DataType::INT8_ARR:
                    break;
                case DataType::UINT16:
                    this->_mult<uint16_t>(other); break;
                case DataType::INT16:
                    this->_mult<int16_t>(other); break;
                case DataType::UINT16_ARR:
                case DataType::INT16_ARR:
                    break;
                case DataType::UINT32:
                    this->_mult<uint32_t>(other); break;
                case DataType::INT32:
                    this->_mult<int32_t>(other); break;
                case DataType::UINT32_ARR:
                case DataType::INT32_ARR:
                    break;
                case DataType::FLOAT32:
                    this->_mult<float>(other);
                    break;
                case DataType::FLOAT32_ARR:
                case DataType::STRING_SHORT:
                case DataType::STRING:
                    break;
            }
        }

        void div(qb::Data& _other) {
            qb::Data other = *this->resolve(other);
            switch (this->type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                case DataType::PTR:
                case DataType::PTR_IDX:
                case DataType::BOOL:
                    break;
                case DataType::UINT8:
                    this->_div<uint8_t>(other); break;
                case DataType::INT8:
                    this->_div<int8_t>(other); break;
                case DataType::UINT8_ARR:
                case DataType::INT8_ARR:
                    break;
                case DataType::UINT16:
                    this->_div<uint16_t>(other); break;
                case DataType::INT16:
                    this->_div<int16_t>(other); break;
                case DataType::UINT16_ARR:
                case DataType::INT16_ARR:
                    break;
                case DataType::UINT32:
                    this->_div<uint32_t>(other); break;
                case DataType::INT32:
                    this->_div<int32_t>(other); break;
                case DataType::UINT32_ARR:
                case DataType::INT32_ARR:
                    break;
                case DataType::FLOAT32:
                    this->_div<float>(other);
                    break;
                case DataType::FLOAT32_ARR:
                case DataType::STRING_SHORT:
                case DataType::STRING:
                    break;
            }
        }

        void mod(qb::Data& _other) {
            qb::Data other = *this->resolve(other);
            switch (this->type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                case DataType::PTR:
                case DataType::PTR_IDX:
                case DataType::BOOL:
                    break;
                case DataType::UINT8:
                    this->_mod<uint8_t>(other); break;
                case DataType::INT8:
                    this->_mod<int8_t>(other); break;
                case DataType::UINT8_ARR:
                case DataType::INT8_ARR:
                    break;
                case DataType::UINT16:
                    this->_mod<uint16_t>(other); break;
                case DataType::INT16:
                    this->_mod<int16_t>(other); break;
                case DataType::UINT16_ARR:
                case DataType::INT16_ARR:
                    break;
                case DataType::UINT32:
                    this->_mod<uint32_t>(other); break;
                case DataType::INT32:
                    this->_mod<int32_t>(other); break;
                case DataType::UINT32_ARR:
                case DataType::INT32_ARR:
                case DataType::FLOAT32:
                case DataType::FLOAT32_ARR:
                case DataType::STRING_SHORT:
                case DataType::STRING:
                    break;
            }
        }

        // initialize (when creating new variables)

        static qb::Data initialize(DataType type) {
            switch (type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                case DataType::PTR:
                case DataType::PTR_IDX:
                    return Data::_void();
                case DataType::BOOL: return Data::_bool();
                case DataType::UINT8: return Data::u8();
                case DataType::INT8: return Data::i8();
                case DataType::UINT8_ARR: return Data::u8_arr();
                case DataType::INT8_ARR: return Data::i8_arr();
                case DataType::UINT16: return Data::u16();
                case DataType::INT16: return Data::i16();
                case DataType::UINT16_ARR: return Data::u16_arr();
                case DataType::INT16_ARR: return Data::i16_arr();
                case DataType::UINT32: return Data::u32();
                case DataType::INT32: return Data::i32();
                case DataType::UINT32_ARR: return Data::u32_arr();
                case DataType::INT32_ARR: return Data::i32_arr();
                case DataType::FLOAT32: return Data::f32();
                case DataType::FLOAT32_ARR: return Data::f32_arr();
                case DataType::STRING: return Data::string();
                case DataType::STRING_SHORT: return Data::string_short();
            }
            return Data::_void();
        }

        // Creators

        private:

        template <typename T>
        static qb::Data _arr(DataType type, uint16_t length, T* values) {
            std::vector<T>* mem = new std::vector<T>(length);
            for (uint16_t i = 0; i < length; i++) {
                (*mem)[i] = values[i];
            }
            return qb::Data({ .type = type, .value = mem });
        }

        public:

        static qb::Data _void() {
            return qb::Data({ .type = DataType::VOID, .value = nullptr });
        }
        static qb::Data _bool(bool x = false) {
            uint8_t* mem = new uint8_t;
            mem[0] = x ? 0x01 : 0x00;
            return qb::Data({ .type = DataType::BOOL, .value = mem });
        }

        static qb::Data ptr(qb::Data* data) {
            Pointer* mem = new Pointer();
            mem->data = data;
            return qb::Data({ .type = DataType::PTR, .value = mem });
        }

        static qb::Data ptr_idx(qb::Data* data, uint16_t index) {
            IndexedPointer* mem = new IndexedPointer();
            mem->data = data;
            mem->index = index;
            return qb::Data({ .type = DataType::PTR_IDX, .value = mem });
        }

        //Creators / 1 byte

        static qb::Data u8(uint8_t x = 0x0) {
            uint8_t* mem = new uint8_t;
            mem[0] = x;
            return qb::Data({ .type = DataType::UINT8, .value = mem });
        }

        static qb::Data i8(int8_t x = 0x0) {
            int8_t* mem = new int8_t;
            mem[0] = x;
            return qb::Data({ .type = DataType::INT8, .value = mem });
        }

        static qb::Data u8_arr(uint16_t length = 0, uint8_t* arr = nullptr) {
            return qb::Data::_arr<uint8_t>(DataType::UINT8, length, arr);
        }
        
        static qb::Data i8_arr(uint16_t length = 0, int8_t* arr = nullptr) {
            return qb::Data::_arr<int8_t>(DataType::INT8, length, arr);
        }

        // Creators / 2 bytes

        static qb::Data u16(uint16_t x = 0x0) {
            uint16_t* mem = new uint16_t;
            mem[0] = x;
            return qb::Data({ .type = DataType::UINT16, .value = mem });
        }

        static qb::Data i16(int16_t x = 0x0) {
            int16_t* mem = new int16_t;
            mem[0] = x;
            return qb::Data({ .type = DataType::INT16, .value = mem });
        }

        
        static qb::Data u16_arr(uint16_t length = 0, uint16_t* arr = nullptr) {
            return qb::Data::_arr<uint16_t>(DataType::UINT16, length, arr);
        }
        
        static qb::Data i16_arr(uint16_t length = 0, int16_t* arr = nullptr) {
            return qb::Data::_arr<int16_t>(DataType::INT16, length, arr);
        }

        // Creators / 4 bytes

        static qb::Data u32(uint32_t x = 0x0) {
            uint32_t* mem = new uint32_t;
            mem[0] = x;
            return qb::Data({ .type = DataType::UINT32, .value = mem });
        }

        static qb::Data i32(int32_t x = 0x0) {
            int32_t* mem = new int32_t;
            mem[0] = x;
            return qb::Data({ .type = DataType::INT32, .value = mem });
        }

        static qb::Data u32_arr(uint16_t length = 0, uint32_t* arr = nullptr) {
            return qb::Data::_arr<uint32_t>(DataType::UINT32, length, arr);
        }
        
        static qb::Data i32_arr(uint16_t length = 0, int32_t* arr = nullptr) {
            return qb::Data::_arr<int32_t>(DataType::INT32, length, arr);
        }

        static qb::Data f32(float x = 0x0) {
            float* mem = new float;
            mem[0] = x;
            return qb::Data({ .type = DataType::FLOAT32, .value = mem });
        }

        static qb::Data f32_arr(uint16_t length = 0, float* arr = nullptr) {
            return qb::Data::_arr<float>(DataType::FLOAT32, length, arr);
        }

        static qb::Data string_short(uint8_t length= 0, const char* bytes = nullptr) {
            std::string* mem = new std::string(bytes, length);
            return qb::Data({ .type = DataType::STRING_SHORT, .value = mem });
        }

        static qb::Data string(uint16_t length= 0, const char* bytes = nullptr) {
            std::string* mem = new std::string(bytes, length);
            return qb::Data({ .type = DataType::STRING, .value = mem });
        }

    // parse (when reading from bytecode)

        static qb::Data parse(DataType type, uint8_t* bytes) {
            switch (type) {
                case DataType::UNKNOWN:
                case DataType::VOID:
                    break;
                case DataType::PTR:
                    // TODO
                    // return qb::Data::ptr(bytes[0], bytes[1]);
                    break;
                case DataType::PTR_IDX:
                    // TODO
                    // return qb::Data::ptr_idx(bytes[0], bytes[1], (bytes[2] << 8) + bytes[3]);
                    break;
                case DataType::BOOL:
                    return qb::Data::_bool(bytes[0]);
                case DataType::UINT8:
                    return qb::Data::u8(bytes[0]);
                case DataType::INT8:
                    return qb::Data::i8(bytes[0]);
                case DataType::UINT16:
                    return qb::Data::u16((bytes[0] << 8) + bytes[1]);
                case DataType::INT16:
                    return qb::Data::i16((bytes[0] << 8) + bytes[1]);
                case DataType::UINT32:
                    return qb::Data::u32((bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3]);
                case DataType::INT32:
                    return qb::Data::i32((bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3]);
                case DataType::FLOAT32:
                    return qb::Data::f32((bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3]);
                case DataType::STRING_SHORT:
                    return qb::Data::string_short(bytes[0], (const char*)(bytes+1));
                case DataType::STRING:
                    return qb::Data::string(*((uint16_t*)bytes), (const char*)(bytes+2));
                default:
                    break;
            }      
            return qb::Data::_void();      
        }

        static qb::Data parse_arr(DataType type, uint16_t length, uint8_t* bytes) {
            
            uint8_t isize;
            switch (type) {
                case DataType::UINT8_ARR:
                case DataType::INT8_ARR:
                    isize = 1; break;
                case DataType::UINT16_ARR:
                case DataType::INT16_ARR:
                    isize = 2; break;
                case DataType::UINT32_ARR:
                case DataType::INT32_ARR:
                case DataType::FLOAT32_ARR:
                    isize = 4; break;
                default:
                    break;
            }
            uint8_t* values = new uint8_t[length*isize];

            for (uint16_t i = 0; i < length; i++) {
                switch (type) {
                    case DataType::UINT8_ARR:
                    case DataType::INT8_ARR:
                        values[i] = bytes[i]; break;
                    case DataType::UINT16_ARR:
                    case DataType::INT16_ARR:
                        ((uint16_t*)values)[i] = (bytes[i] << 8) + bytes[i+1]; break;
                    case DataType::UINT32_ARR:
                    case DataType::INT32_ARR:
                    case DataType::FLOAT32_ARR:
                        ((uint32_t*)values)[i] = (bytes[i] << 24) + (bytes[i+1] << 16) + (bytes[i+2] << 8) + bytes[i+3];
                    default:
                        break;
                }
            }

            qb::Data data;
            switch (type) {
                case DataType::UINT8_ARR: {
                    data = qb::Data::_arr<uint8_t>(type, length, values); break;
                }
                case DataType::INT8_ARR: {
                    data = qb::Data::_arr<int8_t>(type, length, (int8_t*) values); break;
                }
                case DataType::UINT16_ARR: {
                    data = qb::Data::_arr<uint16_t>(type, length, (uint16_t*) values); break;
                }
                case DataType::INT16_ARR: {
                    data = qb::Data::_arr<int16_t>(type, length, (int16_t*) values); break;
                }
                case DataType::UINT32_ARR: {
                    data = qb::Data::_arr<uint32_t>(type, length, (uint32_t*) values); break;
                }
                case DataType::INT32_ARR: {
                    data = qb::Data::_arr<int32_t>(type, length, (int32_t*) values); break;
                }
                case DataType::FLOAT32_ARR: {
                    data = qb::Data::_arr<float>(type, length, (float*) values); break;
                }
                default:
                    break;
            }
            delete[] values;
            return data;
        }


        // Cast

        const Pointer& as_ptr() const {
            return *((Pointer*) this->value);
        }
        const IndexedPointer& as_ptr_idx() const {
            return *((IndexedPointer*) this->value);
        }
        const bool as_bool() const {
            return *((bool*) this->value);
        }
        const uint8_t as_u8() const {
            return *((uint8_t*) this->value);
        }
        const int8_t as_i8() const {
            return *((int8_t*) this->value);
        }
        const std::vector<uint8_t>& as_u8_arr() const {
            return *((std::vector<uint8_t>*) this->value);
        }
        const std::vector<uint8_t>& as_i8_arr() const {
            return *((std::vector<uint8_t>*) this->value);
        }
        const uint16_t as_u16() const {
            return *((uint16_t*) this->value);
        }
        const int16_t as_i16() const {
            return *((int16_t*) this->value);
        }
        const std::vector<uint16_t>& as_u16_arr() const {
            return *((std::vector<uint16_t>*) this->value);
        }
        const std::vector<uint16_t>& as_i16_arr() const {
            return *((std::vector<uint16_t>*) this->value);
        }
        const uint32_t as_u32() const {
            return *((uint32_t*) this->value);
        }
        const int32_t as_i32() const {
            return *((int32_t*) this->value);
        }
        const std::vector<uint32_t>& as_u32_arr() const {
            return *((std::vector<uint32_t>*) this->value);
        }
        const std::vector<uint32_t>& as_i32_arr() const {
            return *((std::vector<uint32_t>*) this->value);
        }
        const float as_f32() const {
            return *((float*) this->value);
        }
        const std::vector<float>& as_f32_arr() const {
            return *((std::vector<float>*) this->value);
        }
        const std::string& as_string() {
            return *((std::string*) this->value);
        }
    };
    
}