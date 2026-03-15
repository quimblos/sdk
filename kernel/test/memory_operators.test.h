#include <iostream>
#include "memory.h"
#include "operator.h"

VAL(v_void, qb::Type::VOID)
VAL(v_error, qb::Type::ERROR, 0xFF, 0x03, 'w', 't', 'f')

VAL(v_ptr, qb::Type::PTR, DEVICE_1, PORT_2, 0x00, 0x03);
VAL(v_ptr_short, qb::Type::PTR_SHORT, DEVICE_2, PORT_1);

VAL(v_bool, qb::Type::BOOL, true);
VAL(v_uint8, qb::Type::UINT8, 32);
VAL(v_int8, qb::Type::INT8, (qb::code_t) -32);
VAL(v_uint16, qb::Type::UINT16, 0xFF, 0xFF);
VAL(v_int16, qb::Type::INT16, 0xFF, 0xFF);
VAL(v_uint32, qb::Type::UINT32, 0xFF, 0xFF, 0xFF, 0xFF);
VAL(v_int32, qb::Type::INT32, 0xFF, 0xFF, 0xFF, 0xFF);
VAL(v_float32, qb::Type::FLOAT32, 0xFF, 0xFF, 0xFF, 0xFF);

VAL(v_string, qb::Type::STRING, 0x00, 0x05, 'a', 'b', 'o', 'u', 'd');
VAL(v_string_short, qb::Type::STRING_SHORT, 0x04, 'a', 'b', 'o', 'o');

