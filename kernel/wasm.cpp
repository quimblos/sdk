#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include <emscripten/bind.h>
#include "engine.h"

emscripten::val as_emval(const qb::Type* type, qb::data_t value) {
    return emscripten::val::undefined();
    // switch (data->type) {
    //     case qb::DataType::VOID:
    //         return emscripten::val::undefined();
    //         break;
    //     case qb::DataType::_NULL:
    //         return emscripten::val::null();
    //         break;
    //     case qb::DataType::ERROR: {
    //         auto error = qb::mem::as_error(data);
    //         auto val = emscripten::val::object();
    //         val.set("code", emscripten::val(error->code));
    //         val.set("message", emscripten::val(error->message));
    //         return val;
    //     }
    //     case qb::DataType::BOOL:
    //         return emscripten::val(qb::mem::as_bool(data)->value);
    //     case qb::DataType::UINT8:
    //         return emscripten::val(qb::mem::as_u8(data)->value);
    //     case qb::DataType::INT8:
    //         return emscripten::val(qb::mem::as_i8(data)->value);
    //     case qb::DataType::UINT16:
    //         return emscripten::val(qb::mem::as_u16(data)->value);
    //     case qb::DataType::INT16:
    //         return emscripten::val(qb::mem::as_i16(data)->value);
    //     case qb::DataType::UINT32:
    //         return emscripten::val(qb::mem::as_u32(data)->value);
    //     case qb::DataType::INT32:
    //         return emscripten::val(qb::mem::as_i32(data)->value);
    //     case qb::DataType::FLOAT32:
    //         return emscripten::val(qb::mem::as_f32(data)->value);
    //     case qb::DataType::STRING:
    //         return emscripten::val(qb::mem::as_str(data)->value);
    //     case qb::DataType::VECTOR:
    //         return emscripten::val::null();
    //     case qb::DataType::REF: {
    //         auto ref = qb::mem::as_ref(data);
    //         auto val = emscripten::val::object();
    //         val.set("device", emscripten::val((qb::device_t) ref->device));
    //         val.set("port", emscripten::val(ref->port));
    //         val.set("flags", emscripten::val(ref->flags));
    //         if (ref->slice != nullptr) {
    //             auto slice = emscripten::val::object();
    //             slice.set("dims", emscripten::val(ref->slice->dims));
    //             val.set("slice", slice);
    //         } 
    //         return val;
    //     }
    // }
}

struct WASMDriverPortDef {
    std::string name;
    qb::TypeDef type_def;
};

class WASMDriver : public qb::Driver {
    private:
        emscripten::val __js_instance__ = emscripten::val::undefined();
        
    public:
        WASMDriver(
            std::string name,
            std::vector<WASMDriverPortDef> port_defs
        ): qb::Driver(name, make_type_defs(port_defs)) {};

        static const std::vector<qb::TypeDef> make_type_defs(
            std::vector<WASMDriverPortDef> port_defs
        ) {
            std::vector<qb::TypeDef> type_defs;
            for (const auto& port : port_defs) {
                type_defs.push_back(port.type_def);
            }
            return type_defs;
        }

        void bind(emscripten::val __js_instance__) {
            this->__js_instance__ = __js_instance__;
        }

        void log(const qb::Type* type, qb::data_t value) {
            this->__js_instance__.call<void>("log", as_emval(type, value));
        }

        void render(qb::port_t port) {
            auto data = emscripten::val::object();
            for (size_t i = 0; i < this->block.data.size(); i++) {
                auto item = emscripten::val::object();
                auto type = this->block.data.get_type(i);
                auto value = this->block.data.get(i);

                item.set("index", emscripten::val(i));
                item.set("type", emscripten::val(type->to_str()));
                item.set("value", as_emval(type, value));

                data.set(i, item);
            }
            this->__js_instance__.call<void>("render", data);
        }      
};

class WASMThread : public qb::Thread {
    
    public:
        qb::Thread::State get_state() { return this->state; }
        uint32_t get_sleep() { return this->sleep; }
        
        void wakeup() {
            qb::Thread::wakeup();
        }
        void reset() {
            qb::Thread::reset();
        }
        void start() {
            qb::Thread::start();
        }
        bool tick() {
            return qb::Thread::tick();
        }

};

class WASMEngine;
class WASMNode : public qb::Node {
    
    public:
        WASMEngine& get_engine() {
            return *(WASMEngine*)qb::Node::get_engine();
        }
        
