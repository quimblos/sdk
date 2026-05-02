#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include <emscripten/bind.h>
#include "engine.h"

emscripten::val as_emval(qb::Data* data) {
    switch (data->type) {
        case qb::DataType::VOID:
            return emscripten::val::undefined();
            break;
        case qb::DataType::_NULL:
            return emscripten::val::null();
            break;
        case qb::DataType::ERROR: {
            auto error = qb::memory::as_error(data);
            auto val = emscripten::val::object();
            val.set("code", emscripten::val(error->code));
            val.set("message", emscripten::val(error->message));
            return val;
        }
        case qb::DataType::BOOL:
            return emscripten::val(qb::memory::as_bool(data)->value);
        case qb::DataType::UINT8:
            return emscripten::val(qb::memory::as_u8(data)->value);
        case qb::DataType::INT8:
            return emscripten::val(qb::memory::as_i8(data)->value);
        case qb::DataType::UINT16:
            return emscripten::val(qb::memory::as_u16(data)->value);
        case qb::DataType::INT16:
            return emscripten::val(qb::memory::as_i16(data)->value);
        case qb::DataType::UINT32:
            return emscripten::val(qb::memory::as_u32(data)->value);
        case qb::DataType::INT32:
            return emscripten::val(qb::memory::as_i32(data)->value);
        case qb::DataType::FLOAT32:
            return emscripten::val(qb::memory::as_f32(data)->value);
        case qb::DataType::STRING:
            return emscripten::val(qb::memory::as_str(data)->value);
        case qb::DataType::VECTOR:
            return emscripten::val::null();
        case qb::DataType::REF: {
            auto ref = qb::memory::as_ref(data);
            auto val = emscripten::val::object();
            val.set("device", emscripten::val((qb::device_t) ref->device));
            val.set("port", emscripten::val(ref->port));
            val.set("flags", emscripten::val(ref->flags));
            if (ref->slice != nullptr) {
                auto slice = emscripten::val::object();
                slice.set("dims", emscripten::val(ref->slice->dims));
                val.set("slice", slice);
            } 
            return val;
        }
    }
}

struct WASMDeviceData {
    std::string name;
    std::vector<qb::code_t> bytes;
};

class WASMDevice : public qb::Device {
    private:
        emscripten::val jsDevice = emscripten::val::undefined();
        WASMDevice(std::string name, std::vector<std::pair<std::string, qb::Data*>> variables): qb::Device(name, variables) {};
    
    // protected:
    //     std::vector<std::pair<std::string, qb::Data*>> variables;

    public:

        static WASMDevice make(
            std::string name,
            std::vector<WASMDeviceData> data
        ) {
            std::vector<std::pair<std::string, qb::Data*>> variables;
            for (const auto& it : data) {
                auto res = qb::Data::parse(it.bytes.data(), it.bytes.size(), 0);
                variables.push_back(std::make_pair(it.name, res.data->copy()));
                delete res.data;
            }
            return WASMDevice(name, variables);
        }

        void bind(emscripten::val jsDevice) {
            this->jsDevice = jsDevice;
        }

        // bool has_i(uint8_t node) {
        //     return node < this->nodes.size();
        // }
        
        void log(qb::Data* data) {
            this->jsDevice.call<void>("update", as_emval(data));
        }

        void on_tick() {
            auto data = emscripten::val::object();
            for (size_t i = 0; i < this->variables.size(); i++) {
                auto val = emscripten::val::object();
                qb::Data* var = this->variables.at(i).second;
                val.set("index", emscripten::val(i));
                val.set("value", as_emval(var));
                data.set(this->variables.at(i).first, val);
            }
            this->jsDevice.call<void>("on_tick", data);
        }      
};

class WASMRunner : public qb::Runner {
    
    public:
        // WASMRunner(qb::Engine& engine, std::string name, const Program* script)
        //     : qb::Runner(engine, name, script) {};
            
        // void destroy() {}

        qb::runner::State get_state() { return this->state; }
        qb::code_addr_t get_cursor() { return this->cursor; }
        uint32_t get_sleep() { return this->sleep; }
        
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
        qb::engine::res_t link_device(WASMDevice& device) {
            return qb::Engine::link_device(&device);
        }
        WASMDevice& get_device(std::string name) {
            return *(WASMDevice*)qb::Engine::get_device(name);
        }
        void delete_device(std::string name) {
            qb::Engine::delete_device(name);
        }

        qb::engine::res_t make_runner(std::string name, std::string hex) {
            auto res = qb::Program::make(name, hex);
            if (res.code != 0) return {
                .ok = false,
                .message = "Failed to parse bytecode"
            };
            qb::Runner* runner = new qb::Runner(this, name, res.program);
            return qb::Engine::link_runner(runner);
        }
        WASMRunner& get_runner(std::string name) {
            return *(WASMRunner*)this->runners.at(name);
        }
        void delete_runner(std::string name) {
            qb::Engine::delete_runner(name);
        }
};

using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_module) {
    // STL
    register_vector<qb::code_t>("VectorCode");
    register_vector<WASMDeviceData>("VectorDeviceData");

    // Structs
    value_object<qb::engine::res_t>("res_Engine")
        .field("ok", &qb::engine::res_t::ok)
        .field("message", &qb::engine::res_t::message);
        
    value_object<WASMDeviceData>("DeviceData")
        .field("name", &WASMDeviceData::name)
        .field("bytes", &WASMDeviceData::bytes);

    enum_<qb::runner::State>("RunnerState")
        .value("IDLE", qb::runner::State::IDLE)
        .value("RUNNING", qb::runner::State::RUNNING)
        .value("SLEEPING", qb::runner::State::SLEEPING)
        .value("OK", qb::runner::State::OK)
        .value("ERROR", qb::runner::State::ERROR);

    // Engine
    class_<WASMEngine>("Engine")
        .constructor()
        .function("link_device", &WASMEngine::link_device)
        .function("get_device", &WASMEngine::get_device)
        .function("delete_device", &WASMEngine::delete_device)
        .function("make_runner", &WASMEngine::make_runner)
        .function("get_runner", &WASMEngine::get_runner)
        .function("delete_runner", &WASMEngine::delete_runner);

    // Runner
    class_<WASMRunner>("Runner")
        .function("start", &WASMRunner::start)
        .function("reset", &WASMRunner::reset)
        .function("wakeup", &WASMRunner::wakeup)
        .function("tick", &WASMRunner::tick)
        .function("get_state", &WASMRunner::get_state)
        .function("get_cursor", &WASMRunner::get_cursor)
        .function("get_sleep", &WASMRunner::get_sleep)
        // .function("get_output", &qb::Runner::get_output)
        .function("get_name", &qb::Runner::get_name)
        .function("get_devices", &qb::Runner::get_devices)
        .function("get_variables", &qb::Runner::get_variables);

    // Device
    class_<WASMDevice>("Device")
        .class_function("make", &WASMDevice::make)
        .function("bind", &WASMDevice::bind)
        .function("has_variable", &qb::Device::has_variable);

}