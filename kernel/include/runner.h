#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include "memory.h"
#include "device.h"
#include "script.h"

#define DEVICE_NAME(D) (D == 0xFE ? "QB" : (D == 0xFF ? "$" : std::to_string(+D)))
#define TARGET_NAME(T) DEVICE_NAME(T->device) << ":" << +T->port << "." << T->index

namespace qb {
    
    class Engine;

    namespace runner {
        enum State {
            IDLE = 0x00,
            RUNNING = 0x01,
            SLEEPING = 0x10,
            OK = 0xF0,
            ERROR = 0xFF
        };

        struct target_t {
            Device* device;
            Node* node;
            index_t index;
        };
    }
    
    class Runner {
    
        protected:
            Engine& engine;
            std::string name;
            const Script* script;
            std::vector<Node*> nodes;

            code_addr_t length = 0;
            code_addr_t cursor = 0;
    
            runner::State state = runner::State::IDLE;
            std::string output = "";
            uint32_t sleep = 0;
    
            void start();
            void reset();
            void wakeup();
            bool tick();
            
        public:
            Runner(Engine& engine, std::string name, const Script* script);
            ~Runner() {
                delete this->script;
                for (Node* node : this->nodes) {
                    delete node;
                }
            }           

            virtual void update() {};
            virtual void destroy() {}

            Device* resolve_device(device_t device);
            runner::target_t resolve_target(OpBind bind, node::Pointer* target);
            Node* resolve_source(OpBind bind, Node* source);

            static Node* get_node(Runner& runner, Device* device, uint8_t port) {
                if (device == nullptr) {
                    return runner.nodes.at(port);
                }
                else {
                    return device->get_node(port);
                }
            }

            static void set_state(Runner& runner, runner::State state) {
                runner.state = state;
            }
            static void set_output(Runner& runner, std::string output) {
                runner.output = output;
            }
            static void set_sleep(Runner& runner, uint32_t sleep) {
                runner.sleep = sleep;
            }

    };

    namespace instruction {

        // Node Manipulation

        struct Set: public Instruction {
            OpBind bind;
            node::Pointer* target;
            Node* source;
            
            Set(OpBind bind, device_t device, port_t port, index_t index, Node* source):
                Instruction(OpCode::SET),
                bind(bind),
                target(node::ptr(device, port, index)),
                source(source) {}
                
            void purge() {
                delete this->source;
            }
            std::string to_str() const {
                std::stringstream ss;
                ss << TARGET_NAME(this->target) << " = " << this->source->to_str();
                return ss.str();
            }

            code_addr_t run(Runner& runner);

        };
        struct Hold: public Instruction {
            device_t device;

            Hold(device_t device):
                Instruction(OpCode::HOLD),
                device(device)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "hold " << DEVICE_NAME(this->device);
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };
        struct Release: public Instruction {
            device_t device;

            Release(device_t device):
                Instruction(OpCode::RELEASE),
                device(device)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "release " << DEVICE_NAME(this->device);
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };

        // Flow Control

        struct GoTo: public Instruction {
            GoTo(code_addr_t next):
                Instruction(OpCode::GOTO)
            {
                this->next = next;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << "goto @" << this->next;
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };

        struct If: public Instruction {
            OpBind bind;
            node::Pointer* target;
            Node* source;
            code_addr_t next_false;
            
            If(OpCode op_code, OpBind bind, device_t device, port_t port, index_t index, Node* source, code_addr_t next, code_addr_t next_false):
                Instruction(op_code),
                bind(bind),
                target(node::ptr(device, port, index)),
                source(source),
                next_false(next_false)
            {
                this->next = next;
            }
            void purge() {
                delete this->source;
            }
        };
        
        struct IfEq: public If {
            IfEq(OpBind bind, device_t device, port_t port, index_t index, Node* source, code_addr_t next, code_addr_t next_false):
                If(OpCode::IF_EQ, bind, device, port, index, source, next, next_false)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "if " << TARGET_NAME(this->target) << " == " << this->source->to_str() << " @" << this->next << " else @" << this->next_false;
                return ss.str();
            }
            
            code_addr_t run(Runner& runner);
        };
        struct IfGt: public If {
            IfGt(OpBind bind, device_t device, port_t port, index_t index, Node* source, code_addr_t next, code_addr_t next_false):
                If(OpCode::IF_GT, bind, device, port, index, source, next, next_false)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "if " << TARGET_NAME(this->target) << " > " << this->source->to_str() << " @" << this->next << " else @" << this->next_false;
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };
        struct IfGtEq: public If {
            IfGtEq(OpBind bind, device_t device, port_t port, index_t index, Node* source, code_addr_t next, code_addr_t next_false):
                If(OpCode::IF_GTEQ, bind, device, port, index, source, next, next_false)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "if " << TARGET_NAME(this->target) << " >= " << this->source->to_str() << " @" << this->next << " else @" << this->next_false;
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };

