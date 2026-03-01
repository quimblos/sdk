#include <iostream>
#include "app.h"

int main(int argc, char* argv[]) {
    std::string hex = HEX({
        HEADER_QUIMBLOS,
        qb::CmdCode::USE_DEVICE, 0x08, 'l','e','d','s','t','r','i','p',
        qb::CmdCode::USE_REG, qb::DataType::UINT8,
        qb::CmdCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::CmdCode::SUM, 0xFF, 0x00, qb::DataType::UINT8, 0x01,
        qb::CmdCode::IF_EQ, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x06, qb::DataType::UINT8, 0x05,
            qb::CmdCode::SUM, 0x00, 0x00, qb::DataType::UINT8, 0x01,
            qb::CmdCode::SET, 0xFF, 0x00, qb::DataType::UINT8, 0x00,
            qb::CmdCode::GOTO, 0x00, 0x00,
        qb::CmdCode::GOTO, 0x00, 0x00,
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