        const std::string& get_name() {
            return qb::Node::get_name();
        }
        
        bool link_thread(WASMThread& thread) {
            auto res = qb::Node::link_thread(&thread);
            return res.code == 0;
        }

        bool delete_thread(const std::string& name) {
            auto res = qb::Node::delete_thread(name);
            return res.code == 0;
        }

};

class WASMEngine : public qb::Engine {

    public:
        bool link_driver(WASMDriver& driver) {
            auto res = qb::Engine::link_driver(&driver);
            return res.code == 0;
        }
        WASMDriver& get_driver(std::string name) {
            auto res = qb::Engine::get_driver(name);
            return *(WASMDriver*)res.out.driver;
        }
        bool delete_driver(std::string name) {
            auto res = qb::Engine::delete_driver(name);
            return res.code == 0;
        }

        bool link_node(WASMNode& node) {
            auto res = qb::Engine::link_node(&node);
            return res.code == 0;
        }
        WASMNode& get_node(std::string name) {
            auto res = qb::Engine::get_node(name);
            return *(WASMNode*)res.out.node;
        }
        bool delete_node(std::string name) {
            auto res = qb::Engine::delete_node(name);
            return res.code == 0;
        }
};

using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_module) {
    // STL
    register_vector<qb::TypeDef>("VectorTypeDef");
    register_vector<WASMDriverPortDef>("VectorWASMDriverPortDef");

    // Structs
    enum_<qb::TypeKind>("TypeKind")
        .value("VOID", qb::TypeKind::VOID)
        .value("BOOL", qb::TypeKind::BOOL)
        .value("INT", qb::TypeKind::INT)
        .value("FLOAT", qb::TypeKind::FLOAT)
        .value("STRING", qb::TypeKind::STRING)
        .value("REF", qb::TypeKind::REF)
        .value("REF_SLICE", qb::TypeKind::REF_SLICE)
        .value("VECTOR", qb::TypeKind::VECTOR)
        .value("MAP", qb::TypeKind::MAP)
        .value("STRUCT", qb::TypeKind::STRUCT)
        .value("EVENT", qb::TypeKind::EVENT)
        .value("FN", qb::TypeKind::FN);
    value_object<qb::TypeDef>("TypeDef")
        .field("add", &qb::TypeDef::add)
        .field("use", &qb::TypeDef::use)
        .field("is_const", &qb::TypeDef::is_const);
    value_object<qb::Type>("Type")
        .field("kind", &qb::Type::kind)
        .field("flags", &qb::Type::flags)
        .field("schema", &qb::Type::schema);
        
    enum_<qb::Thread::State>("ThreadState")
        .value("IDLE", qb::Thread::State::IDLE)
        .value("RUNNING", qb::Thread::State::RUNNING)
        .value("SLEEPING", qb::Thread::State::SLEEPING)
        .value("WAITING_DRIVER", qb::Thread::State::WAITING_DRIVER)
        .value("OK", qb::Thread::State::OK)
        .value("ERROR", qb::Thread::State::ERROR);

    value_object<WASMDriverPortDef>("WASMDriverPortDef")
        .field("name", &WASMDriverPortDef::name)
        .field("type_def", &WASMDriverPortDef::type_def);

    // Driver
    class_<WASMDriver>("Driver")
        .constructor()
        .function("bind", &WASMDriver::bind)
        .function("log", &WASMDriver::log)
        .function("render", &WASMDriver::render);

    // Thread
    class_<WASMThread>("Thread")
        .constructor()
        .function("get_state", &WASMThread::get_state)
        .function("get_sleep", &WASMThread::get_sleep)
        .function("wakeup", &WASMThread::wakeup)
        .function("reset", &WASMThread::reset)
        .function("start", &WASMThread::start)
        .function("tick", &WASMThread::tick);

    // Node
    class_<WASMNode>("Node")
        .constructor()
        .function("get_engine", &WASMNode::get_engine)
        .function("get_name", &WASMNode::get_name)
        .function("link_thread", &WASMNode::link_thread)
        .function("delete_thread", &WASMNode::delete_thread);

    // Engine
    class_<WASMEngine>("Engine")
        .constructor()
        .function("link_driver", &WASMEngine::link_driver)
        .function("get_driver", &WASMEngine::get_driver)
        .function("delete_driver", &WASMEngine::delete_driver)
        .function("link_node", &WASMEngine::link_node)
        .function("get_node", &WASMEngine::get_node)
        .function("delete_node", &WASMEngine::delete_node);
}