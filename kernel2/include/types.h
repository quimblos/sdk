#pragma once
#include <stdint.h>

namespace qb {

    typedef uint8_t code_t;
    typedef uint16_t code_addr_t;

    typedef uint8_t type_t;
    
    typedef uint8_t device_t;
    typedef uint8_t port_t;
    typedef uint16_t index_t;

    // value is either:
    // - (null) nullptr
    // - (numeric, string) a pointer to the memory allocated by the qb::Data object
    // - (error, array or ref) a pointer to the qb::Data object
    struct data_t {
        type_t type;
        void* value;
        bool heap = false;
    };

}

/*
    Response Codes
*/

// Data Parsing

#define QB_DATA_R_PARSE_OK 00
#define QB_DATA_R_PARSE_FAILED_UNEXPECTED_EOF 01
#define QB_DATA_R_PARSE_FAILED_UNKNOWN_TYPE 10
#define QB_DATA_R_PARSE_ARRAY_FAILED_UNKNOWN_ARRAY 20

#define QB_CODE_R_PARSE_OK 0
#define QB_CODE_R_PARSE_FAILED_UNEXPECTED_EOF 1
#define QB_CODE_R_PARSE_FAILED_UNKNOWN_TYPE 2

#define QB_CODE_R_PARSE_FAILED_INVALID_HEADER 3
#define QB_CODE_R_PARSE_FAILED_INVALID_OPCODE 4
#define QB_CODE_R_PARSE_FAILED_TARGET 5
#define QB_CODE_R_PARSE_FAILED_DATA 6
#define QB_CODE_R_PARSE_FAILED_DEVICE_INDEX 7
#define QB_CODE_R_PARSE_FAILED_UNBOUND_GOTO 8