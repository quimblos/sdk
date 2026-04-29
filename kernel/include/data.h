#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "types.h"
#include "float.h"

namespace qb {

    /*
        Type Codes
    */

    enum DataType {
        // Void
        VOID = 0x00,
        _NULL = 0x01,
        ERROR = 0x02,

        // Numerics
        BOOL = 0x0A,    // 1 byte
        UINT8 = 0x10,
        INT8 = 0x11,
        UINT16 = 0x20,
        INT16 = 0x21,
        UINT32 = 0x40,
        INT32 = 0x41,
        FLOAT32 = 0x4A,

        // Strings
        STRING = 0xA0,          // + 0xLEN 0xLEN
        STRING_SHORT = 0xA1,    // + 0xLEN        (converted to STRING on parse, used to reduce payload)

        // Vectors
        VECTOR = 0xB0,           // 

        // References
        REF = 0xF0,             // + 0xDEVICE 0xPORT (0xINDEX 0xINDEX) - if deref device
    };

    /*
        Response
    */

    struct Data;
    namespace data {
        struct res_t {
            uint8_t code;
            Data* data;
            code_addr_t next_addr;
        };
    }

    /*
        Base Data struct
    */

    struct Data {
        // A byte representing the static type of this data
        type_t type;

        Data(type_t type): type(type) {}
        virtual ~Data() {};

        // Copies the data and it's contained value
        // (On refs doesn't copy referenced values, only the reference)
        virtual Data* copy() const = 0;

        // Clears the memory stored for this data
        virtual void clear() {};

        // Returns a string representation of the value
        // (Used for debugging)
        virtual const std::string to_str() const = 0;

        // Builds a data from a byte sequence
        static data::res_t parse(const code_t* bytes, code_addr_t code_len, code_addr_t addr);

        // Builds a data from a given type and a byte sequence
        static data::res_t make(type_t type, const code_t* bytes, code_addr_t code_len, code_addr_t addr);
    };

    /*
        Type Implementations
    */

    namespace data {

        struct Null: public Data {
            Null():
                Data(DataType::_NULL)
            {}
            
            Null(uint8_t from_int):
                Data(DataType::_NULL)
            {}

            Null* copy() const {
                return new Null();
            }

            const std::string to_str() const {
                return "<void>";
            };
        };

        struct Error: public Data {
            uint8_t code;
            std::string message;

            Error():
                Data(DataType::ERROR), code(0) {}

            Error(uint8_t code):
                Data(DataType::ERROR), code(code) {}

            Error(uint8_t code, index_t length, const char* bytes):
                Data(DataType::ERROR), code(code) {
                this->message = std::string(bytes, length);
            }

            Error(uint8_t code, std::string message):
                Data(DataType::ERROR), code(code), message(message) {}

            Error* copy() const {
                return new Error(this->code, this->message.size(), this->message.c_str());
            }

            void clear() {
                this->code = 0;
                this->message = "";
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<error." << +this->code << ":" << this->message << ">";
                return ss.str();
            };
        };

        template <typename T>
        struct Numeric: public Data {
            T value = (T) 0;

            Numeric(type_t type, T value):
                Data(type), value(value) {}

            Numeric<T>* copy() const {
                return new Numeric<T>(this->type, this->value);
            }

            void clear() {
                this->value = 0;
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<";
                switch (this->type) {
                    case DataType::BOOL: ss << "bool:" << (this->value ? "true" : "false"); break;
                    case DataType::UINT8: ss << "u8:" << +(this->value); break;
                    case DataType::INT8: ss << "i8:" << +(this->value); break;
                    case DataType::UINT16: ss << "u16:" << this->value; break;
                    case DataType::INT16: ss << "i16:" << this->value; break;
                    case DataType::UINT32: ss << "u32:" << this->value; break;
                    case DataType::INT32: ss << "i32:" << this->value; break;
                    case DataType::FLOAT32: ss << "f32:" << this->value; break;
                }
                ss << ">";
                return ss.str();
            };
        };

        struct String: public Data {
            std::string value;

            String():
                Data(DataType::STRING) {}

            String(uint8_t from_int):
                Data(DataType::STRING) {}

            String(index_t length, const char* bytes):
                Data(DataType::STRING)
            {
                this->value = std::string(bytes, length);
            }

            String(std::string value):
                Data(DataType::ERROR), value(value) {}

