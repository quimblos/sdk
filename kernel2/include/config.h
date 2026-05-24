#pragma once
#include <iostream>

#define LOG(X) std::cout << #X " = " << (sizeof(X) == 1 ? +X : X) << std::endl;

#define BLOCK_ENGINE 0xFF
#define BLOCK_NODE 0xFE
#define BLOCK_THREAD 0xFD
#define BLOCK_METHOD 0xFC

#define PORT_CONST_FALSE 0x00
#define PORT_CONST_TRUE 0x01

#define EPSILON 0.000000000000000001

namespace qb {

    typedef uint8_t void_t;
    typedef uint8_t block_t;
    typedef uint8_t port_t;
    typedef uint16_t index_t;
    typedef uint8_t byte_t;

    typedef byte_t* data_t;

    typedef uint8_t type_t;
    typedef uint8_t instr_type_t;
    
    typedef uint16_t code_addr_t;

}