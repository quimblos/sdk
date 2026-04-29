namespace sandbox {
    std::string accumulator = HEX({
        HEADER_QUIMBLOS,
        qb::InstructionType::USE_DEVICE, 0x04, 'L', 'E', 'D', '8',
        qb::InstructionType::USE_VAR, qb::DataType::BOOL, 0x00,
        qb::InstructionType::USE_VAR, qb::DataType::UINT8, 0x00,
        qb::InstructionType::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::InstructionType::ADD, 0b01111111, 0x01, qb::DataType::UINT8, 0x01,
        qb::InstructionType::SET_IF_EQ, 0b01111111, 0x00, qb::DataType::REF, 0b01111111, 0x01, qb::DataType::UINT8, 0x05, qb::DataType::BOOL, true, qb::DataType::BOOL, false,
        qb::InstructionType::BRANCH, qb::DataType::REF, 0b01111111, 0x00, 0x00, 0x04, 0x00, 0x06,
            qb::InstructionType::ADD, 0x00, 0x00, qb::DataType::UINT8, 0x01,
            qb::InstructionType::SET, 0b01111111, 0x01, qb::DataType::UINT8, 0x00,
        qb::InstructionType::GOTO, 0x00, 0x00,
    });
}