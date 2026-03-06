#include <iostream>
#include "sdk.h"
#include "device.h"
#include "runner.h"

ThreadRunner* runner;

int run(std::string name, std::string hex) {
    std::cout << "- Creating Engine..." << std::endl;
    qb::Engine engine;

    std::cout << "- Adding LedBar device..." << std::endl;
    LedBar device;
    engine.putDevice(device);

    std::cout << "- Parsing hexcode: " << hex << std::endl;
    auto parser_out = qb::parser::parse(engine, name, hex);
    if (!parser_out.ok) {
        std::cout << "[error] " << parser_out.message << std::endl;
        return -1;
    }

    std::cout << "- Creating " << name << " runner... " << std::endl;
    auto runner_out = engine.makeRunner<ThreadRunner>(name, parser_out.script);
    if (!runner_out.ok) {
        std::cout << "[error] " << runner_out.message << std::endl;
        return -1;
    }

    std::cout << "- Running... " << std::endl;
    runner = runner_out.runner;
    signal(SIGINT, [](int signum) { runner->sigint(); });
    runner->run();

    return 0;
}