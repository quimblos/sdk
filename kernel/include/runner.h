#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include "data.h"
#include "device.h"
#include "script.h"

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
    }
    
    class Runner {
    
        protected:
            qb::Engine& engine;
            std::string name;
            const qb::Script* script;
            std::vector<qb::Data> variables;

            qb::code_addr_t length = 0;
            qb::code_addr_t cursor = 0;
    
            runner::State state = runner::State::IDLE;
            std::string output = "";
            uint32_t sleep = 0;
    
            void start();
            void reset();
            void wakeup();
            bool tick();
            
        public:
            Runner(qb::Engine& engine, std::string name, const qb::Script* script);
            ~Runner() {
                delete this->script;
                for (qb::Data& data : this->variables) {
                    data.purge();
                }
            }           

            virtual void update();
            virtual void destroy() {}

            static qb::Data& data(qb::Runner& runner, qb::Device* device, uint8_t port) {
                if (device == nullptr) {
                    return runner.variables.at(port);
                }
                else {
                    return device->get(port);
                }
            }

            static void setState(qb::Runner& runner, qb::runner::State state) {
                runner.state = state;
            }
            static void setOutput(qb::Runner& runner, std::string output) {
                runner.output = output;
            }
            static void setSleep(qb::Runner& runner, uint32_t sleep) {
                runner.sleep = sleep;
            }

    };

    namespace instruction {

        struct Set: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::Data data;
            
            Set(qb::Device* device, uint8_t port, qb::Data data): Instruction(qb::OpCode::SET), device(device), port(port), data(data) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                port.set(this->data);
                this->device->update();
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };
        struct SetAt: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::Data data;
            qb::Data index;

            SetAt(qb::Device* device, uint8_t port, qb::Data data, qb::Data index): Instruction(qb::OpCode::SET_AT), device(device), port(port), data(data), index(index) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                port.set(this->data, &this->index);
                this->device->update();
                return this->next;
            }
            void purge() {
                this->data.purge();
                this->index.purge();
            }
        };
        struct Hold: public Instruction {
            qb::Device* device;

            Hold(qb::Device* device): Instruction(qb::OpCode::HOLD), device(device) {}

            code_addr_t run(qb::Runner& runner) {
                this->device->hold();
                return this->next;
            }
        };
        struct Release: public Instruction {
            qb::Device* device;

            Release(qb::Device* device): Instruction(qb::OpCode::RELEASE), device(device) {}

            code_addr_t run(qb::Runner& runner) {
                this->device->release();
                return this->next;
            }
        };

        struct Goto: public Instruction {
            Goto(): Instruction(qb::OpCode::GOTO) {}
            code_addr_t run(qb::Runner& runner) {
                return this->next;
            }
        };
        struct IfEq: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::code_addr_t next_false;
            qb::Data data;
            
            IfEq(qb::Device* device, uint8_t port, qb::Data data): Instruction(qb::OpCode::IF_EQ), device(device), port(port), data(data) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                int8_t d = port.compare(this->data);
                if (d != 0) return this->next_false;
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };
        struct IfGt: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::code_addr_t next_false;
            qb::Data data;

            IfGt(qb::Device* device, uint8_t port, qb::Data data): Instruction(qb::OpCode::IF_GT), device(device), port(port), data(data) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                int8_t d = port.compare(this->data);
                if (d <= 0) return this->next_false;
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };
        struct IfGtEq: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::code_addr_t next_false;
            qb::Data data;

            IfGtEq(qb::Device* device, uint8_t port, qb::Data data): Instruction(qb::OpCode::IF_GTEQ), device(device), port(port), data(data) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                int8_t d = port.compare(this->data);
                if (d < 0) return this->next_false;
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };

        struct Add: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::Data data;

            Add(qb::Device* device, uint8_t port, qb::Data data): Instruction(qb::OpCode::ADD), device(device), port(port), data(data) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                port.add(this->data);
                this->device->update();
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };
        struct Sub: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::Data data;

            Sub(qb::Device* device, uint8_t port, qb::Data data): Instruction(qb::OpCode::SUB), device(device), port(port), data(data) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                port.add(this->data);
                this->device->update();
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };
        struct Mult: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::Data data;

            Mult(qb::Device* device, uint8_t port, qb::Data data): Instruction(qb::OpCode::MULT), device(device), port(port), data(data) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                port.add(this->data);
                this->device->update();
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };
        struct Div: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::Data data;

            Div(qb::Device* device, uint8_t port, qb::Data data): Instruction(qb::OpCode::DIV), device(device), port(port), data(data) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                port.add(this->data);
                this->device->update();
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };
        struct Mod: public Instruction {
            qb::Device* device;
            uint8_t port;
            qb::Data data;

            Mod(qb::Device* device, uint8_t port, qb::Data data): Instruction(qb::OpCode::MOD), device(device), port(port), data(data) {}

            code_addr_t run(qb::Runner& runner) {
                auto port = qb::Runner::data(runner, this->device, this->port);
                port.add(this->data);
                this->device->update();
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };

        struct Log: public Instruction {
            qb::Device* device;
            qb::Data data;

            Log(qb::Device* device, qb::Data data): Instruction(qb::OpCode::LOG), device(device), data(data) {}
            
            code_addr_t run(qb::Runner& runner) {
                // TODO
                return this->next;
            }
            void purge() {
                this->data.purge();
            }
        };

        struct Sleep: public Instruction {
            uint32_t ms;

            Sleep(uint32_t ms): Instruction(qb::OpCode::SLEEP), ms(ms) {}

            code_addr_t run(qb::Runner& runner) {
                qb::Runner::setState(runner, qb::runner::State::SLEEPING);
                qb::Runner::setSleep(runner, this->ms);
                return this->next;
            }
        };
        struct Stop: public Instruction {
            std::string msg;

            Stop(std::string msg): Instruction(qb::OpCode::STOP), msg(msg) {}

            code_addr_t run(qb::Runner& runner) {
                qb::Runner::setState(runner, qb::runner::State::OK);
                qb::Runner::setOutput(runner, this->msg);
                return this->next;
            }
        };
        struct Error: public Instruction {
            std::string msg;

            Error(std::string msg): Instruction(qb::OpCode::ERROR), msg(msg) {}

            code_addr_t run(qb::Runner& runner) {
                qb::Runner::setState(runner, qb::runner::State::ERROR);
                qb::Runner::setOutput(runner, this->msg);
                return this->next;
            }
        };

        struct Reset: public Instruction {
            Reset(): Instruction(qb::OpCode::RESET) {}

            code_addr_t run(qb::Runner& runner) {
                // TODO
                return this->next;
            }
        };
        struct Reboot: public Instruction {
            Reboot(): Instruction(qb::OpCode::REBOOT) {}

            code_addr_t run(qb::Runner& runner) {
                // TODO
                return this->next;
            }
        };
    }
}

#include "engine.h"