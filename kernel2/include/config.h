#pragma once

#define LOG(X) std::cout << #X " = " << X << std::endl;

#define BLOCK_THREAD_CONST 0xFF
#define BLOCK_THREAD 0xFE
#define BLOCK_NODE 0xFD

#define PORT_CONST_FALSE 0x00
#define PORT_CONST_TRUE 0x01

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