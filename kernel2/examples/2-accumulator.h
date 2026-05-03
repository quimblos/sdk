namespace sandbox {
    std::string accumulator = HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x04, 'L', 'E', 'D', '8',
        qb::OpCode::USE_VAR, qb::DataType::BOOL, 0x00,
        qb::OpCode::USE_VAR, qb::DataType::UINT8, 0x00,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::ADD, 0b01111111, 0x01, qb::DataType::UINT8, 0x01,
        qb::OpCode::SET_IF_EQ, 0b01111111, 0x00, qb::DataType::REF, 0b01111111, 0x01, qb::DataType::UINT8, 0x05, qb::DataType::BOOL, true, qb::DataType::BOOL, false,
        qb::OpCode::BRANCH, qb::DataType::REF, 0b01111111, 0x00, 0x00, 0x04, 0x00, 0x06,
            qb::OpCode::ADD, 0x00, 0x00, qb::DataType::UINT8, 0x01,
            qb::OpCode::SET, 0b01111111, 0x01, qb::DataType::UINT8, 0x00,
        qb::OpCode::GOTO, 0x00, 0x00,
    });
}