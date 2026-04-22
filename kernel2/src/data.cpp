#include "data.h"

#define ASSERT_N_BYTES(N) \
    if (addr + N > code_len) return { \
        .code = QB_DATA_R_PARSE_FAILED_UNEXPECTED_EOF, \
        .next_addr = 0xFFFF \
    };

/*
    Initializers
*/

qb::data::Null* qb::data::_null() {
    return new qb::data::Null();
}
qb::data::Error* qb::data::error(uint8_t code, std::string message) {
    return new qb::data::Error(code, message);
}
qb::data::Numeric<bool>* qb::data::_bool(bool val) {
    return new qb::data::Numeric<bool>(qb::DataType::BOOL, val);
}
qb::data::Numeric<uint8_t>* qb::data::u8(uint8_t val) {
    return new qb::data::Numeric<uint8_t>(qb::DataType::UINT8, val);
}
qb::data::Numeric<int8_t>* qb::data::i8(int8_t val) {
    return new qb::data::Numeric<int8_t>(qb::DataType::INT8, val);
}
qb::data::Numeric<uint16_t>* qb::data::u16(uint16_t val) {
    return new qb::data::Numeric<uint16_t>(qb::DataType::UINT16, val);
}
qb::data::Numeric<int16_t>* qb::data::i16(int16_t val) {
    return new qb::data::Numeric<int16_t>(qb::DataType::INT16, val);
}
qb::data::Numeric<uint32_t>* qb::data::u32(uint32_t val) {
    return new qb::data::Numeric<uint32_t>(qb::DataType::UINT32, val);
}
qb::data::Numeric<int32_t>* qb::data::i32(int32_t val) {
    return new qb::data::Numeric<int32_t>(qb::DataType::INT32, val);
}
qb::data::Numeric<float>* qb::data::f32(float val) {
    return new qb::data::Numeric<float>(qb::DataType::FLOAT32, val);
}
qb::data::String* qb::data::str(std::string val) {
    return new qb::data::String(val.length(), val.c_str());
}
qb::data::Reference* qb::data::ref(bool deref, device_t device, port_t port, uint16_t index) {
    return new qb::data::Reference(deref, device, port, index);
}

/*
    [Bytecode parsing]

    0xPRIMITIVE 0x00 0x00 ...
    0xERR 0xCODE 0xLEN 'a' 'b' ...
    0xSTR 0xLEN 0xLEN 'a' 'b' ...
    0xSTR_SHORT 0xLEN 'a' 'b' ...
    0xARR 0xTYPE 0xLEN 0xLEN 0x00 0x00 ...
    0xARR_SHORT 0xTYPE 0xLEN 0xLEN 0x00 0x00 ...
*/

qb::data::res_t qb::Data::parse(qb::code_t* bytes, qb::code_addr_t code_len, qb::code_addr_t addr) {
    ASSERT_N_BYTES(1);
    code_t type = bytes[addr];
    addr += 1;
    return make(type, bytes, code_len, addr);
}

