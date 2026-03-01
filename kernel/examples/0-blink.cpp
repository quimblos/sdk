#include <iostream>
#include "app.h"

int main(int argc, char* argv[]) {
    std::string hex = HEX({
        HEADER_QUIMBLOS,
        qb::CmdCode::USE_DEVICE, 0x08, 'l','e','d','s','t','r','i','p',
        qb::CmdCode::SET, 0x00, 0x00, qb::DataType::BITMASK8, 0b01010101,
        qb::CmdCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::CmdCode::SET, 0x00, 0x00, qb::DataType::BITMASK8, 0b10101010,
        qb::CmdCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::CmdCode::GOTO, 0x00, 0x00
    });
    
    return run("blink", hex);
}
