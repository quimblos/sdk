#pragma once
#include <stdint.h>
#include <memory>
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

        // 1 byte
        UINT8 = 0x10,
        INT8 = 0x1A,
        BITMASK8 = 0x1B,

        // 2 bytes
        UINT16 = 0x20,
        UINT8_XY = 0x21,
        INT16 = 0x2A,
        INT8_XY = 0x2B,
        
        // 4 bytes
        UINT32 = 0x40,
        UINT16_XY = 0x41,
        UINT8_XYZW = 0x42,
        INT32 = 0x4A,
        INT16_XY = 0x4B,
        INT8_XYZW = 0x4C,
        
        // n bytes
        STRING = 0xF0,
        CONST = 0xFF
    };
    
    // Stores data on:
    // - Devices: registers
    // - Scripts: cmds
    //
    // This object should not be copied, since it's
    // destructor destroys the value, and copies would
    // do so when destroyed.
    //
    struct Data {
        DataType type;
        data_t* value = nullptr;

        Data* ref() {
            return new Data(
                this->type,
                this->value
            );
        }

        void purge() {
            // std::cout << "[delete]" << std::endl;
            if (this->value == nullptr) return;
            switch (type) {
                case DataType::VOID:
                    break;
                case DataType::UINT8:
                case DataType::INT8:
                case DataType::BITMASK8:
                    delete (uint8_t*) this->value;
                    break;
                case DataType::UINT16:
                case DataType::UINT8_XY:
                case DataType::INT16:
                case DataType::INT8_XY:
                    delete (uint16_t*) this->value;
                    break;
                case DataType::UINT32:
                case DataType::UINT16_XY:
                case DataType::UINT8_XYZW:
                case DataType::INT32:
                case DataType::INT16_XY:
                case DataType::INT8_XYZW:
                    delete (uint32_t*) this->value;
                    break;
                case DataType::STRING:
                    delete (std::string*) this->value;
                    break;
                case DataType::CONST:
                    break;
            }
            // this->value = nullptr;
        }

        void set(qb::Data& other) {
            switch (type) {
                case DataType::VOID:
                    break;
                case DataType::UINT8:
                case DataType::INT8:
                case DataType::BITMASK8:
                    ((uint8_t*) this->value)[0] = ((uint8_t*) other.value)[0];
                    break;
                case DataType::UINT16:
                    ((uint16_t*) this->value)[0] = ((uint16_t*) other.value)[0];
                    break;
                case DataType::UINT8_XY:
                    ((uint8_t*) this->value)[0] = ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] = ((uint8_t*) other.value)[1];
                    break;
                case DataType::INT16:
                    ((int16_t*) this->value)[0] = ((int16_t*) other.value)[0];
                    break;
                case DataType::INT8_XY:
                    ((int8_t*) this->value)[0] = ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] = ((int8_t*) other.value)[1];
                    break;
                case DataType::UINT32:
                    ((uint32_t*) this->value)[0] = ((uint32_t*) other.value)[0];
                    break;
                case DataType::UINT16_XY:
                    ((uint16_t*) this->value)[0] = ((uint16_t*) other.value)[0];
                    ((uint16_t*) this->value)[1] = ((uint16_t*) other.value)[1];
                    break;
                case DataType::UINT8_XYZW:
                    ((uint8_t*) this->value)[0] = ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] = ((uint8_t*) other.value)[1];
                    ((uint8_t*) this->value)[2] = ((uint8_t*) other.value)[2];
                    ((uint8_t*) this->value)[3] = ((uint8_t*) other.value)[3];
                    break;
                case DataType::INT32:
                    ((int32_t*) this->value)[0] = ((int32_t*) other.value)[0];
                    break;
                case DataType::INT16_XY:
                    ((int16_t*) this->value)[0] = ((int16_t*) other.value)[0];
                    ((int16_t*) this->value)[1] = ((int16_t*) other.value)[1];
                    break;
                case DataType::INT8_XYZW:
                    ((int8_t*) this->value)[0] = ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] = ((int8_t*) other.value)[1];
                    ((int8_t*) this->value)[2] = ((int8_t*) other.value)[2];
                    ((int8_t*) this->value)[3] = ((int8_t*) other.value)[3];
                    break;
                case DataType::STRING:
                    *((std::string*) this->value) = std::string(*((std::string*) other.value));
                    break;
                case DataType::CONST:
                    *((std::string*) this->value) = std::string(*((std::string*) other.value));
                    break;
            }
        }


        static qb::Data _void() {
            return qb::Data({ .type = qb::DataType::VOID, .value = nullptr });
        }

        static qb::Data u8(uint8_t x = 0x0) {
            uint8_t* mem = new uint8_t;
            mem[0] = x;
            return qb::Data({ .type = qb::DataType::UINT8, .value = mem });
        }

        static qb::Data i8(int8_t x = 0x0) {
            int8_t* mem = new int8_t;
            mem[0] = x;
            return qb::Data({ .type = qb::DataType::INT8, .value = mem });
        }

        static qb::Data b8(bool v0 = false, bool v1 = false, bool v2 = false, bool v3 = false, bool v4 = false, bool v5 = false, bool v6 = false, bool v7 = false) {
            uint8_t* mem = new uint8_t;
            *mem = (v0 << 7) + (v1 << 6) + (v2 << 5) + (v3 << 4) + (v4 << 3) + (v5 << 2) + (v6 << 1) + v7;
            return qb::Data({ .type = qb::DataType::BITMASK8, .value = mem });
        }

        static qb::Data u16(uint16_t x = 0x00) {
            uint16_t* mem = new uint16_t;
            mem[0] = x;
            return qb::Data({ .type = qb::DataType::UINT16, .value = mem });
        }

        static qb::Data u8xy(uint8_t x = 0x0, uint8_t y = 0x0) {
            uint8_t* mem = new uint8_t[2];
            mem[0] = x;
            mem[1] = x;
            return qb::Data({ .type = qb::DataType::UINT8_XY, .value = mem });
        }

        static qb::Data i16(int16_t x = 0x00) {
            int16_t* mem = new int16_t;
            mem[0] = x;
            return qb::Data({ .type = qb::DataType::INT16, .value = mem });
        }

        static qb::Data i8xy(int8_t x = 0x0, int8_t y = 0x0) {
            int8_t* mem = new int8_t[2];
            mem[0] = x;
            mem[1] = x;
            return qb::Data({ .type = qb::DataType::INT8_XY, .value = mem });
        }

        static qb::Data u32(uint32_t x = 0x0000) {
            uint32_t* mem = new uint32_t;
            mem[0] = x;
            return qb::Data({ .type = qb::DataType::UINT32, .value = mem });
        }

        static qb::Data u16xy(uint16_t x = 0x00, uint16_t y = 0x00) {
            uint16_t* mem = new uint16_t[2];
            mem[0] = x;
            mem[1] = y;
            return qb::Data({ .type = qb::DataType::UINT16_XY, .value = mem });
        }

        static qb::Data u8xyzw(uint8_t x = 0x0, uint8_t y = 0x0, uint8_t z = 0x0, uint8_t w = 0x0) {
            uint8_t* mem = new uint8_t[4];
            mem[0] = x;
            mem[1] = y;
            mem[2] = z;
            mem[3] = w;
            return qb::Data({ .type = qb::DataType::UINT8_XYZW, .value = mem });
        }

        static qb::Data i32(int32_t x = 0x0000) {
            int32_t* mem = new int32_t;
            mem[0] = x;
            return qb::Data({ .type = qb::DataType::INT32, .value = mem });
        }

        static qb::Data i16xy(int16_t x = 0x00, int16_t y = 0x00) {
            int16_t* mem = new int16_t[2];
            mem[0] = x;
            mem[1] = y;
            return qb::Data({ .type = qb::DataType::INT16_XY, .value = mem });
        }

        static qb::Data i8xyzw(int8_t x = 0x0, int8_t y = 0x0, int8_t z = 0x0, int8_t w = 0x0) {
            int8_t* mem = new int8_t[4];
            mem[0] = x;
            mem[1] = y;
            mem[2] = z;
            mem[3] = w;
            return qb::Data({ .type = qb::DataType::INT8_XYZW, .value = mem });
        }

        static qb::Data string(uint16_t length, const char* bytes) {
            std::string* mem = new std::string(bytes, length);
            return qb::Data({ .type = qb::DataType::STRING, .value = mem });
        }

        uint8_t as_u8() {
            return *((uint8_t*) this->value);
        }
        int8_t as_i8() {
            return *((int8_t*) this->value);
        }
        bool* as_b8() {
            return (bool*) this->value;
        }
        uint16_t as_u16() {
            return *((uint16_t*) this->value);
        }
        uint8_t* as_u8xy() {
            return (uint8_t*) this->value;
        }
        int16_t as_i16() {
            return *((int16_t*) this->value);
        }
        int8_t* as_i8xy() {
            return (int8_t*) this->value;
        }
        uint32_t as_u32() {
            return *((uint32_t*) this->value);
        }
        uint16_t* as_u16xy() {
            return (uint16_t*) this->value;
        }
        uint8_t* as_u8xyzw() {
            return (uint8_t*) this->value;
        }
        int32_t as_i32() {
            return *((int32_t*) this->value);
        }
        int16_t* as_i16xy() {
            return (int16_t*) this->value;
        }
        int8_t* as_i8xyzw() {
            return (int8_t*) this->value;
        }
        const std::string& as_string() {
            return *((std::string*) this->value);
        }

    };
    
}