qb::data::res_t qb::Data::make(qb::type_t type, qb::code_t* bytes, qb::code_addr_t code_len, qb::code_addr_t addr) {
    
    switch (type) {
        case qb::DataType::_NULL:
        {
           return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Null(),
                .next_addr = (qb::code_addr_t) (addr)
            };
        }

        case qb::DataType::ERROR:
        {
            ASSERT_N_BYTES(1);
            uint8_t code = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(1);
            uint8_t length = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(length);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Error(code, (uint16_t) length, (char*) bytes+addr),
                .next_addr = (qb::code_addr_t) (addr + length)
            };
        }

        case qb::DataType::BOOL:
        {
            ASSERT_N_BYTES(1);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<bool>(qb::DataType::BOOL, bytes[addr]),
                .next_addr = (qb::code_addr_t) (addr+1)
            };
        }
            
        case qb::DataType::UINT8:
        {
            ASSERT_N_BYTES(1);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<uint8_t>(qb::DataType::UINT8, bytes[addr]),
                .next_addr = (qb::code_addr_t) (addr+1)
            };
        }

        case qb::DataType::INT8:
        {
            ASSERT_N_BYTES(1);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<int8_t>(qb::DataType::INT8, bytes[addr]),
                .next_addr = (qb::code_addr_t) (addr+1)
            };
        }
            
        case qb::DataType::UINT16:
        {
            ASSERT_N_BYTES(2);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<uint16_t>(qb::DataType::UINT16, parse_u16(bytes+addr)),
                .next_addr = (qb::code_addr_t) (addr+2)
            };
        }

        case qb::DataType::INT16:
        {
            ASSERT_N_BYTES(2);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<int16_t>(qb::DataType::INT16, parse_u16(bytes+addr)),
                .next_addr = (qb::code_addr_t) (addr+2)
            };
        }
            
        case qb::DataType::UINT32:
        {
            ASSERT_N_BYTES(4);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<uint32_t>(qb::DataType::UINT32, parse_u32(bytes+addr)),
                .next_addr = (qb::code_addr_t) (addr+4)
            };
        }

        case qb::DataType::INT32:
        {
            ASSERT_N_BYTES(4);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<int32_t>(qb::DataType::INT32, parse_u32(bytes+addr)),
                .next_addr = (qb::code_addr_t) (addr+4)
            };
        }

        case qb::DataType::FLOAT32:
        {
            ASSERT_N_BYTES(4);
            
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<float>(qb::DataType::FLOAT32, u32_to_float(parse_u32(bytes+addr))),
                .next_addr = (qb::code_addr_t) (addr+4)
            };
        }

        case qb::DataType::STRING:
        {
            ASSERT_N_BYTES(2);
            uint16_t length = (bytes[addr] << 8) | bytes[addr+1];
            addr += 2;
            ASSERT_N_BYTES(length);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::String(length, (char*) bytes+addr),
                .next_addr = (qb::code_addr_t) (addr+length)
            };
        }

        case qb::DataType::STRING_SHORT:
        {
            ASSERT_N_BYTES(1);
            uint8_t length = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(length);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::String((uint16_t) length, (char*) bytes+addr),
                .next_addr = (qb::code_addr_t) (addr+length)
            };
        }

        // case qb::DataType::ARRAY:
        // case qb::DataType::ARRAY_SHORT:
        // {
        //     ASSERT_N_BYTES(1);
        //     qb::type_t item_type = bytes[addr];
        //     addr += 1;

        //     uint16_t length = 0;
        //     if (type == qb::DataType::ARRAY) {
        //         ASSERT_N_BYTES(2);
        //         length = (bytes[addr] << 8) | bytes[addr+1];
        //         addr += 2;
        //     }
        //     else {
        //         ASSERT_N_BYTES(1);
        //         length = bytes[addr];
        //         addr += 1;
        //     }

        //     qb::data::res_t array_res;
        //     switch (item_type) {
        //         case qb::DataType::VOID:
        //         case qb::DataType::ERROR:
        //             return { .code = QB_DATA_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY, .next_addr = 0xFFFF };
        //         case qb::DataType::BOOL:
        //             array_res = parse_array<bool>(item_type, length, bytes, code_len, addr); break;
        //         case qb::DataType::UINT8:
        //             array_res = parse_array<uint8_t>(item_type, length, bytes, code_len, addr); break;
        //         case qb::DataType::INT8:
        //             array_res = parse_array<int8_t>(item_type, length, bytes, code_len, addr); break;
        //         case qb::DataType::UINT16:
        //             array_res = parse_array<uint16_t>(item_type, length, bytes, code_len, addr); break;
        //         case qb::DataType::INT16:
        //             array_res = parse_array<int16_t>(item_type, length, bytes, code_len, addr); break;
        //         case qb::DataType::UINT32:
        //             array_res = parse_array<uint32_t>(item_type, length, bytes, code_len, addr); break;
        //         case qb::DataType::INT32:
        //             array_res = parse_array<int32_t>(item_type, length, bytes, code_len, addr); break;
        //         case qb::DataType::FLOAT32:
        //             array_res = parse_array<float>(item_type, length, bytes, code_len, addr); break;
        //         case qb::DataType::STRING:
        //         case qb::DataType::STRING_SHORT:
        //             array_res = parse_array<std::string>(item_type, length, bytes, code_len, addr); break;
        //         case qb::DataType::ARRAY:
        //         case qb::DataType::ARRAY_SHORT:
        //         case qb::DataType::REF:
        //         case qb::DataType::REF_SHORT:
        //             return { .code = QB_DATA_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY, .next_addr = 0xFFFF };
        //         default:
        //             return { .code = QB_DATA_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY, .next_addr = 0xFFFF };
        //     }

        //     if (array_res.code != QB_DATA_R_PARSE_OK) {
        //         return array_res;
        //     }

        //     return {
        //         .code = QB_DATA_R_PARSE_OK,
        //         .data = array_res.value,
        //         .next_addr = (qb::code_addr_t) (array_res.next_addr)
        //     };
        // }

        case qb::DataType::REF:
        {
            ASSERT_N_BYTES(1);
            bool deref = (bytes[addr] >> 7);
            qb::device_t device = bytes[addr] & 0b01111111;
            ASSERT_N_BYTES(1);
            qb::port_t port = bytes[addr+1];
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Reference(deref, device, port, 0),
                .next_addr = (qb::code_addr_t) (addr+2)
            };
        }

        // case qb::DataType::REF_IDX:
        // {
        //     ASSERT_N_BYTES(1);
        //     qb::device_t device = bytes[addr];
        //     addr += 1;
        //     ASSERT_N_BYTES(1);
        //     qb::port_t port = bytes[addr];
        //     addr += 1;

        //     qb::data::res_t index = qb::data::from_bytes(bytes, code_len, addr);
        //     addr = index.next_addr;

        //     return {
        //         .code = QB_DATA_R_PARSE_OK,
        //         .data = new qb::data::Reference(device, port, index.value),
        //         .next_addr = (qb::code_addr_t) (addr)
        //     };
        // }

    }

    return {
        .code = QB_DATA_R_PARSE_FAILED_UNKNOWN_TYPE,
        .next_addr = 0xFFFF
    };
}

