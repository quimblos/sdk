#pragma once
#include <string>
#include <sstream>

namespace qb {

    typedef uint8_t code_t;
    typedef uint16_t code_addr_t;

    typedef uint8_t device_t;
    typedef uint8_t port_t;

    typedef uint8_t type_t;
    typedef uint16_t index_t;

    /*
        Static typing
    */

    enum Type {
        // Void
        VOID = 0x00,
        ERROR = 0xFF,

        // Pointers
        PTR = 0x02,           // 4 bytes -> points to a specific dev+port+idx
        PTR_SHORT = 0x01,     // 2 bytes -> points to a specific dev+port     (converted to PTR on parse, used to reduce payload)

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
        STRING = 0xF0,          // + 0xLEN 0xLEN
        STRING_SHORT = 0xF1,    // + 0xLEN        (converted to STRING on parse, used to reduce payload)

        // Arrays
        ARRAY = 0xE0,           // + 0xTYPE 0xLEN 0xLEN
        ARRAY_SHORT = 0xE1,     // + 0xTYPE 0xLEN  (converted to ARRAY on parse, used to reduce payload)
    };

    /*
        Base value struct
    */


    struct Node;

    namespace node {
        struct res_t {
            uint8_t code;
            Node* value;
            code_addr_t next_addr;
        };
    }

    struct Node {
        // A byte representing the static type of this value
        type_t type;

        Node(type_t type): type(type) {}
        virtual ~Node() {};

        // 
        virtual Node* copy() const = 0;

        // Clears the value
        virtual void clear() {};

        // Returns a string representation of the value
        // (Used for debugging)
        virtual const std::string to_str() const = 0;

        // Builds a value from a byte sequence
        static node::res_t parse(type_t type, code_t* bytes, code_addr_t code_len, code_addr_t addr);
        static node::res_t from_bytes(code_t* bytes, code_addr_t code_len, code_addr_t addr);
    };

    /*
        Typed value implementations
    */

    namespace node {

        struct Void: public Node {
            Void():
                Node(Type::VOID)
            {}

            Node* copy() const {
                return new Void();
            }

            const std::string to_str() const {
                return "<void>";
            };
        };

        struct Error: public Node {
            std::string data = "";
            uint8_t code = 0;
            Error(
                uint8_t code,
                uint16_t length,
                const char* bytes
            ):
                Node(Type::ERROR),
                code(code)
            {
                this->data = std::string(bytes, length);
            }

            Error(
                uint8_t code,
                std::string data
            ):
                Node(Type::ERROR),
                data(data)
            {}

            Node* copy() const {
                return new Error(this->code, this->data.size(), this->data.c_str());
            }

            void clear() {
                this->data = "";
                this->code = 0;
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<error." << +this->code << ":" << this->data << ">";
                return ss.str();
            };
        };
    
        struct Pointer: public Node {
            device_t device = 0;
            port_t port = 0;
            index_t index = 0;
            // Node* data = nullptr;
            Pointer(
                device_t device,
                port_t port,
                index_t index = 0
            ):
                Node(Type::PTR),
                device(device),
                port(port),
                index(index)
            {}

            Node* copy() const {
                return new Pointer(this->device, this->port, this->index);
            }

            void clear() {
                this->device = 0;
                this->port = 0;
                this->index = 0;
                // this->data = nullptr;
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<ptr:";
                // if (this->data == nullptr) {
                    ss << +(this->device) << "#" << +(this->port) << "[" << this->index << "]>";
                // }
                // else {
                //     ss << this->data->to_str() << ">";
                // }
                return ss.str();
            };
        };
        
        template <typename T>
        struct Numeric: public Node {
            T data = 0;
            Numeric(
                type_t type,
                T data
            ):
                Node(type),
                data(data)
            {}

            Node* copy() const {
                return new Numeric<T>(this->type, this->data);
            }

