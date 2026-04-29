#include "data.h"
#include <iostream>

#define ASSERT_N_BYTES(N) \
    if (addr + N > code_len) return { \
        .code = QB_DATA_R_PARSE_FAILED_UNEXPECTED_EOF, \
        .next_addr = 0xFFFF \
    };

#define PARSE_U8(VAR) \
    ASSERT_N_BYTES(1) \
    uint8_t VAR = bytes[addr]; \
    addr += 1;

#define PARSE_U16(VAR) \
    ASSERT_N_BYTES(2) \
    uint16_t VAR = parse_u16(bytes+addr); \
    addr += 2;

#define PARSE_U32(VAR) \
    ASSERT_N_BYTES(4) \
    uint32_t VAR = parse_u32(bytes+addr); \
    addr += 4;

#define PARSE_F32(VAR) \
    ASSERT_N_BYTES(4) \
    float VAR = parse_f32(bytes+addr); \
    addr += 4;

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
qb::data::Reference* qb::data::ref(device_t device, port_t port, bool deref, slice_t slice) {
    return new qb::data::Reference(device, port, deref, qb::data::Slice::from_vec(slice));
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

qb::data::res_t qb::Data::parse(const qb::code_t* bytes, qb::code_addr_t code_len, qb::code_addr_t addr) {
    ASSERT_N_BYTES(1);
    code_t type = bytes[addr];
    addr += 1;
    return make(type, bytes, code_len, addr);
}

qb::data::res_t qb::Data::make(qb::type_t type, const qb::code_t* bytes, qb::code_addr_t code_len, qb::code_addr_t addr) {
    
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
            PARSE_U8(code);
            PARSE_U8(length);
            ASSERT_N_BYTES(length);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Error(code, (uint16_t) length, (char*) bytes+addr),
                .next_addr = (qb::code_addr_t) (addr + length)
            };
        }

        case qb::DataType::BOOL:
        {
            PARSE_U8(value);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<bool>(qb::DataType::BOOL, value),
                .next_addr = addr
            };
        }
            
        case qb::DataType::UINT8:
        {
            PARSE_U8(value);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<uint8_t>(qb::DataType::UINT8, value),
                .next_addr = addr
            };
        }

        case qb::DataType::INT8:
        {
            PARSE_U8(value);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<int8_t>(qb::DataType::INT8, (int8_t) value),
                .next_addr = addr
            };
        }
            
        case qb::DataType::UINT16:
        {
            PARSE_U16(value);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<uint16_t>(qb::DataType::UINT16, value),
                .next_addr = addr
            };
        }

        case qb::DataType::INT16:
        {
            PARSE_U16(value);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<int16_t>(qb::DataType::INT16, (int16_t) value),
                .next_addr = addr
            };
        }
            
        case qb::DataType::UINT32:
        {
            PARSE_U32(value);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<uint32_t>(qb::DataType::UINT32, value),
                .next_addr = addr
            };
        }

        case qb::DataType::INT32:
        {
            PARSE_U32(value);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<int32_t>(qb::DataType::INT32, (int32_t) value),
                .next_addr = addr
            };
        }

        case qb::DataType::FLOAT32:
        {
            PARSE_F32(value);            
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Numeric<float>(qb::DataType::FLOAT32, value),
                .next_addr = addr
            };
        }

        case qb::DataType::STRING:
        {
            PARSE_U16(length);
            ASSERT_N_BYTES(length);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::String(length, (char*) bytes+addr),
                .next_addr = (qb::code_addr_t) (addr+length)
            };
        }

        case qb::DataType::STRING_SHORT:
        {
            PARSE_U8(length);
            ASSERT_N_BYTES(length);
            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::String((uint16_t) length, (char*) bytes+addr),
                .next_addr = (qb::code_addr_t) (addr+length)
            };
        }

        case qb::DataType::VECTOR:
        {
            PARSE_U8(item_type);
            PARSE_U8(dims);
            auto shape = new qb::index_t[dims];
            for (uint8_t i = 0; i < dims; i++) {
                PARSE_U16(dim_size);
                shape[i] = dim_size;
            }

            PARSE_U16(initializers);

            qb::data::Vector<void>* vec = nullptr;
            switch (item_type) {
                case qb::DataType::VOID:
                case qb::DataType::_NULL:
                    return {
                        .code = QB_DATA_R_PARSE_VECTOR_FAILED_UNKNOWN_ITEM_TYPE,
                        .next_addr = 0xFFFF
                    };
                case qb::DataType::ERROR:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<qb::data::Error>(item_type, dims, shape); break;
                case qb::DataType::BOOL:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<bool>(item_type, dims, shape); break;
                case qb::DataType::UINT8:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<uint8_t>(item_type, dims, shape); break;
                case qb::DataType::INT8:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<int8_t>(item_type, dims, shape); break;
                case qb::DataType::UINT16:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<qb::index_t>(item_type, dims, shape); break;
                case qb::DataType::INT16:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<int16_t>(item_type, dims, shape); break;
                case qb::DataType::UINT32:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<uint32_t>(item_type, dims, shape); break;
                case qb::DataType::INT32:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<int32_t>(item_type, dims, shape); break;
                case qb::DataType::FLOAT32:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<float>(item_type, dims, shape); break;
                case qb::DataType::STRING:
                case qb::DataType::STRING_SHORT:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<std::string>(item_type, dims, shape); break;
                case qb::DataType::VECTOR:
                    return {
                        .code = QB_DATA_R_PARSE_VECTOR_FAILED_UNKNOWN_ITEM_TYPE,
                        .next_addr = 0xFFFF
                    };
                case qb::DataType::REF:
                    vec = (qb::data::Vector<void>*) new qb::data::Vector<qb::data::Reference>(item_type, dims, shape); break;
            }
            delete[] shape;

            if (initializers > 0) {
                for (qb::index_t i = 0; i < initializers; i++) {
                    auto res = make(item_type, bytes, code_len, addr);
                    if (res.code > 0) return res;
                    for (qb::index_t j = i; j < vec->size; j+=initializers) {
                        switch (item_type) {
                            case qb::DataType::ERROR:
                                ((qb::data::Vector<qb::data::Error>*)vec)->items[j] = *((qb::data::Error*)res.data)->copy(); break;
                            case qb::DataType::BOOL:
                                ((qb::data::Vector<bool>*)vec)->items[j] = ((qb::data::Numeric<bool>*)res.data)->value; break;
                            case qb::DataType::UINT8:
                                ((qb::data::Vector<uint8_t>*)vec)->items[j] = ((qb::data::Numeric<uint8_t>*)res.data)->value; break;
                            case qb::DataType::INT8:
                                ((qb::data::Vector<int8_t>*)vec)->items[j] = ((qb::data::Numeric<int8_t>*)res.data)->value; break;
                            case qb::DataType::UINT16:
                                ((qb::data::Vector<uint16_t>*)vec)->items[j] = ((qb::data::Numeric<uint16_t>*)res.data)->value; break;
                            case qb::DataType::INT16:
                                ((qb::data::Vector<int16_t>*)vec)->items[j] = ((qb::data::Numeric<int16_t>*)res.data)->value; break;
                            case qb::DataType::UINT32:
                                ((qb::data::Vector<uint32_t>*)vec)->items[j] = ((qb::data::Numeric<uint32_t>*)res.data)->value; break;
                            case qb::DataType::INT32:
                                ((qb::data::Vector<int32_t>*)vec)->items[j] = ((qb::data::Numeric<int32_t>*)res.data)->value; break;
                            case qb::DataType::FLOAT32:
                                ((qb::data::Vector<float>*)vec)->items[j] = ((qb::data::Numeric<float>*)res.data)->value; break;
                            case qb::DataType::STRING:
                            case qb::DataType::STRING_SHORT:
                                ((qb::data::Vector<std::string>*)vec)->items[j] = ((qb::data::String*)res.data)->value; break;
                            case qb::DataType::REF:
                                ((qb::data::Vector<qb::data::Reference>*)vec)->items[j] = *((qb::data::Reference*)res.data)->copy(); break;
                        }
                    }
                    delete res.data;
                    addr = res.next_addr;
                }
            }

            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = vec,
                .next_addr = addr
            };
        }

        case qb::DataType::REF:
        {
            PARSE_U8(device);
            PARSE_U8(port);
            PARSE_U8(flags);

            bool flag_deref = (flags & 0b00000001) > 0;
            bool flag_slice = (flags & 0b00000010) > 0;

            qb::data::Slice* slice = nullptr;
            if (flag_slice) {
                PARSE_U8(dims);
                slice = new qb::data::Slice(dims);

                for (uint8_t i = 0; i < dims; i++) {
                    PARSE_U16(start);
                    PARSE_U16(end);
                    slice->start[i] = start;
                    slice->end[i] = end;
                }
            }

            return {
                .code = QB_DATA_R_PARSE_OK,
                .data = new qb::data::Reference(device, port, flag_deref, slice),
                .next_addr = addr
            };
        }

    }

    return {
        .code = QB_DATA_R_PARSE_FAILED_UNKNOWN_TYPE,
        .next_addr = 0xFFFF
    };
}

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
qb::data::Vector<T>* qb::data::as_vec(qb::Data* data) { return (qb::data::Vector<T>*) data; }

qb::data::Reference* qb::data::as_ref(qb::Data* data) { return (qb::data::Reference*) data; }