VAL(v_array_bool, qb::Type::ARRAY, qb::Type::BOOL, 0x00, 0x03, true, false, true);
VAL(v_array_uint8, qb::Type::ARRAY, qb::Type::UINT8, 0x00, 0x03, 1, 2, 3);
VAL(v_array_int8, qb::Type::ARRAY, qb::Type::INT8, 0x00, 0x03, (qb::code_t) -1, (qb::code_t) -2, (qb::code_t) -3);
VAL(v_array_uint16, qb::Type::ARRAY, qb::Type::UINT16, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
VAL(v_array_int16, qb::Type::ARRAY, qb::Type::INT16, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
VAL(v_array_uint32, qb::Type::ARRAY, qb::Type::UINT32, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
VAL(v_array_int32, qb::Type::ARRAY, qb::Type::INT32, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
VAL(v_array_float32, qb::Type::ARRAY, qb::Type::FLOAT32, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
VAL(v_array_string, qb::Type::ARRAY, qb::Type::STRING, 0x00, 0x02, 0x00, 0x03, 'a', 'b', 'c', 0x00, 0x05, 'd', 'e', 'f', 'g', 'h');
VAL(v_array_string_short, qb::Type::ARRAY, qb::Type::STRING_SHORT, 0x00, 0x02, 0x03, 'a', 'b', 'c', 0x05, 'd', 'e', 'f', 'g', 'h');

void test_declare(qb::node::res_t res) {
    if (res.code == 0) {
        std::cout << __OK << res.value->to_str() << NC << std::endl;
    }
    else {
        std::cout << __ERROR << +res.code << " " << NC << std::endl;
    }
}

void test_copy_to_ok(qb::node::res_t a, qb::node::res_t b) {
    auto ptr = qb::node::Pointer(0xFF, 0x00);
    ptr.data = a.value;
    auto res = qb::_operator::copy_to(&ptr, b.value);
    if (res.code == 0) {
        std::cout << __OK << a.value->to_str() << " = " << b.value->to_str() << NC << std::endl;
    }
    else {
        std::cout << __ERROR << +res.code << a.value->to_str() << " = " << b.value->to_str() << NC << std::endl;
    }
}

void test_copy_to_fail(qb::node::res_t a, qb::node::res_t b) {
    auto ptr = qb::node::Pointer(0xFF, 0x00);
    ptr.data = a.value;
    auto res = qb::_operator::copy_to(&ptr, b.value);
    if (res.code > 0) {
        std::cout << __OK << a.value->to_str() << " = " << b.value->to_str() << NC << std::endl;
    }
    else {
        std::cout << __ERROR << +res.code << a.value->to_str() << " = " << b.value->to_str() << NC << std::endl;
    }
}

void test_memory_operators() {

    /* Declaration */
    std::cout << "\n" << CYAN << "declaration" << NC << std::endl;
    {
        test_declare(v_void);
        test_declare(v_error);
        test_declare(v_ptr);
        test_declare(v_ptr_short);
        test_declare(v_bool);
        test_declare(v_uint8);
        test_declare(v_int8);
        test_declare(v_uint16);
        test_declare(v_int16);
        test_declare(v_uint32);
        test_declare(v_int32);
        test_declare(v_float32);
        test_declare(v_string);
        test_declare(v_string_short);
        test_declare(v_array_bool);
        test_declare(v_array_uint8);
        test_declare(v_array_int8);
        test_declare(v_array_uint16);
        test_declare(v_array_int16);
        test_declare(v_array_uint32);
        test_declare(v_array_int32);
        test_declare(v_array_float32);
        test_declare(v_array_string);
        test_declare(v_array_string_short);
    }

    /* Assignment */

    std::cout << "\n" << CYAN << "void = ..." << NC << std::endl;
    {
        test_copy_to_fail(v_void, v_void);
        test_copy_to_fail(v_void, v_error);
        test_copy_to_fail(v_void, v_ptr);
        test_copy_to_fail(v_void, v_bool);
        test_copy_to_fail(v_void, v_uint8);
        test_copy_to_fail(v_void, v_int8);
        test_copy_to_fail(v_void, v_uint16);
        test_copy_to_fail(v_void, v_int16);
        test_copy_to_fail(v_void, v_uint32);
        test_copy_to_fail(v_void, v_int32);
        test_copy_to_fail(v_void, v_float32);
        test_copy_to_fail(v_void, v_string);
        test_copy_to_fail(v_void, v_array_bool);
        test_copy_to_fail(v_void, v_array_uint8);
        test_copy_to_fail(v_void, v_array_int8);
        test_copy_to_fail(v_void, v_array_uint16);
        test_copy_to_fail(v_void, v_array_int16);
        test_copy_to_fail(v_void, v_array_uint32);
        test_copy_to_fail(v_void, v_array_int32);
        test_copy_to_fail(v_void, v_array_float32);
        test_copy_to_fail(v_void, v_array_string);
    }

    std::cout << "\n" << CYAN << "error = ..." << NC << std::endl;
    {
        test_copy_to_fail(v_error, v_void);
        test_copy_to_ok  (v_error, v_error);
        test_copy_to_fail(v_error, v_ptr);
        test_copy_to_fail(v_error, v_bool);
        test_copy_to_fail(v_error, v_uint8);
        test_copy_to_fail(v_error, v_int8);
        test_copy_to_fail(v_error, v_uint16);
        test_copy_to_fail(v_error, v_int16);
        test_copy_to_fail(v_error, v_uint32);
        test_copy_to_fail(v_error, v_int32);
        test_copy_to_fail(v_error, v_float32);
        test_copy_to_ok  (v_error, v_string);
        test_copy_to_fail(v_error, v_array_bool);
        test_copy_to_fail(v_error, v_array_uint8);
        test_copy_to_fail(v_error, v_array_int8);
        test_copy_to_fail(v_error, v_array_uint16);
        test_copy_to_fail(v_error, v_array_int16);
        test_copy_to_fail(v_error, v_array_uint32);
        test_copy_to_fail(v_error, v_array_int32);
        test_copy_to_fail(v_error, v_array_float32);
        test_copy_to_fail(v_error, v_array_string);
    }

    std::cout << "\n" << CYAN << "ptr = ..." << NC << std::endl;
    {
        test_copy_to_fail(v_ptr, v_void);
        test_copy_to_fail(v_ptr, v_error);
        test_copy_to_ok  (v_ptr, v_ptr);
        test_copy_to_fail(v_ptr, v_bool);
        test_copy_to_fail(v_ptr, v_uint8);
        test_copy_to_fail(v_ptr, v_int8);
        test_copy_to_fail(v_ptr, v_uint16);
        test_copy_to_fail(v_ptr, v_int16);
        test_copy_to_fail(v_ptr, v_uint32);
        test_copy_to_fail(v_ptr, v_int32);
        test_copy_to_fail(v_ptr, v_float32);
        test_copy_to_fail(v_ptr, v_string);
        test_copy_to_fail(v_ptr, v_array_bool);
        test_copy_to_fail(v_ptr, v_array_uint8);
        test_copy_to_fail(v_ptr, v_array_int8);
        test_copy_to_fail(v_ptr, v_array_uint16);
        test_copy_to_fail(v_ptr, v_array_int16);
        test_copy_to_fail(v_ptr, v_array_uint32);
        test_copy_to_fail(v_ptr, v_array_int32);
        test_copy_to_fail(v_ptr, v_array_float32);
        test_copy_to_fail(v_ptr, v_array_string);
    }

    std::cout << "\n" << CYAN << "bool = ..." << NC << std::endl;
    {
        test_copy_to_fail(v_bool, v_void);
        test_copy_to_fail(v_bool, v_error);
        test_copy_to_fail(v_bool, v_ptr);
        test_copy_to_ok  (v_bool, v_bool);
        test_copy_to_ok  (v_bool, v_uint8);
        test_copy_to_ok  (v_bool, v_int8);
        test_copy_to_ok  (v_bool, v_uint16);
        test_copy_to_ok  (v_bool, v_int16);
        test_copy_to_ok  (v_bool, v_uint32);
        test_copy_to_ok  (v_bool, v_int32);
        test_copy_to_ok  (v_bool, v_float32);
        test_copy_to_fail(v_bool, v_string);
        test_copy_to_fail(v_bool, v_array_bool);
        test_copy_to_fail(v_bool, v_array_uint8);
        test_copy_to_fail(v_bool, v_array_int8);
        test_copy_to_fail(v_bool, v_array_uint16);
        test_copy_to_fail(v_bool, v_array_int16);
        test_copy_to_fail(v_bool, v_array_uint32);
        test_copy_to_fail(v_bool, v_array_int32);
        test_copy_to_fail(v_bool, v_array_float32);
        test_copy_to_fail(v_bool, v_array_string);
    }

    std::cout << "\n" << CYAN << "uint8 = ..." << NC << std::endl;
    {
        test_copy_to_fail(v_uint8, v_void);
        test_copy_to_fail(v_uint8, v_error);
        test_copy_to_fail(v_uint8, v_ptr);
        test_copy_to_ok  (v_uint8, v_bool);
        test_copy_to_ok  (v_uint8, v_uint8);
        test_copy_to_ok  (v_uint8, v_int8);
        test_copy_to_ok  (v_uint8, v_uint16);
        test_copy_to_ok  (v_uint8, v_int16);
        test_copy_to_ok  (v_uint8, v_uint32);
        test_copy_to_ok  (v_uint8, v_int32);
        test_copy_to_ok  (v_uint8, v_float32);
        test_copy_to_fail(v_uint8, v_string);
        test_copy_to_fail(v_uint8, v_array_bool);
        test_copy_to_fail(v_uint8, v_array_uint8);
        test_copy_to_fail(v_uint8, v_array_int8);
        test_copy_to_fail(v_uint8, v_array_uint16);
        test_copy_to_fail(v_uint8, v_array_int16);
        test_copy_to_fail(v_uint8, v_array_uint32);
        test_copy_to_fail(v_uint8, v_array_int32);
        test_copy_to_fail(v_uint8, v_array_float32);
        test_copy_to_fail(v_uint8, v_array_string);
    }

    // ... other primitives ...
    
    std::cout << "\n" << CYAN << "string = ..." << NC << std::endl;
    {
        test_copy_to_fail(v_string, v_void);
        test_copy_to_ok  (v_string, v_error);
        test_copy_to_fail(v_string, v_ptr);
        test_copy_to_fail(v_string, v_bool);
        test_copy_to_fail(v_string, v_uint8);
        test_copy_to_fail(v_string, v_int8);
        test_copy_to_fail(v_string, v_uint16);
        test_copy_to_fail(v_string, v_int16);
        test_copy_to_fail(v_string, v_uint32);
        test_copy_to_fail(v_string, v_int32);
        test_copy_to_fail(v_string, v_float32);
        test_copy_to_ok  (v_string, v_string_short);
        test_copy_to_fail(v_string, v_array_bool);
        test_copy_to_fail(v_string, v_array_uint8);
        test_copy_to_fail(v_string, v_array_int8);
        test_copy_to_fail(v_string, v_array_uint16);
        test_copy_to_fail(v_string, v_array_int16);
        test_copy_to_fail(v_string, v_array_uint32);
        test_copy_to_fail(v_string, v_array_int32);
        test_copy_to_fail(v_string, v_array_float32);
        test_copy_to_fail(v_string, v_array_string);
    }

    std::cout << "\n" << CYAN << "bool[0] = ..." << NC << std::endl;
    {
        test_copy_to_fail(v_array_bool, v_void);
        test_copy_to_fail(v_array_bool, v_error);
        test_copy_to_fail(v_array_bool, v_ptr);
        test_copy_to_ok(v_array_bool, v_bool);
        test_copy_to_ok(v_array_bool, v_uint8);
        test_copy_to_ok(v_array_bool, v_int8);
        test_copy_to_ok(v_array_bool, v_uint16);
        test_copy_to_ok(v_array_bool, v_int16);
        test_copy_to_ok(v_array_bool, v_uint32);
        test_copy_to_ok(v_array_bool, v_int32);
        test_copy_to_ok(v_array_bool, v_float32);
        test_copy_to_fail(v_array_bool, v_string_short);
        test_copy_to_fail(v_array_bool, v_array_bool);
        test_copy_to_fail(v_array_bool, v_array_uint8);
        test_copy_to_fail(v_array_bool, v_array_int8);
        test_copy_to_fail(v_array_bool, v_array_uint16);
        test_copy_to_fail(v_array_bool, v_array_int16);
        test_copy_to_fail(v_array_bool, v_array_uint32);
        test_copy_to_fail(v_array_bool, v_array_int32);
        test_copy_to_fail(v_array_bool, v_array_float32);
        test_copy_to_fail(v_array_bool, v_array_string);
    }

    // ... other primitive arrays ...

}