            void clear() {
                this->data = 0;
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<";
                switch (this->type) {
                    case Type::VOID:
                        ss << "void"; break;
                    case Type::PTR:
                    case Type::STRING:
                    case Type::ARRAY:
                        ss << "!"; break;
                    case Type::BOOL: ss << "bool:" << (this->data ? "true" : "false"); break;
                    case Type::UINT8: ss << "u8:" << +(this->data); break;
                    case Type::INT8: ss << "i8:" << +(this->data); break;
                    case Type::UINT16: ss << "u16:" << this->data; break;
                    case Type::INT16: ss << "i16:" << this->data; break;
                    case Type::UINT32: ss << "u32:" << this->data; break;
                    case Type::INT32: ss << "i32:" << this->data; break;
                    case Type::FLOAT32: ss << "f32:" << this->data; break;
                }
                ss << ">";
                return ss.str();
            };
        };
    
        struct String: public Node {
            std::string data = "";
            String(
                uint16_t length,
                const char* bytes
            ):
                Node(Type::STRING)
            {
                this->data = std::string(bytes, length);
            }

            String(
                code_addr_t addr,
                std::string data
            ):
                Node(Type::ERROR),
                data(data)
            {}

            Node* copy() const {
                return new String(this->data.size(), this->data.c_str());
            }

            void clear() {
                this->data = "";
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<str:" << this->data << ">";
                return ss.str();
            };
        };
    
        template <typename T>
        struct Array: public Node {
            type_t item_type;
            index_t length = 0;
            T* items = nullptr;
            Array(
                type_t item_type,
                index_t length
            ):
                Node(Type::ARRAY),
                item_type(item_type),
                length(length)
            {
                this->items = new T[length];
            }
            ~Array() {
                delete[] this->items;
            }

            Node* copy() const {
                auto copy = new Array<T>(this->item_type, this->length);
                for (index_t i = 0; i < this->length; i++) {
                    copy->items[i] = this->items[i];
                }
                return copy;
            }

            void clear() {
                for (index_t i = 0; i < this->length; i++) {
                    this->items[i] = (T) 0;
                }
            }

            const std::string to_str() const {
                std::stringstream ss;
                ss << "<";
                switch (this->item_type) {
                    case Type::VOID: ss << "void"; break;
                    case Type::PTR: ss << "ptr"; break;
                    case Type::STRING: ss << "str"; break;
                    case Type::ARRAY: ss << "arr"; break;
                    case Type::BOOL: ss << "bool"; break;
                    case Type::UINT8: ss << "u8"; break;
                    case Type::INT8: ss << "i8"; break;
                    case Type::UINT16: ss << "u16"; break;
                    case Type::INT16: ss << "i16"; break;
                    case Type::UINT32: ss << "u32"; break;
                    case Type::INT32: ss << "i32"; break;
                    case Type::FLOAT32: ss << "f32"; break;
                }
                ss << "[" << this->length << "]:";
                for (index_t i = 0; i < this->length; i++) {
                    switch (this->item_type) {
                        case Type::VOID: break;
                        case Type::PTR: break;
                        case Type::BOOL: ss << (((bool*) this->items)[i] ? "true" : "false"); break;
                        case Type::UINT8: ss << +(((uint8_t*)this->items)[i]); break;
                        case Type::INT8: ss << +((int8_t*)this->items)[i]; break;
                        case Type::UINT16: ss << ((uint16_t*)this->items)[i]; break;
                        case Type::INT16: ss << ((int16_t*)this->items)[i]; break;
                        case Type::UINT32: ss << ((uint32_t*)this->items)[i]; break;
                        case Type::INT32: ss << ((int32_t*)this->items)[i]; break;
                        case Type::FLOAT32: ss << ((float*)this->items)[i]; break;
                        case Type::STRING: ss << ((std::string*)this->items)[i]; break;
                        case Type::ARRAY: break;
                    }
                    if (i < this->length - 1) ss << ',';
                }
                ss << ">";
                return ss.str();
            };
        };

        // Simple initializers

        Pointer* ptr(device_t device, port_t port, index_t index);
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
        Array<T>* arr(Type type, index_t length) {
            return Array<T>(type, length);
        }
    }

    namespace node {
        bool as_bool(Node* node);
        uint8_t as_u8(Node* node);
    };
}

/* Response Codes*/

#define QB_MEMORY_R_OK 00

#define QB_MEMORY_R_PARSE_FAILED_UNEXPECTED_EOF 01
#define QB_MEMORY_R_PARSE_FAILED_UNKNOWN_TYPE 10
#define QB_MEMORY_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY 20