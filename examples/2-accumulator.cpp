#include <iostream>
#include "app.h"

int main(int argc, char* argv[]) {
    std::string hex = HEX({
        HEADER_QUIMBLOS,
        qb::CmdCode::USE_DEVICE, 0x08, 'l','e','d','s','t','r','i','p',
        qb::CmdCode::USE_REG, qb::DataType::UINT8,
        qb::CmdCode::ADD, 0xFF, 0x00, qb::DataType::UINT8, 0x01,
        qb::CmdCode::IF_EQ, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x05, qb::DataType::UINT8, 0x04,
            qb::CmdCode::ADD, 0x00, 0x00, qb::DataType::UINT8, 0x01,
            qb::CmdCode::SET, 0xFF, 0x00, qb::DataType::UINT8, 0x00,
            qb::CmdCode::GOTO, 0x00, 0x00,
        qb::CmdCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::CmdCode::GOTO, 0x00, 0x00,
    });
    
    return run("blink", hex);
}