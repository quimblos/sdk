#include <iostream>
#include "app.h"

int main(int argc, char* argv[]) {
    std::string hex = HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x08, 'L','E','D','S','T','R','I','P',
        qb::OpCode::SET_AT, 0x00, 0x00, qb::DataType::UINT8, 0x01, qb::DataType::UINT8, 0xFF
    });
    
    return run("blink", hex);
}
