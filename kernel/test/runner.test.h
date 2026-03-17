#include <iostream>
#include "hex.h"
#include "parser.h"

ThreadRunner* runner;

void test_run(std::string name, std::string hex) {
    std::cout << "hex:" << hex << std::endl;
    qb::Engine engine;
    LedDevice device;
    engine.put_device(device);

    auto parser_res = qb::parser::parse(engine, "test", hex);
    if (parser_res.code != 0) {
        std::cout << __ERROR << "parser." << +parser_res.code << " " << name << NC << std::endl;
        return;
    }
    std::cout << parser_res.script->describe();
    
    auto runner_res = engine.make_runner<ThreadRunner>(name, parser_res.script);
    if (!runner_res.ok) {
        std::cout << __ERROR << runner_res.message << " " << name << NC << std::endl;
        return;
    }

    runner = (ThreadRunner*) runner_res.runner;
    signal(SIGINT, [](int signum) { runner->sigint(); });
    runner->run();

    // std::cout << std::endl;
    // std::cout << __OK << " " << name << NC << std::endl;
    // std::cout << parser_res.script->describe();
    // std::cout << std::endl;
}

void test_runner() {

    test_run("USE_NODE", HEX({
        HEADER_QUIMBLOS,
        qb::OpCode::USE_DEVICE, 0x03, 'L', 'E', 'D',
        qb::OpCode::USE_NODE, qb::Type::UINT8, 0x00,
        qb::OpCode::SET, qb::OpBind::SHORT_NODE_NODE, 0xFF, 0x00, qb::Type::UINT8, 0x01,
        qb::OpCode::HOLD, 0x00,
        qb::OpCode::ADD, qb::OpBind::SHORT_NODE_NODE, 0x00, 0x00, qb::Type::UINT8, 0x01,
        qb::OpCode::RELEASE, 0x00,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::ADD, qb::OpBind::SHORT_NODE_NODE, 0x00, 0x00, qb::Type::UINT8, 0x02,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::ADD, qb::OpBind::SHORT_NODE_NODE, 0x00, 0x00, qb::Type::UINT8, 0x03,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::SUB, qb::OpBind::SHORT_NODE_NODE, 0x00, 0x00, qb::Type::UINT8, 0x01,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::MULT, qb::OpBind::SHORT_NODE_NODE, 0x00, 0x00, qb::Type::UINT8, 0x02,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::DIV, qb::OpBind::SHORT_NODE_NODE, 0x00, 0x00, qb::Type::UINT8, 0x02,
        qb::OpCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
        qb::OpCode::LOG, 0x00, qb::Type::UINT8, 0x03
    }));

}