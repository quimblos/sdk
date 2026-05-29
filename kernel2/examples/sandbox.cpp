#include <iostream>
#include "example.h"

int main(int argc, char* argv[]) {
    
    invoke("blink", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::ADD_CONST, B_TYPE_U8, 0b01010101,
        qb::OpCode::ADD_CONST, B_TYPE_U8, 0b10101010,
        qb::OpCode::ADD_CONST, B_TYPE_U16, 0x03, 0xe8,
        // qb::OpCode::ADD_VAR, B_TYPE_U8,
        qb::OpCode::SET, 0x00, BLOCK_CONTEXT, 1, BLOCK_CONTEXT, 2,
        qb::OpCode::SLEEP, BLOCK_CONTEXT, 3,
        // qb::OpCode::SET, 0x00, BLOCK_CONTEXT, 0, BLOCK_CONTEXT, 2,
    }));
}
