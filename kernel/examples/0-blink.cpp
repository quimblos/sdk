#include <iostream>
#include "app.h"

int main(int argc, char* argv[]) {
    std::string hex = HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x06, 'L','E','D','B','A','R',
        qb::OpCode::SET, 0x00, 0x00, qb::DataType::UINT8, 0b01010101,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::SET, 0x00, 0x00, qb::DataType::UINT8, 0b10101010,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::GOTO, 0x00, 0x00
    });
    
    return run("blink", hex);
}