            String* copy() const {
                return new String(this->value.size(), this->value.c_str());
            }

            void clear() {
                this->value = "";
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<str:" << this->value << ">";
                return ss.str();
            };
        };

        struct Slice {
            uint8_t dims = 0;
            index_t* start = nullptr;
            index_t* end = nullptr;

            Slice(uint8_t dims) {
                this->dims = dims;
                this->start = new index_t[dims];
                this->end = new index_t[dims];
            }

            ~Slice() {
                delete[] this->start;
                delete[] this->end;
            }

            static Slice* from_vec(slice_t vec) {
                auto slice = new Slice(vec.size());
                for (uint8_t i = 0; i < slice->dims; i++) {
                    slice->start[i] = vec[i].first;
                    slice->end[i] = vec[i].second;
                }
                return slice;
            }

            Slice* copy() {
                auto slice = new Slice(this->dims);
                for (uint8_t i = 0; i < slice->dims; i++) {
                    slice->start[i] = this->start[i];
                    slice->end[i] = this->end[i];
                }
                return slice;
            }
        };

        struct Reference: public Data {
            device_t device = 0xFF;
            port_t port = 0;
            bool deref = false;
            Slice* slice = nullptr;

            Reference():
                Data(DataType::REF) {}

            Reference(uint8_t port):
                Data(DataType::REF), port(port) {}

            Reference(device_t device, port_t port, bool deref, Slice* slice):
                Data(DataType::REF), device(device), port(port), deref(deref), slice(slice) {}

            Reference* copy() const {
                Slice* slice = this->slice != nullptr ? this->slice->copy() : nullptr;
                return new Reference(this->deref, this->device, this->port, slice);
            }

            ~Reference() {
                if (this->slice != nullptr) {
                    delete this->slice;
                }
            }

            void clear() {
                this->deref = false;
                this->device = 0;
                this->port = 0;
                this->slice = nullptr;
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<";
                if (this->deref) {
                    ss << "*";
                }
                ss << "ref:" << +(this->device) << "#" << +(this->port);
                if (this->slice != nullptr) {
                    ss << "[";
                    for (uint8_t i = 0; i < this->slice->dims; i++) {
                        ss << this->slice->start[i] << ":" << this->slice->end[i] << (i < this->slice->dims-1 ? "," : "");
                    }
                    ss << "]";
                }
                ss << ">";
                return ss.str();
            };
        };

        template <typename T>
        struct Vector: public Data {
            type_t item_type;
            uint8_t dims = 0;
            index_t* shape = nullptr;
            index_t size = 0;
            T* items = nullptr;

            Vector(type_t item_type, uint8_t dims, index_t* shape):
                Data(DataType::VECTOR), item_type(item_type), dims(dims) {
                this->shape = new index_t[dims];
                for (uint8_t i = 0; i < dims; i++) {
                    this->shape[i] = shape[i];
                }

                this->size = 1;
                for (uint8_t i = 0; i < dims; i++) {
                    this->size *= shape[i];
                }

                this->items = new T[this->size];
                this->clear();
            }

            ~Vector() {
                delete[] this->shape;
                delete[] this->items;
            }

            Vector<T>* copy() const {
                auto copy = new Vector<T>(this->item_type, this->dims, this->shape);
                for (uint16_t i = 0; i < this->size; i++) {
                    copy->items[i] = this->items[i];
                }
                return copy;
            }

            void clear() {
                for (uint16_t i = 0; i < this->size; i++) {
                    this->items[i] = (T) 0;
                }
            }