        // Arithmetic

        struct Math0: public Instruction {
            OpBind bind;
            node::Pointer* target;
            
            Math0(OpCode op_code, OpBind bind, device_t device, port_t port, index_t index):
                Instruction(op_code),
                bind(bind),
                target(node::ptr(device, port, index))
            {}
        };

        struct Math1: public Instruction {
            OpBind bind;
            node::Pointer* target;
            Node* source;
            
            Math1(OpCode op_code, OpBind bind, device_t device, port_t port, index_t index, Node* source):
                Instruction(op_code),
                bind(bind),
                target(node::ptr(device, port, index)),
                source(source)
            {}
            void purge() {
                delete this->source;
            }
        };

        struct Add: public Math1 {
            Add(OpBind bind, device_t device, port_t port, index_t index, Node* source):
                Math1(OpCode::ADD, bind, device, port, index, source)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << TARGET_NAME(this->target) << " += " << this->source->to_str();
                return ss.str();
            }
            
            code_addr_t run(Runner& runner);
        };
        struct Sub: public Math1 {
            Sub(OpBind bind, device_t device, port_t port, index_t index, Node* source):
                Math1(OpCode::SUB, bind, device, port, index, source)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << TARGET_NAME(this->target) << " -= " << this->source->to_str();
                return ss.str();
            }
            
            code_addr_t run(Runner& runner);
        };
        struct Mult: public Math1 {
            Mult(OpBind bind, device_t device, port_t port, index_t index, Node* source):
                Math1(OpCode::MULT, bind, device, port, index, source)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << TARGET_NAME(this->target) << " *= " << this->source->to_str();
                return ss.str();
            }
            
            code_addr_t run(Runner& runner);
        };
        struct Div: public Math1 {
            Div(OpBind bind, device_t device, port_t port, index_t index, Node* source):
                Math1(OpCode::DIV, bind, device, port, index, source)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << TARGET_NAME(this->target) << " /= " << this->source->to_str();
                return ss.str();
            }
            
            code_addr_t run(Runner& runner);
        };
        struct Mod: public Math1 {
            Mod(OpBind bind, device_t device, port_t port, index_t index, Node* source):
                Math1(OpCode::MOD, bind, device, port, index, source)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << TARGET_NAME(this->target) << " %= " << this->source->to_str();
                return ss.str();
            }
            
            code_addr_t run(Runner& runner);
        };
        struct Pow: public Math1 {
            Pow(OpBind bind, device_t device, port_t port, index_t index, Node* source):
                Math1(OpCode::POW, bind, device, port, index, source)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << TARGET_NAME(this->target) << " ^= " << this->source->to_str();
                return ss.str();
            }
            
            code_addr_t run(Runner& runner);
        };

        struct Floor: public Math0 {
            Floor(OpBind bind, device_t device, port_t port, index_t index):
                Math0(OpCode::FLOOR, bind, device, port, index)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "math_floor " << TARGET_NAME(this->target);
                return ss.str();
            }
            
            code_addr_t run(Runner& runner);
        };
        struct Ceil: public Math0 {
            Ceil(OpBind bind, device_t device, port_t port, index_t index):
                Math0(OpCode::FLOOR, bind, device, port, index)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "math_ceil " << TARGET_NAME(this->target);
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };

        // Log

        struct Log: public Instruction {
            device_t device;
            Node* source;

            Log(device_t device, Node* source):
                Instruction(OpCode::LOG),
                device(device),
                source(source)
            {}
            void purge() {
                delete this->source;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << "log " << this->source->to_str();
                return ss.str();
            }
            
            code_addr_t run(Runner& runner);
        };

        // Runner

        struct Sleep: public Instruction {
            uint32_t ms;

            Sleep(uint32_t ms):
                Instruction(OpCode::SLEEP),
                ms(ms)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "sleep " << this->ms;
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };
        struct Return: public Instruction {
            Node* source;

            Return(Node* source):
                Instruction(OpCode::RETURN),
                source(source)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "return " << this->source->to_str();
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };

        // Engine

        struct Reset: public Instruction {
            Reset():
                Instruction(OpCode::RESET)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "reset";
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };
        struct Reboot: public Instruction {
            Reboot():
                Instruction(OpCode::REBOOT)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "reboot";
                return ss.str();
            }

            code_addr_t run(Runner& runner);
        };
    }
}

#include "engine.h"