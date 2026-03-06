#include <iostream>
#include "app.h"

int main(int argc, char* argv[]) {
    std::string hex = HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x08, 'l','e','d','s','t','r','i','p',
        qb::OpCode::USE_PORT, qb::DataType::UINT8,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::ADD, 0xFF, 0x00, qb::DataType::UINT8, 0x01,
        qb::OpCode::IF_EQ, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x06, qb::DataType::UINT8, 0x05,
            qb::OpCode::ADD, 0x00, 0x00, qb::DataType::UINT8, 0x01,
            qb::OpCode::SET, 0xFF, 0x00, qb::DataType::UINT8, 0x00,
            qb::OpCode::GOTO, 0x00, 0x00,
        qb::OpCode::GOTO, 0x00, 0x00,
    });
    
    return run("blink", hex);
}


/*
@device ledstrip
@var acc uint8

loop:
    sleep 1000
    acc += 1
    if acc == 5:
        ledstrip.leds += 1
        acc = 0
    end
end
*/