            void* ptr_at(index_t index) {
                switch (this->item_type) {
                    case DataType::_NULL: return nullptr;
                    case DataType::ERROR: return ((Error*)this->items) + index;
                    case DataType::BOOL: return ((bool*) this->items) + index;
                    case DataType::UINT8: return ((uint8_t*)this->items) + index;
                    case DataType::INT8: return ((int8_t*)this->items) + index;
                    case DataType::UINT16: return ((uint16_t*)this->items) + index;
                    case DataType::INT16: return ((int16_t*)this->items) + index;
                    case DataType::UINT32: return ((uint32_t*)this->items) + index;
                    case DataType::INT32: return ((int32_t*)this->items) + index;
                    case DataType::FLOAT32: return ((float*)this->items) + index;
                    case DataType::STRING: return ((std::string*)this->items) + index;
                    case DataType::VECTOR: return ((Vector*)this->items) + index;
                    case DataType::REF: return ((Reference*)this->items) + index;
                    default: return nullptr;
                }
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<";
                switch (this->item_type) {
                    case DataType::_NULL: ss << "void"; break;
                    case DataType::ERROR: ss << "error"; break;
                    case DataType::BOOL: ss << "bool"; break;
                    case DataType::UINT8: ss << "u8"; break;
                    case DataType::INT8: ss << "i8"; break;
                    case DataType::UINT16: ss << "u16"; break;
                    case DataType::INT16: ss << "i16"; break;
                    case DataType::UINT32: ss << "u32"; break;
                    case DataType::INT32: ss << "i32"; break;
                    case DataType::FLOAT32: ss << "f32"; break;
                    case DataType::STRING: ss << "str"; break;
                    case DataType::VECTOR: ss << "vec"; break;
                    case DataType::REF: ss << "ref"; break;
                }
                ss << "[" << this->size << "]:";
                for (uint16_t i = 0; i < this->size; i++) {
                    switch (this->item_type) {
                        case DataType::_NULL: ss << "void"; break;
                        case DataType::ERROR: ss << ((Error*)this->items)[i].message; break;
                        case DataType::BOOL: ss << (((bool*) this->items)[i] ? "true" : "false"); break;
                        case DataType::UINT8: ss << +(((uint8_t*)this->items)[i]); break;
                        case DataType::INT8: ss << +((int8_t*)this->items)[i]; break;
                        case DataType::UINT16: ss << ((uint16_t*)this->items)[i]; break;
                        case DataType::INT16: ss << ((int16_t*)this->items)[i]; break;
                        case DataType::UINT32: ss << ((uint32_t*)this->items)[i]; break;
                        case DataType::INT32: ss << ((int32_t*)this->items)[i]; break;
                        case DataType::FLOAT32: ss << ((float*)this->items)[i]; break;
                        case DataType::STRING: ss << ((std::string*)this->items)[i]; break;
                        case DataType::VECTOR: ss << ((Vector*)this->items)[i].to_str(); break;
                        case DataType::REF: ss << ((Reference*)this->items)[i].to_str(); break;
                    }
                    if (i < this->size - 1) ss << ',';
                }
                ss << ">";
                return ss.str();
            };
        };
    }

    /*
        Data Factories
    */

    namespace data {
        Null* _null();
        Error* error(uint8_t code, std::string message);
        Numeric<bool>* _bool(bool val = 0);
        Numeric<uint8_t>* u8(uint8_t val = 0);
        Numeric<int8_t>* i8(int8_t val = 0);
        Numeric<uint16_t>* u16(uint16_t val = 0);
        Numeric<int16_t>* i16(int16_t val = 0);
        Numeric<uint32_t>* u32(uint32_t val = 0);
        Numeric<int32_t>* i32(int32_t val = 0);
        Numeric<float>* f32(float val = 0);
        String* str(std::string val);

        template <typename T>
        Vector<T>* vec(DataType item_type, uint8_t dims, index_t* shape) {
            return new Vector<T>(item_type, dims, shape);
        }
        template <typename T>
        Vector<T>* vec(DataType item_type, std::vector<index_t> shape) {
            return new Vector<T>(item_type, shape.size(), shape.data());
        }

        Reference* ref(device_t device, port_t port, bool deref, slice_t slice);
    }

    /*
        (Unsafe) Data Casting
    */

    namespace data {
        Null* as_null(qb::Data* data);
        Error* as_error(qb::Data* data);
        Numeric<bool>* as_bool(qb::Data* data);
        Numeric<uint8_t>* as_u8(qb::Data* data);
        Numeric<int8_t>* as_i8(qb::Data* data);
        Numeric<uint16_t>* as_u16(qb::Data* data);
        Numeric<int16_t>* as_i16(qb::Data* data);
        Numeric<uint32_t>* as_u32(qb::Data* data);
        Numeric<int32_t>* as_i32(qb::Data* data);
        Numeric<float>* as_f32(qb::Data* data);
        String* as_str(qb::Data* data);
        
        template <typename T>
        Vector<T>* as_vec(qb::Data* data);
        
        Reference* as_ref(qb::Data* data);
    }
}