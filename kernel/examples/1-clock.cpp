#include <iostream>
#include "app.h"

int main(int argc, char* argv[]) {
    std::string hex = HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x08, 'l','e','d','s','t','r','i','p',
        qb::OpCode::SUM, 0x00, 0x00, qb::DataType::UINT8, 0x01,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::GOTO, 0x00, 0x00
    });
    
    return run("blink", hex);
}