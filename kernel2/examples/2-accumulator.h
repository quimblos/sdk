#define MATH_OP_ADD 0b01000000
#define COMPARE_OP_EQ 0b01000000

namespace sandbox {
    std::string accumulator = HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DRIVER, 0x04, 'L', 'E', 'D', '8',

        qb::OpCode::ADD_CONST, B_TYPE_U8, 0x00,
        qb::OpCode::ADD_CONST, B_TYPE_U8, 0x01,
        qb::OpCode::ADD_CONST, B_TYPE_U8, 0x05,
        qb::OpCode::ADD_CONST, B_TYPE_U16, 0x03, 0xe8,
        qb::OpCode::ADD_VAR, B_TYPE_U8,
        qb::OpCode::ADD_VAR, B_TYPE_BOOL,

        qb::OpCode::SLEEP, BLOCK_CONTEXT, 4,
        qb::OpCode::MATH, MATH_OP_ADD, BLOCK_CONTEXT, 5, BLOCK_CONTEXT, 2,
        qb::OpCode::SET_IF, COMPARE_OP_EQ, BLOCK_CONTEXT, 6, BLOCK_CONTEXT, 5, BLOCK_CONTEXT, 3,
        qb::OpCode::IF, 0x00, BLOCK_CONTEXT, 6, 0x00, 0x04, 0x00, 0x06,
            qb::OpCode::MATH, MATH_OP_ADD, 0, 0, BLOCK_CONTEXT, 2,
            qb::OpCode::SET, 0x00, BLOCK_CONTEXT, 5, BLOCK_CONTEXT, 1,
        qb::OpCode::GOTO, 0x00, 0x00
    });
}