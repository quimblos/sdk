namespace sandbox {
    std::string clock = HEX({
        // HEADER_QUIMBLOS,
        qb::InstructionType::USE_DEVICE, 0x04, 'L','E','D','8',
        qb::InstructionType::ADD, 0x00, 0x00, qb::DataType::UINT8, 0x01,
        qb::InstructionType::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::InstructionType::GOTO, 0x00, 0x00
    });
}