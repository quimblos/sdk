#define OP_ADD 0b01000000

namespace sandbox {
    std::string clock = HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DRIVER, 0x04, 'L', 'E', 'D', '8',

        qb::OpCode::ADD_CONST, B_TYPE_U8, 0x01,
        qb::OpCode::ADD_CONST, B_TYPE_U16, 0x03, 0xe8,
        
        qb::OpCode::MATH, OP_ADD, 0, 0, BLOCK_CONTEXT, 1,
        qb::OpCode::SLEEP, BLOCK_CONTEXT, 2,
        qb::OpCode::GOTO, 0x00, 0x00
    });
}