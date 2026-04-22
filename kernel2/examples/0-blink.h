namespace sandbox {
    std::string blink = HEX({
        // HEADER_QUIMBLOS,
        qb::InstructionType::USE_DEVICE, 0x04, 'L','E','D','8',
        qb::InstructionType::SET, 0x00, 0x00, qb::DataType::UINT8, 0b01010101,
        qb::InstructionType::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::InstructionType::SET, 0x00, 0x00, qb::DataType::UINT8, 0b10101010,
        qb::InstructionType::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::InstructionType::GOTO, 0x00, 0x00
    });
}