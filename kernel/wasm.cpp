#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include <emscripten/bind.h>
#include "sdk.h"

struct WASMDeviceNode {
    std::string name;
    std::string type;
    uint16_t arr_length;
};

class WASMDevice : public qb::Device {
    private:
        emscripten::val jsDevice = emscripten::val::undefined();

    public:    

    WASMDevice(
        std::string name,
        std::vector<WASMDeviceNode> nodes
    ): qb::Device(name) {
        for (const auto& it : nodes) {
            if (it.type == "u8") {
                this->add_node(it.name, qb::node::u8());
            }
        }
    }

    void bind(emscripten::val jsDevice) {
        this->jsDevice = jsDevice;
    }

    bool has_i(uint8_t node) {
        return node < this->nodes.size();
    }
    
    void update() {
        auto val = emscripten::val::object();
        for (size_t i = 0; i < this->nodes.size(); i++) {
            qb::Node* node = this->nodes.at(i).second;
            if (node->type == qb::Type::UINT8) {
                uint8_t value = qb::node::as_u8(node);
                val.set(i, emscripten::val(value));
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
        qb::engine::res_t<void> put_device(WASMDevice& device) {
            return qb::Engine::put_device(device);
        }

    int8_t make_runner(std::string name, std::string hex) {
        auto parser_res = qb::parser::parse(*this, name, hex);
        if (parser_res.code > 0) {
            std::cout << "[error] parser:" << +parser_res.code << std::endl;
            return -1;
        }
        auto checker_res = qb::static_checker::check(*this, parser_res.script);
        if (checker_res.code > 0) {
            std::cout << "[error] checker:" << +checker_res.code << std::endl;
            return -1;
        }

        auto runner_res = qb::Engine::make_runner<WASMRunner>(name, parser_res.script);
        if (!runner_res.ok) {
            std::cout << "[error] runner:" << runner_res.message << std::endl;
            return -1;
        }
        return 0;
    }

    WASMRunner& get_runner(std::string name) {
        return *(WASMRunner*)this->runners.at(name);
    }

    qb::engine::res_t<void> delete_runner(std::string name) {
        return qb::Engine::delete_runner(name);
    }
};

using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_module) {
    // STL
    register_vector<std::string>("VectorString");
    register_vector<WASMDeviceNode>("VectorDeviceNode");

    // Structs
    value_object<qb::engine::res_t<void>>("res_Engine")
        .field("ok", &qb::engine::res_t<void>::ok)
        .field("message", &qb::engine::res_t<void>::message);
        
    value_object<WASMDeviceNode>("DeviceNode")
        .field("name", &WASMDeviceNode::name)
        .field("type", &WASMDeviceNode::type)
        .field("arr_length", &WASMDeviceNode::arr_length);

    enum_<qb::runner::State>("RunnerState")
        .value("IDLE", qb::runner::State::IDLE)
        .value("RUNNING", qb::runner::State::RUNNING)
        .value("SLEEPING", qb::runner::State::SLEEPING)
        .value("OK", qb::runner::State::OK)
        .value("ERROR", qb::runner::State::ERROR);

    // Engine
    class_<WASMEngine>("Engine")
        .constructor()
        .function("put_device", &WASMEngine::put_device)
        .function("make_runner", &WASMEngine::make_runner)
        .function("get_runner", &WASMEngine::get_runner)
        .function("delete_runner", &WASMEngine::delete_runner);

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
        .constructor<std::string, std::vector<WASMDeviceNode>>()
        .function("has_i", &WASMDevice::has_i)
        .function("bind", &WASMDevice::bind);
}