// template <typename T>
// qb::data::res_t parse_array(qb::type_t item_type, qb::index_t length, qb::code_t* bytes, qb::code_addr_t code_len, qb::code_addr_t addr) {

//     qb::data::Array<T>* array;
//     switch (item_type) {
//         case qb::DataType::VOID:
//         case qb::DataType::ERROR:
//         case qb::DataType::REF:
//         case qb::DataType::REF_SHORT:
//             break;
//         case qb::DataType::BOOL:
//         case qb::DataType::UINT8:
//         case qb::DataType::INT8:
//         case qb::DataType::UINT16:
//         case qb::DataType::INT16:
//         case qb::DataType::UINT32:
//         case qb::DataType::INT32:
//         case qb::DataType::FLOAT32:
//         case qb::DataType::STRING:
//             array = new qb::data::Array<T>(item_type, length); break;
//         case qb::DataType::STRING_SHORT:
//             array = new qb::data::Array<T>(qb::DataType::STRING, length); break;
//         case qb::DataType::ARRAY:
//         case qb::DataType::ARRAY_SHORT:
//             break;
//     }
//     if (array == nullptr) return {
//         .code = QB_DATA_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY,
//         .next_addr = 0xFFFF
//     };

//     for (qb::index_t i = 0; i < length; i++) {
//         qb::data::res_t item = qb::data::parse(item_type, bytes, code_len, addr);
//         if (item.code > 0) {
//             delete array;
//             return item;
//         }
//         switch (item_type) {
//             case qb::DataType::VOID:
//             case qb::DataType::ERROR:
//             case qb::DataType::REF:
//             case qb::DataType::REF_SHORT:
//                 break;
//             case qb::DataType::BOOL:
//             case qb::DataType::UINT8:
//             case qb::DataType::INT8:
//             case qb::DataType::UINT16:
//             case qb::DataType::INT16:
//             case qb::DataType::UINT32:
//             case qb::DataType::INT32:
//             case qb::DataType::FLOAT32:
//             case qb::DataType::STRING:
//             case qb::DataType::STRING_SHORT:
//                 array->items[i] = ((qb::data::Numeric<T>*)item.value)->data;
//             case qb::DataType::ARRAY:
//             case qb::DataType::ARRAY_SHORT:
//                 break;
//         }
//         delete item.value;
//         addr = item.next_addr;
//     }
    
//     return {
//         .code = QB_DATA_R_PARSE_OK,
//         .data = array,
//         .next_addr = addr
//     };
// }

// (Unsafe) cast

qb::data::Null* qb::data::as_null(qb::Data* data) { return (qb::data::Null*) data; }
qb::data::Error* qb::data::as_error(qb::Data* data) { return (qb::data::Error*) data; }
qb::data::Numeric<bool>* qb::data::as_bool(qb::Data* data) { return (qb::data::Numeric<bool>*) data; }
qb::data::Numeric<uint8_t>* qb::data::as_u8(qb::Data* data) { return (qb::data::Numeric<uint8_t>*) data; }
qb::data::Numeric<int8_t>* qb::data::as_i8(qb::Data* data) { return (qb::data::Numeric<int8_t>*) data; }
qb::data::Numeric<uint16_t>* qb::data::as_u16(qb::Data* data) { return (qb::data::Numeric<uint16_t>*) data; }
qb::data::Numeric<int16_t>* qb::data::as_i16(qb::Data* data) { return (qb::data::Numeric<int16_t>*) data; }
qb::data::Numeric<uint32_t>* qb::data::as_u32(qb::Data* data) { return (qb::data::Numeric<uint32_t>*) data; }
qb::data::Numeric<int32_t>* qb::data::as_i32(qb::Data* data) { return (qb::data::Numeric<int32_t>*) data; }
qb::data::Numeric<float>* qb::data::as_f32(qb::Data* data) { return (qb::data::Numeric<float>*) data; }
qb::data::String* qb::data::as_str(qb::Data* data) { return (qb::data::String*) data; }

template <typename T>
qb::data::Array<T>* qb::data::as_arr(qb::Data* data) { return (qb::data::Array<T>*) data; }

qb::data::Reference* qb::data::as_ref(qb::Data* data) { return (qb::data::Reference*) data; }