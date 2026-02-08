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
        FLOAT32 = 0x4F,
        
        // n bytes
        STRING = 0xF0,
        CONST = 0xFF
    };
    
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
            switch (this->type) {
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
                case DataType::FLOAT32:
                    delete (float*) this->value;
                    break;
                case DataType::STRING:
                    delete (std::string*) this->value;
                    break;
                case DataType::CONST:
                    break;
            }
            // this->value = nullptr;
        }

        void log() {
            switch (this->type) {
                case DataType::VOID:
                    std::cout << "{void};"; break;
                case DataType::UINT8:
                    std::cout << "{u8:" << +(this->as_u8()) << "};"; break;
                case DataType::INT8:
                    std::cout << "{i8:" << +(this->as_i8()) << "};"; break;
                case DataType::BITMASK8:
                    std::cout << "{b8:" << std::bitset<8>(+*(this->as_b8())) << "};"; break;
                case DataType::UINT16:
                    std::cout << "{u16:" << +(this->as_u16()) << "};"; break;
                case DataType::UINT8_XY:
                    std::cout << "{u8_xy:" << +(this->as_u8xy()[0]) << "," << +(this->as_u8xy()[1]) << "};"; break;
                case DataType::INT16:
                    std::cout << "{i16:" << +(this->as_i16()) << "};"; break;
                case DataType::INT8_XY:
                    std::cout << "{i8_xy:" << +(this->as_i8xy()[0]) << "," << +(this->as_i8xy()[1]) << "};"; break;
                case DataType::UINT32:
                    std::cout << "{u32:" << +(this->as_u32()) << "};"; break;
                case DataType::UINT16_XY:
                    std::cout << "{u16_xy:" << +(this->as_u16xy()[0]) << "," << +(this->as_u16xy()[1]) << "};"; break;
                case DataType::UINT8_XYZW:
                    std::cout << "{u8_xyzw:" << +(this->as_u8xyzw()[0]) << "," << +(this->as_u8xyzw()[1]) << "," << +(this->as_u8xyzw()[2]) << "," << +(this->as_u8xyzw()[3]) << "};"; break;
                case DataType::INT32:
                    std::cout << "{i32:" << +(this->as_i32()) << "};"; break;
                case DataType::INT16_XY:
                    std::cout << "{i16_xy:" << +(this->as_i16xy()[0]) << "," << +(this->as_i16xy()[1]) << "};"; break;
                case DataType::INT8_XYZW:
                    std::cout << "{i8_xyzw:" << +(this->as_i8xyzw()[0]) << "," << +(this->as_i8xyzw()[1]) << "," << +(this->as_i8xyzw()[2]) << "," << +(this->as_i8xyzw()[3]) << "};"; break;
                case DataType::FLOAT32:
                    std::cout << "{f32:" << +(this->as_f32()[0]); break;
                case DataType::STRING:
                    std::cout << "{str:" << this->as_string() << "};"; break;
                case DataType::CONST:
                    std::cout << "{const:" << this->as_string() << "};"; break;
            }
            // this->value = nullptr;
        }

        void set(qb::Data& other) {
            switch (this->type) {
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
                case DataType::FLOAT32:
                    ((float*) this->value)[0] = ((float*) other.value)[0];
                    break;
                case DataType::STRING:
                    *((std::string*) this->value) = std::string(*((std::string*) other.value));
                    break;
                case DataType::CONST:
                    *((std::string*) this->value) = std::string(*((std::string*) other.value));
                    break;
            }
        }

        void reset() {
            switch (this->type) {
                case DataType::VOID:
                    break;
                case DataType::UINT8:
                case DataType::INT8:
                case DataType::BITMASK8:
                    ((uint8_t*) this->value)[0] = 0x00;
                    break;
                case DataType::UINT16:
                    ((uint16_t*) this->value)[0] = 0x00;
                    break;
                case DataType::UINT8_XY:
                    ((uint8_t*) this->value)[0] = 0x00;
                    ((uint8_t*) this->value)[1] = 0x00;
                    break;
                case DataType::INT16:
                    ((int16_t*) this->value)[0] = 0x00;
                    break;
                case DataType::INT8_XY:
                    ((int8_t*) this->value)[0] = 0x00;
                    ((int8_t*) this->value)[1] = 0x00;
                    break;
                case DataType::UINT32:
                    ((uint32_t*) this->value)[0] = 0x00;
                    break;
                case DataType::UINT16_XY:
                    ((uint16_t*) this->value)[0] = 0x00;
                    ((uint16_t*) this->value)[1] = 0x00;
                    break;
                case DataType::UINT8_XYZW:
                    ((uint8_t*) this->value)[0] = 0x00;
                    ((uint8_t*) this->value)[1] = 0x00;
                    ((uint8_t*) this->value)[2] = 0x00;
                    ((uint8_t*) this->value)[3] = 0x00;
                    break;
                case DataType::INT32:
                    ((int32_t*) this->value)[0] = 0x00;
                    break;
                case DataType::INT16_XY:
                    ((int16_t*) this->value)[0] = 0x00;
                    ((int16_t*) this->value)[1] = 0x00;
                    break;
                case DataType::INT8_XYZW:
                    ((int8_t*) this->value)[0] = 0x00;
                    ((int8_t*) this->value)[1] = 0x00;
                    ((int8_t*) this->value)[2] = 0x00;
                    ((int8_t*) this->value)[3] = 0x00;
                    break;
                case DataType::FLOAT32:
                    ((float*) this->value)[0] = 0x00;
                    break;
                case DataType::STRING:
                    *((std::string*) this->value) = std::string("");
                    break;
                case DataType::CONST:
                    *((std::string*) this->value) = std::string("");
                    break;
            }
        }

        uint8_t compare(qb::Data& other) {
            switch (this->type) {
                case DataType::VOID:
                    return 0;
                case DataType::UINT8:
                case DataType::INT8:
                case DataType::BITMASK8:
                    {
                        uint8_t diff = ((uint8_t*) other.value)[0] - ((uint8_t*) this->value)[0];
                        return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                    }
                case DataType::UINT16:
                    {
                        uint16_t diff = ((uint16_t*) other.value)[0] - ((uint16_t*) this->value)[0];
                        return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                    }
                case DataType::UINT8_XY:
                    {
                        uint8_t diff0 = ((uint8_t*) other.value)[0] - ((uint8_t*) this->value)[0];
                        uint8_t diff1 = ((uint8_t*) other.value)[1] - ((uint8_t*) this->value)[1];
                        return (diff0 > 0 && diff1 > 0) ? 1 : ((diff0 < 0 && diff1 < 0) ? -1 : 0);
                    }
                case DataType::INT16:
                    {
                        int16_t diff = ((int16_t*) other.value)[0] - ((int16_t*) this->value)[0];
                        return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                    }
                case DataType::INT8_XY:
                    {
                        int8_t diff0 = ((int8_t*) other.value)[0] - ((int8_t*) this->value)[0];
                        int8_t diff1 = ((int8_t*) other.value)[1] - ((int8_t*) this->value)[1];
                        return (diff0 > 0 && diff1 > 0) ? 1 : ((diff0 < 0 && diff1 < 0) ? -1 : 0);
                    }
                case DataType::UINT32:
                    {
                        uint32_t diff = ((uint32_t*) other.value)[0] - ((uint32_t*) this->value)[0];
                        return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                    }
                case DataType::UINT16_XY:
                    {
                        uint16_t diff0 = ((uint16_t*) other.value)[0] - ((uint16_t*) this->value)[0];
                        uint16_t diff1 = ((uint16_t*) other.value)[1] - ((uint16_t*) this->value)[1];
                        return (diff0 > 0 && diff1 > 0) ? 1 : ((diff0 < 0 && diff1 < 0) ? -1 : 0);
                    }
                case DataType::UINT8_XYZW:
                    {
                        uint8_t diff0 = ((uint8_t*) other.value)[0] - ((uint8_t*) this->value)[0];
                        uint8_t diff1 = ((uint8_t*) other.value)[1] - ((uint8_t*) this->value)[1];
                        uint8_t diff2 = ((uint8_t*) other.value)[2] - ((uint8_t*) this->value)[2];
                        uint8_t diff3 = ((uint8_t*) other.value)[3] - ((uint8_t*) this->value)[3];
                        return (diff0 > 0 && diff1 > 0 && diff2 > 0 && diff3 > 0) ? 1 : ((diff0 < 0 && diff1 < 0 && diff2 < 0 && diff3 < 0) ? -1 : 0);
                    }
                case DataType::INT32:
                    {
                        int32_t diff = ((int32_t*) other.value)[0] - ((int32_t*) this->value)[0];
                        return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                    }
                case DataType::INT16_XY:
                    {
                        int16_t diff0 = ((int16_t*) other.value)[0] - ((int16_t*) this->value)[0];
                        int16_t diff1 = ((int16_t*) other.value)[1] - ((int16_t*) this->value)[1];
                        return (diff0 > 0 && diff1 > 0) ? 1 : ((diff0 < 0 && diff1 < 0) ? -1 : 0);
                    }
                case DataType::INT8_XYZW:
                    {
                        int8_t diff0 = ((int8_t*) other.value)[0] - ((int8_t*) this->value)[0];
                        int8_t diff1 = ((int8_t*) other.value)[1] - ((int8_t*) this->value)[1];
                        int8_t diff2 = ((int8_t*) other.value)[2] - ((int8_t*) this->value)[2];
                        int8_t diff3 = ((int8_t*) other.value)[3] - ((int8_t*) this->value)[3];
                        return (diff0 > 0 && diff1 > 0 && diff2 > 0 && diff3 > 0) ? 1 : ((diff0 < 0 && diff1 < 0 && diff2 < 0 && diff3 < 0) ? -1 : 0);
                    }
                case DataType::FLOAT32:
                    {
                        float diff = ((float*) other.value)[0] - ((float*) this->value)[0];
                        return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                    }
                case DataType::STRING:
                    return 0;
                case DataType::CONST:
                    return 0;
            }
            return 0;
        }

        void add(qb::Data& other) {
            switch (this->type) {
                case DataType::VOID:
                    break;
                case DataType::UINT8:
                case DataType::INT8:
                case DataType::BITMASK8:
                    ((uint8_t*) this->value)[0] += ((uint8_t*) other.value)[0];
                    break;
                case DataType::UINT16:
                    ((uint16_t*) this->value)[0] += ((uint16_t*) other.value)[0];
                    break;
                case DataType::UINT8_XY:
                    ((uint8_t*) this->value)[0] += ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] += ((uint8_t*) other.value)[1];
                    break;
                case DataType::INT16:
                    ((int16_t*) this->value)[0] += ((int16_t*) other.value)[0];
                    break;
                case DataType::INT8_XY:
                    ((int8_t*) this->value)[0] += ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] += ((int8_t*) other.value)[1];
                    break;
                case DataType::UINT32:
                    ((uint32_t*) this->value)[0] += ((uint32_t*) other.value)[0];
                    break;
                case DataType::UINT16_XY:
                    ((uint16_t*) this->value)[0] += ((uint16_t*) other.value)[0];
                    ((uint16_t*) this->value)[1] += ((uint16_t*) other.value)[1];
                    break;
                case DataType::UINT8_XYZW:
                    ((uint8_t*) this->value)[0] += ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] += ((uint8_t*) other.value)[1];
                    ((uint8_t*) this->value)[2] += ((uint8_t*) other.value)[2];
                    ((uint8_t*) this->value)[3] += ((uint8_t*) other.value)[3];
                    break;
                case DataType::INT32:
                    ((int32_t*) this->value)[0] += ((int32_t*) other.value)[0];
                    break;
                case DataType::INT16_XY:
                    ((int16_t*) this->value)[0] += ((int16_t*) other.value)[0];
                    ((int16_t*) this->value)[1] += ((int16_t*) other.value)[1];
                    break;
                case DataType::INT8_XYZW:
                    ((int8_t*) this->value)[0] += ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] += ((int8_t*) other.value)[1];
                    ((int8_t*) this->value)[2] += ((int8_t*) other.value)[2];
                    ((int8_t*) this->value)[3] += ((int8_t*) other.value)[3];
                    break;
                case DataType::FLOAT32:
                    ((float*) this->value)[0] += ((float*) other.value)[0];
                    break;
                case DataType::STRING:
                    break;
                case DataType::CONST:
                    break;
            }
        }

        void sub(qb::Data& other) {
            switch (this->type) {
                case DataType::VOID:
                    break;
                case DataType::UINT8:
                case DataType::INT8:
                case DataType::BITMASK8:
                    ((uint8_t*) this->value)[0] -= ((uint8_t*) other.value)[0];
                    break;
                case DataType::UINT16:
                    ((uint16_t*) this->value)[0] -= ((uint16_t*) other.value)[0];
                    break;
                case DataType::UINT8_XY:
                    ((uint8_t*) this->value)[0] -= ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] -= ((uint8_t*) other.value)[1];
                    break;
                case DataType::INT16:
                    ((int16_t*) this->value)[0] -= ((int16_t*) other.value)[0];
                    break;
                case DataType::INT8_XY:
                    ((int8_t*) this->value)[0] -= ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] -= ((int8_t*) other.value)[1];
                    break;
                case DataType::UINT32:
                    ((uint32_t*) this->value)[0] -= ((uint32_t*) other.value)[0];
                    break;
                case DataType::UINT16_XY:
                    ((uint16_t*) this->value)[0] -= ((uint16_t*) other.value)[0];
                    ((uint16_t*) this->value)[1] -= ((uint16_t*) other.value)[1];
                    break;
                case DataType::UINT8_XYZW:
                    ((uint8_t*) this->value)[0] -= ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] -= ((uint8_t*) other.value)[1];
                    ((uint8_t*) this->value)[2] -= ((uint8_t*) other.value)[2];
                    ((uint8_t*) this->value)[3] -= ((uint8_t*) other.value)[3];
                    break;
                case DataType::INT32:
                    ((int32_t*) this->value)[0] -= ((int32_t*) other.value)[0];
                    break;
                case DataType::INT16_XY:
                    ((int16_t*) this->value)[0] -= ((int16_t*) other.value)[0];
                    ((int16_t*) this->value)[1] -= ((int16_t*) other.value)[1];
                    break;
                case DataType::INT8_XYZW:
                    ((int8_t*) this->value)[0] -= ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] -= ((int8_t*) other.value)[1];
                    ((int8_t*) this->value)[2] -= ((int8_t*) other.value)[2];
                    ((int8_t*) this->value)[3] -= ((int8_t*) other.value)[3];
                    break;
                case DataType::FLOAT32:
                    ((float*) this->value)[0] -= ((float*) other.value)[0];
                    break;
                case DataType::STRING:
                    break;
                case DataType::CONST:
                    break;
            }
        }

        void mult(qb::Data& other) {
            switch (this->type) {
                case DataType::VOID:
                    break;
                case DataType::UINT8:
                case DataType::INT8:
                case DataType::BITMASK8:
                    ((uint8_t*) this->value)[0] *= ((uint8_t*) other.value)[0];
                    break;
                case DataType::UINT16:
                    ((uint16_t*) this->value)[0] *= ((uint16_t*) other.value)[0];
                    break;
                case DataType::UINT8_XY:
                    ((uint8_t*) this->value)[0] *= ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] *= ((uint8_t*) other.value)[1];
                    break;
                case DataType::INT16:
                    ((int16_t*) this->value)[0] *= ((int16_t*) other.value)[0];
                    break;
                case DataType::INT8_XY:
                    ((int8_t*) this->value)[0] *= ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] *= ((int8_t*) other.value)[1];
                    break;
                case DataType::UINT32:
                    ((uint32_t*) this->value)[0] *= ((uint32_t*) other.value)[0];
                    break;
                case DataType::UINT16_XY:
                    ((uint16_t*) this->value)[0] *= ((uint16_t*) other.value)[0];
                    ((uint16_t*) this->value)[1] *= ((uint16_t*) other.value)[1];
                    break;
                case DataType::UINT8_XYZW:
                    ((uint8_t*) this->value)[0] *= ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] *= ((uint8_t*) other.value)[1];
                    ((uint8_t*) this->value)[2] *= ((uint8_t*) other.value)[2];
                    ((uint8_t*) this->value)[3] *= ((uint8_t*) other.value)[3];
                    break;
                case DataType::INT32:
                    ((int32_t*) this->value)[0] *= ((int32_t*) other.value)[0];
                    break;
                case DataType::INT16_XY:
                    ((int16_t*) this->value)[0] *= ((int16_t*) other.value)[0];
                    ((int16_t*) this->value)[1] *= ((int16_t*) other.value)[1];
                    break;
                case DataType::INT8_XYZW:
                    ((int8_t*) this->value)[0] *= ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] *= ((int8_t*) other.value)[1];
                    ((int8_t*) this->value)[2] *= ((int8_t*) other.value)[2];
                    ((int8_t*) this->value)[3] *= ((int8_t*) other.value)[3];
                    break;
                case DataType::FLOAT32:
                    ((float*) this->value)[0] *= ((float*) other.value)[0];
                    break;
                case DataType::STRING:
                    break;
                case DataType::CONST:
                    break;
            }
        }

        void div(qb::Data& other) {
            switch (this->type) {
                case DataType::VOID:
                    break;
                case DataType::UINT8:
                case DataType::INT8:
                case DataType::BITMASK8:
                    ((uint8_t*) this->value)[0] /= ((uint8_t*) other.value)[0];
                    break;
                case DataType::UINT16:
                    ((uint16_t*) this->value)[0] /= ((uint16_t*) other.value)[0];
                    break;
                case DataType::UINT8_XY:
                    ((uint8_t*) this->value)[0] /= ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] /= ((uint8_t*) other.value)[1];
                    break;
                case DataType::INT16:
                    ((int16_t*) this->value)[0] /= ((int16_t*) other.value)[0];
                    break;
                case DataType::INT8_XY:
                    ((int8_t*) this->value)[0] /= ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] /= ((int8_t*) other.value)[1];
                    break;
                case DataType::UINT32:
                    ((uint32_t*) this->value)[0] /= ((uint32_t*) other.value)[0];
                    break;
                case DataType::UINT16_XY:
                    ((uint16_t*) this->value)[0] /= ((uint16_t*) other.value)[0];
                    ((uint16_t*) this->value)[1] /= ((uint16_t*) other.value)[1];
                    break;
                case DataType::UINT8_XYZW:
                    ((uint8_t*) this->value)[0] /= ((uint8_t*) other.value)[0];
                    ((uint8_t*) this->value)[1] /= ((uint8_t*) other.value)[1];
                    ((uint8_t*) this->value)[2] /= ((uint8_t*) other.value)[2];
                    ((uint8_t*) this->value)[3] /= ((uint8_t*) other.value)[3];
                    break;
                case DataType::INT32:
                    ((int32_t*) this->value)[0] /= ((int32_t*) other.value)[0];
                    break;
                case DataType::INT16_XY:
                    ((int16_t*) this->value)[0] /= ((int16_t*) other.value)[0];
                    ((int16_t*) this->value)[1] /= ((int16_t*) other.value)[1];
                    break;
                case DataType::INT8_XYZW:
                    ((int8_t*) this->value)[0] /= ((int8_t*) other.value)[0];
                    ((int8_t*) this->value)[1] /= ((int8_t*) other.value)[1];
                    ((int8_t*) this->value)[2] /= ((int8_t*) other.value)[2];
                    ((int8_t*) this->value)[3] /= ((int8_t*) other.value)[3];
                    break;
                case DataType::FLOAT32:
                    ((float*) this->value)[0] /= ((float*) other.value)[0];
                    break;
                case DataType::STRING:
                    break;
                case DataType::CONST:
                    break;
            }
        }

        static qb::Data of_type(DataType type) {
            switch (type) {
                case DataType::VOID: return Data::_void();
                case DataType::UINT8: return Data::u8();
                case DataType::INT8: return Data::i8();
                case DataType::BITMASK8:return Data::b8();
                case DataType::UINT16: return Data::u16();
                case DataType::UINT8_XY: return Data::u8xy();
                case DataType::INT16: return Data::i16();
                case DataType::INT8_XY: return Data::i8xy();
                case DataType::UINT32: return Data::u32();
                case DataType::UINT16_XY: return Data::u16xy();
                case DataType::UINT8_XYZW: return Data::u8xyzw();
                case DataType::INT32: return Data::i32();
                case DataType::INT16_XY: return Data::i16xy();
                case DataType::INT8_XYZW: return Data::i8xyzw();
                case DataType::FLOAT32: return Data::f32();
                case DataType::STRING: return Data::string();
                case DataType::CONST: return Data::string();
            }
            return Data::_void();
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

        static qb::Data f32(float x = 0x0) {
            float* mem = new float;
            mem[0] = x;
            return qb::Data({ .type = qb::DataType::FLOAT32, .value = mem });
        }

        static qb::Data string(uint16_t length= 0, const char* bytes = nullptr) {
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
        float* as_f32() {
            return (float*) this->value;
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