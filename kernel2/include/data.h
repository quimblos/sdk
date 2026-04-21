#pragma once
#include <string>
#include <sstream>
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

        // Arrays
        ARRAY = 0xB0,           // + 0xTYPE 0xLEN 0xLEN
        ARRAY_SHORT = 0xB1,     // + 0xTYPE 0xLEN  (converted to ARRAY on parse, used to reduce payload)

        // References
        REF = 0xF0,             // + 0xDEVICE 0xPORT
        REF_IDX = 0xF1,         // + 0xDEVICE 0xPORT {index:any}    (converted to REF on parse)
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
        static data::res_t parse(code_t* bytes, code_addr_t code_len, code_addr_t addr);

        // Build a data from a given type and a it's bytes
        static data::res_t make(type_t type, code_t* bytes, code_addr_t code_len, code_addr_t addr);
    };

    /*
        Type Implementations
    */

    namespace data {

        struct Null: public Data {
            Null():
                Data(DataType::_NULL)
            {}

            Data* copy() const {
                return new Null();
            }

            const std::string to_str() const {
                return "<void>";
            };
        };

        struct Error: public Data {
            uint8_t code;
            std::string message;

            Error(uint8_t code, index_t length, const char* bytes):
                Data(DataType::ERROR), code(code) {
                this->message = std::string(bytes, length);
            }

            Error(uint8_t code, std::string message):
                Data(DataType::ERROR), code(code), message(message) {}

            Data* copy() const {
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
            T value;
            Numeric(type_t type, T value):
                Data(type), value(value) {}

            Data* copy() const {
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

            String(index_t length, const char* bytes):
                Data(DataType::STRING)
            {
                this->value = std::string(bytes, length);
            }

            String(std::string value):
                Data(DataType::ERROR), value(value) {}

            Data* copy() const {
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

        struct Reference: public Data {
            bool deref = false;
            device_t device : 7;
            port_t port;
            uint16_t index;

            Reference(bool deref, device_t device, port_t port, uint16_t index):
                Data(DataType::REF), deref(deref), device(device), port(port), index(index) {}

            Data* copy() const {
                return new Reference(this->deref, this->device, this->port, this->index);
            }

            void clear() {
                this->device = 0;
                this->port = 0;
                this->index = 0;
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<ref:" << +(this->device) << "#" << +(this->port) << "[" << this->index << "]" << ">";
                return ss.str();
            };
        };

        template <typename T>
        struct Array: public Data {
            type_t item_type;
            index_t length = 0;
            T* items = nullptr;

            Array(type_t item_type, index_t length):
                Data(DataType::ARRAY), item_type(item_type), length(length) {
                this->items = new T[length];
            }
            ~Array() {
                delete[] this->items;
            }

            Data* copy() const {
                auto copy = new Array<T>(this->item_type, this->length);
                for (uint16_t i = 0; i < this->length; i++) {
                    copy->items[i] = this->items[i];
                }
                return copy;
            }

            void clear() {
                for (uint16_t i = 0; i < this->length; i++) {
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
                    case DataType::ARRAY: return ((Array*)this->items) + index;
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
                    case DataType::ARRAY: ss << "arr"; break;
                    case DataType::REF: ss << "ref"; break;
                }
                ss << "[" << this->length << "]:";
                for (uint16_t i = 0; i < this->length; i++) {
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
                        case DataType::ARRAY: ss << ((Array*)this->items)[i].to_str(); break;
                        case DataType::REF: ss << ((Reference*)this->items)[i].to_str(); break;
                    }
                    if (i < this->length - 1) ss << ',';
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
        Array<T>* arr(DataType item_type, index_t length) {
            return new Array<T>(item_type, length);
        }

        Reference* ref(bool deref, device_t device, port_t port, uint16_t index);
    }
}