#include "memory.h"

#define ASSERT_N_BYTES(N) \
    if (addr > code_len-N) return { \
        .code = QB_MEMORY_R_PARSE_FAILED_UNEXPECTED_EOF, \
        .next_addr = 0xFFFF \
    };

/*
    Initializers
*/

qb::node::Pointer* qb::node::ptr(device_t device, port_t port, index_t index = 0) {
    return new qb::node::Pointer(device, port, index);
}
qb::node::Numeric<bool>* qb::node::_bool(bool val) {
    return new qb::node::Numeric<bool>(qb::Type::BOOL, val);
}
qb::node::Numeric<uint8_t>* qb::node::u8(uint8_t val) {
    return new qb::node::Numeric<uint8_t>(qb::Type::UINT8, val);
}
qb::node::Numeric<int8_t>* qb::node::i8(int8_t val) {
    return new qb::node::Numeric<int8_t>(qb::Type::INT8, val);
}
qb::node::Numeric<uint16_t>* qb::node::u16(uint16_t val) {
    return new qb::node::Numeric<uint16_t>(qb::Type::UINT16, val);
}
qb::node::Numeric<int16_t>* qb::node::i16(int16_t val) {
    return new qb::node::Numeric<int16_t>(qb::Type::INT16, val);
}
qb::node::Numeric<uint32_t>* qb::node::u32(uint32_t val) {
    return new qb::node::Numeric<uint32_t>(qb::Type::UINT32, val);
}
qb::node::Numeric<int32_t>* qb::node::i32(int32_t val) {
    return new qb::node::Numeric<int32_t>(qb::Type::INT32, val);
}
qb::node::Numeric<float>* qb::node::f32(float val) {
    return new qb::node::Numeric<float>(qb::Type::FLOAT32, val);
}
qb::node::String* qb::node::str(std::string val) {
    return new qb::node::String(val.length(), val.c_str());
}

/*
    Bytecode parsing
*/

template <typename T>
qb::node::res_t parse_array(qb::type_t item_type, qb::index_t length, qb::code_t* bytes, qb::code_addr_t code_len, qb::code_addr_t addr) {

    qb::node::Array<T>* array;
    switch (item_type) {
        case qb::Type::VOID:
        case qb::Type::ERROR:
        case qb::Type::PTR:
        case qb::Type::PTR_SHORT:
            break;
        case qb::Type::BOOL:
        case qb::Type::UINT8:
        case qb::Type::INT8:
        case qb::Type::UINT16:
        case qb::Type::INT16:
        case qb::Type::UINT32:
        case qb::Type::INT32:
        case qb::Type::FLOAT32:
        case qb::Type::STRING:
            array = new qb::node::Array<T>(item_type, length); break;
        case qb::Type::STRING_SHORT:
            array = new qb::node::Array<T>(qb::Type::STRING, length); break;
        case qb::Type::ARRAY:
        case qb::Type::ARRAY_SHORT:
            break;
    }
    if (array == nullptr) return {
        .code = QB_MEMORY_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY,
        .next_addr = 0xFFFF
    };

    for (qb::index_t i = 0; i < length; i++) {
        qb::node::res_t item = qb::Node::parse(item_type, bytes, code_len, addr);
        if (item.code > 0) {
            delete array;
            return item;
        }
        switch (item_type) {
            case qb::Type::VOID:
            case qb::Type::ERROR:
            case qb::Type::PTR:
            case qb::Type::PTR_SHORT:
                break;
            case qb::Type::BOOL:
            case qb::Type::UINT8:
            case qb::Type::INT8:
            case qb::Type::UINT16:
            case qb::Type::INT16:
            case qb::Type::UINT32:
            case qb::Type::INT32:
            case qb::Type::FLOAT32:
            case qb::Type::STRING:
            case qb::Type::STRING_SHORT:
                array->items[i] = ((qb::node::Numeric<T>*)item.value)->data;
            case qb::Type::ARRAY:
            case qb::Type::ARRAY_SHORT:
                break;
        }
        delete item.value;
        addr = item.next_addr;
    }
    
    return {
        .code = QB_MEMORY_R_OK,
        .value = array,
        .next_addr = addr
    };
}

