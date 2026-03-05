#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include <emscripten/bind.h>
#include "sdk.h"

class WASMDevice : public qb::Device {
    private:
        emscripten::val jsDevice;

    public:    

    WASMDevice(
        std::string name,
        std::vector<std::string> registers
    ): qb::Device(name) {
        for (const auto& it : registers) {
            if (it == "b8") {
                this->addRegister(qb::Data::b8());
            }
        }
    }

    void bind(emscripten::val jsDevice) {
        this->jsDevice = jsDevice;
    }

    bool has_i(uint8_t reg_i) {
        return reg_i < this->registers.size();
    }
    
    void update() {
        auto val = emscripten::val::object();
        for (size_t i = 0; i < this->registers.size(); i++) {
            const auto reg = this->registers.at(i);
            if (reg.type == qb::DataType::BITMASK8) {
                val.set(i, emscripten::val(reg.as_u8()));
            }
        }
        this->jsDevice.call<void>("update", val);
    }
};

class WASMRunner : public qb::Runner {
    
    public:
        WASMRunner(qb::Engine& engine, std::string name, const qb::Script* script)
            : qb::Runner(engine, name, script) {};
            
        void destroy() {}

        qb::runner::State getState() { return this->state; }
        uint32_t getSleep() { return this->sleep; }
        
        void start() {
            qb::Runner::start();
        }
        void reset() {
            qb::Runner::reset();
        }
        void wakeup() {
            qb::Runner::wakeup();
        }
        bool tick() {
            return qb::Runner::tick();
        }

};

class WASMEngine : public qb::Engine {
    public:
        qb::engine::res_t<void> putDevice(WASMDevice& device) {
            return qb::Engine::putDevice(device);
    }

    int8_t makeRunner(std::string name, std::string hex) {
        auto parser_out = qb::parser::parse(*this, name, hex);
        if (!parser_out.ok) {
            std::cout << "[error] " << parser_out.message << std::endl;
            return -1;
        }

        auto runner_out = qb::Engine::makeRunner<WASMRunner>(name, parser_out.script);
        if (!runner_out.ok) {
            std::cout << "[error] " << runner_out.message << std::endl;
            return -1;
        }
        return 0;
    }

    WASMRunner& getRunner(std::string name) {
        return *(WASMRunner*)this->runners.at(name);
    }
};

using namespace emscripten;

// WASMRunner* runner;

// int run(std::string name, std::string hex) {
//     std::cout << "- Creating Engine..." << std::endl;
//     qb::Engine engine;

//     std::cout << "- Adding LedStrip device..." << std::endl;
//     LedStrip device;
//     engine.putDevice(device);

//     std::cout << "- Parsing hexcode: " << hex << std::endl;
//     auto parser_out = qb::parser::parse(engine, name, hex);
//     if (!parser_out.ok) {
//         std::cout << "[error] " << parser_out.message << std::endl;
//         return -1;
//     }

//     std::cout << "- Creating " << name << " runner... " << std::endl;
//     auto runner_out = engine.makeRunner<WASMRunner>(name, parser_out.script);
//     if (!runner_out.ok) {
//         std::cout << "[error] " << runner_out.message << std::endl;
//         return -1;
//     }

//     std::cout << "- Running... " << std::endl;
//     runner = runner_out.runner;
//     runner->run();

//     return 0;
// }

EMSCRIPTEN_BINDINGS(my_module) {
    // STL
    register_vector<std::string>("VectorString");

    // Structs
    value_object<qb::engine::res_t<void>>("res_Engine")
        .field("ok", &qb::engine::res_t<void>::ok)
        .field("message", &qb::engine::res_t<void>::message);

    // value_object<qb::parser::res_t>("res_Parser")
    //     .field("ok", &qb::parser::res_t::ok)
    //     .field("message", &qb::parser::res_t::message)
    //     .field("script", &qb::parser::res_t::script);

    // value_object<qb::Script>("Script")
    //     .field("name", &qb::Script::name);

    enum_<qb::runner::State>("RunnerState")
        .value("IDLE", qb::runner::State::IDLE)
        .value("RUNNING", qb::runner::State::RUNNING)
        .value("SLEEPING", qb::runner::State::SLEEPING)
        .value("OK", qb::runner::State::OK)
        .value("ERROR", qb::runner::State::ERROR);

    // Engine
    class_<WASMEngine>("Engine")
        .constructor()
        .function("putDevice", &WASMEngine::putDevice)
        .function("makeRunner", &WASMEngine::makeRunner)
        .function("getRunner", &WASMEngine::getRunner);

    // Runner
    class_<WASMRunner>("Runner")
        .function("getState", &WASMRunner::getState)
        .function("getSleep", &WASMRunner::getSleep)
        .function("start", &WASMRunner::start)
        .function("tick", &WASMRunner::tick)
        .function("reset", &WASMRunner::reset)
        .function("wakeup", &WASMRunner::wakeup);

    // Device
    class_<WASMDevice>("Device")
        .constructor<std::string, std::vector<std::string>>()
        .function("has_i", &WASMDevice::has_i)
        .function("bind", &WASMDevice::bind);
}