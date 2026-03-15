#include <iostream>
#include "parser.h"
#include "static_checker.h"
#include "hex.h"

void test_check(bool should_pass, std::string name, std::string hex) {
    qb::Engine engine;
    LedDevice device;
    engine.put_device(device);

    auto parser_res = qb::parser::parse(engine, "test", hex);
    if (parser_res.code != 0) {
        std::cout << __ERROR << "parser" << +parser_res.code << " " << name << NC << std::endl;
        std::cout << std::endl;
        return;
    }

    auto checker_res = qb::static_checker::check(engine, parser_res.script);
    if (should_pass) {
        if (checker_res.code == 0) {
            std::cout << __OK << " " << name << NC << std::endl;
        }
        else {
            std::cout << __ERROR << +checker_res.code << " " << name << NC << std::endl;
        }
    }
    else {
        if (checker_res.code != 0) {
            std::cout << __OK << " (error." << +checker_res.code << ") " << name << NC << std::endl;
        }
        else {
            std::cout << __ERROR << +checker_res.code << " " << name << NC << std::endl;
        }
    }

    std::cout << parser_res.script->describe();
    std::cout << std::endl;
}

void test_static_checker() {

    /* Type Casting */

    test_check(true, "$.u8 <- u8", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_NODE, qb::Type::UINT8, 0x00,
        qb::OpCode::SET, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x03
    }));

    test_check(false, "$.u8 <- str", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_NODE, qb::Type::UINT8, 0x00,
        qb::OpCode::SET, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::STRING_SHORT, 0x03, 'w', 't', 'f'
    }));

    test_check(true, "device.u8 <- u8", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L', 'E', 'D',
        qb::OpCode::SET, qb::OpBind::SHORT_NODE_NODE, 0x00, 0x00, qb::Type::UINT8, 0x03
    }));

    test_check(false, "device.u8 <- str", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L', 'E', 'D',
        qb::OpCode::SET, qb::OpBind::SHORT_NODE_NODE, 0x00, 0x00, qb::Type::STRING_SHORT, 0x03, 'w', 't', 'f'
    }));

    test_check(false, "ptr <- u8", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L', 'E', 'D',
        qb::OpCode::USE_NODE, qb::Type::PTR_SHORT, 0x00, 0x00,
        qb::OpCode::SET, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x03
    }));

    test_check(true, "ptr:[device.u8] <- u8", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L', 'E', 'D',
        qb::OpCode::USE_NODE, qb::Type::PTR_SHORT, 0x00, 0x00,
        qb::OpCode::SET, qb::OpBind::SHORT_REF_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x03
    }));

    test_check(false, "ptr:[device.u8] <- str", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L', 'E', 'D',
        qb::OpCode::USE_NODE, qb::Type::PTR_SHORT, 0x00, 0x00,
        qb::OpCode::SET, qb::OpBind::SHORT_REF_NODE, 0xFF, 0x00, qb::Type::STRING_SHORT, 0x03, 'w', 't', 'f'
    }));

    test_check(false, "u8:[device.u8] <- str", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L', 'E', 'D',
        qb::OpCode::USE_NODE, qb::Type::UINT8, 0x00,
        qb::OpCode::SET, qb::OpBind::SHORT_REF_NODE, 0xFF, 0x00, qb::Type::STRING_SHORT, 0x03, 'w', 't', 'f'
    }));

    test_check(false, "hold script", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::HOLD, 0xFE
    }));

}