qb::node::res_t qb::Node::parse(qb::type_t type, qb::code_t* bytes, qb::code_addr_t code_len, qb::code_addr_t addr) {
    
    qb::Node* value = nullptr;

    switch (type) {
        case qb::Type::VOID:
        {
           return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Void(),
                .next_addr = (qb::code_addr_t) (addr)
            };
        }

        case qb::Type::ERROR:
        {
            ASSERT_N_BYTES(1);
            uint8_t code = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(1);
            uint8_t length = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(length);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Error(code, (uint16_t) length, (char*) bytes+addr),
                .next_addr = (qb::code_addr_t) (addr + length)
            };
        }

        case qb::Type::PTR:
        {
            ASSERT_N_BYTES(1);
            qb::device_t device = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(1);
            qb::port_t port = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(2);
            qb::index_t index = (bytes[addr] << 8) + bytes[addr+1];
            addr += 2;
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Pointer(device, port, index),
                .next_addr = (qb::code_addr_t) (addr)
            };
        }

        case qb::Type::PTR_SHORT:
        {
            ASSERT_N_BYTES(1);
            qb::device_t device = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(1);
            qb::port_t port = bytes[addr];
            addr += 1;
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Pointer(device, port),
                .next_addr = (qb::code_addr_t) (addr)
            };
        }

        case qb::Type::BOOL:
        {
            ASSERT_N_BYTES(1);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Numeric<bool>(qb::Type::BOOL, bytes[addr]),
                .next_addr = (qb::code_addr_t) (addr+1)
            };
        }
            
        case qb::Type::UINT8:
        {
            ASSERT_N_BYTES(1);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Numeric<uint8_t>(qb::Type::UINT8, bytes[addr]),
                .next_addr = (qb::code_addr_t) (addr+1)
            };
        }

        case qb::Type::INT8:
        {
            ASSERT_N_BYTES(1);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Numeric<int8_t>(qb::Type::INT8, bytes[addr]),
                .next_addr = (qb::code_addr_t) (addr+1)
            };
        }
            
        case qb::Type::UINT16:
        {
            ASSERT_N_BYTES(2);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Numeric<uint16_t>(qb::Type::UINT16, (bytes[addr] << 8) + bytes[addr+1]),
                .next_addr = (qb::code_addr_t) (addr+2)
            };
        }

        case qb::Type::INT16:
        {
            ASSERT_N_BYTES(2);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Numeric<int16_t>(qb::Type::INT16, (bytes[addr] << 8) + bytes[addr+1]),
                .next_addr = (qb::code_addr_t) (addr+2)
            };
        }
            
        case qb::Type::UINT32:
        {
            ASSERT_N_BYTES(4);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Numeric<uint32_t>(qb::Type::UINT32, (bytes[addr] << 24) + (bytes[addr+1] << 16) + (bytes[addr+2] << 8) + bytes[addr+3]),
                .next_addr = (qb::code_addr_t) (addr+4)
            };
        }

        case qb::Type::INT32:
        {
            ASSERT_N_BYTES(4);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Numeric<int32_t>(qb::Type::INT32, (bytes[addr] << 24) + (bytes[addr+1] << 16) + (bytes[addr+2] << 8) + bytes[addr+3]),
                .next_addr = (qb::code_addr_t) (addr+4)
            };
        }

        case qb::Type::FLOAT32:
        {
            ASSERT_N_BYTES(4);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::Numeric<float>(qb::Type::FLOAT32, (bytes[addr] << 24) + (bytes[addr+1] << 16) + (bytes[addr+2] << 8) + bytes[addr+3]),
                .next_addr = (qb::code_addr_t) (addr+4)
            };
        }

        case qb::Type::STRING:
        {
            ASSERT_N_BYTES(2);
            uint16_t length = (bytes[addr] << 8) + bytes[addr+1];
            addr += 2;
            ASSERT_N_BYTES(length);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::String(length, (char*) bytes+addr),
                .next_addr = (qb::code_addr_t) (addr+length)
            };
        }

        case qb::Type::STRING_SHORT:
        {
            ASSERT_N_BYTES(1);
            uint8_t length = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(length);
            return {
                .code = QB_MEMORY_R_OK,
                .value = new qb::node::String((uint16_t) length, (char*) bytes+addr),
                .next_addr = (qb::code_addr_t) (addr+length)
            };
        }

        case qb::Type::ARRAY:
        {
            ASSERT_N_BYTES(1);
            qb::type_t item_type = bytes[addr];
            addr += 1;
            ASSERT_N_BYTES(2);
            uint16_t length = (bytes[addr] << 8) + bytes[addr+1];
            addr += 2;

            qb::node::res_t array_res;
            switch (item_type) {
                case qb::Type::VOID:
                case qb::Type::ERROR:
                case qb::Type::PTR:
                case qb::Type::PTR_SHORT:
                    return { .code = QB_MEMORY_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY, .next_addr = 0xFFFF };
                case qb::Type::BOOL:
                    array_res = parse_array<bool>(item_type, length, bytes, code_len, addr); break;
                case qb::Type::UINT8:
                    array_res = parse_array<uint8_t>(item_type, length, bytes, code_len, addr); break;
                case qb::Type::INT8:
                    array_res = parse_array<int8_t>(item_type, length, bytes, code_len, addr); break;
                case qb::Type::UINT16:
                    array_res = parse_array<uint16_t>(item_type, length, bytes, code_len, addr); break;
                case qb::Type::INT16:
                    array_res = parse_array<int16_t>(item_type, length, bytes, code_len, addr); break;
                case qb::Type::UINT32:
                    array_res = parse_array<uint32_t>(item_type, length, bytes, code_len, addr); break;
                case qb::Type::INT32:
                    array_res = parse_array<int32_t>(item_type, length, bytes, code_len, addr); break;
                case qb::Type::FLOAT32:
                    array_res = parse_array<float>(item_type, length, bytes, code_len, addr); break;
                case qb::Type::STRING:
                case qb::Type::STRING_SHORT:
                    array_res = parse_array<std::string>(item_type, length, bytes, code_len, addr); break;
                case qb::Type::ARRAY:
                case qb::Type::ARRAY_SHORT:
                    return { .code = QB_MEMORY_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY, .next_addr = 0xFFFF };
                default:
                    return { .code = QB_MEMORY_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY, .next_addr = 0xFFFF };
            }

            return {
                .code = QB_MEMORY_R_OK,
                .value = array_res.value,
                .next_addr = (qb::code_addr_t) (array_res.next_addr)
            };
        }
    }

    return {
        .code = QB_MEMORY_R_PARSE_FAILED_UNKNOWN_TYPE,
        .next_addr = 0xFFFF
    };
}

/*
    Public Static
*/

qb::node::res_t qb::Node::from_bytes(qb::code_t* bytes, qb::code_addr_t code_len, qb::code_addr_t addr) {
    ASSERT_N_BYTES(1);
    code_t type = bytes[addr];
    addr += 1;

    return parse(type, bytes, code_len, addr);
}