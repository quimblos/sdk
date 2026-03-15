#include <iostream>
#include "parser.h"
#include "hex.h"

void test_parse(std::string name, std::string hex) {
    qb::Engine engine;
    LedDevice device;
    engine.put_device(device);

    auto parser_res = qb::parser::parse(engine, "test", hex);
    if (parser_res.code == 0) {
        std::cout << __OK << " " << name << NC << std::endl;
        std::cout << parser_res.script->describe();
        std::cout << std::endl;
    }
    else {
        std::cout << __ERROR << +parser_res.code << " " << name << NC << std::endl;
        std::cout << std::endl;
    }
}

void test_code_parser() {

    /* Instructions */

    test_parse("USE_DEVICE", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L','E','D',
    }));
    test_parse("USE_NODE", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_NODE, qb::Type::UINT8, 0x01
    }));

    test_parse("SET (device)", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L','E','D',
        qb::OpCode::SET, qb::OpBind::SHORT_NODE_NODE, 0x00, 0x00, qb::Type::UINT8, 0xFF
    }));
    test_parse("SET (node)", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_NODE_ALIASED, qb::Type::UINT8, 0x00, 0x03, 'v', 'a', 'l',
        qb::OpCode::SET, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0xFF
    }));

    test_parse("HOLD", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L','E','D',
        qb::OpCode::HOLD, 0x00
    }));
    test_parse("RELEASE", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L','E','D',
        qb::OpCode::RELEASE, 0x00
    }));

    test_parse("GOTO", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x00, 0x01,
        qb::OpCode::GOTO, 0x00, 0x00,
    }));
    test_parse("IF_EQ", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x00, 0x01,
        qb::OpCode::IF_EQ, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x00, 0x00, 0x01, 0xFF, 0xFF
    }));
    test_parse("IF_GT", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x00, 0x01,
        qb::OpCode::IF_GT, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x00, 0x00, 0x01, 0xFF, 0xFF
    }));
    test_parse("IF_GTEQ", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x00, 0x01,
        qb::OpCode::IF_GTEQ, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x00, 0x00, 0x01, 0xFF, 0xFF
    }));

    test_parse("ADD", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::ADD, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x00
    }));
    test_parse("SUB", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::SUB, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x00
    }));
    test_parse("MULT", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::MULT, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x00
    }));
    test_parse("DIV", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::DIV, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x00
    }));
    test_parse("MOD", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::MOD, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x00
    }));
    test_parse("POW", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::POW, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x00
    }));
    test_parse("FLOOR", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::FLOOR, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00
    }));
    test_parse("CEIL", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::CEIL, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00
    }));

    test_parse("LOG", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L','E','D',
        qb::OpCode::LOG, 0x00, qb::Type::STRING_SHORT, 0x02, 'O', 'N'
    }));
    test_parse("SLEEP", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x00, 0x01
    }));
    test_parse("RETURN", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::RETURN, qb::Type::STRING_SHORT, 0x02, 'O', 'K'
    }));

    test_parse("RESET", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::RESET
    }));
    test_parse("REBOOT", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::REBOOT
    }));

}