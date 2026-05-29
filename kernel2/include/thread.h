#pragma once
#include "parser.h"
#include "stack.h"

namespace qb {

    class Node;
    class Driver;
    class Thread {

        public:
            enum State {
                IDLE = 0x00,
                RUNNING = 0x01,
                SLEEPING = 0x10,
                WAITING_DRIVER = 0x20,
                OK = 0xF0,
                ERROR = 0xFF
            };

        protected:

        // Metadata
        const std::string name;

        // Context
        Node* node;

        // Code
        const Code* code;

        // State
        State state = State::IDLE;
        uint32_t sleep = 0;

        // Execution
        Stack stack;
        
        // Drivers
        const Driver** drivers;

        // Memory
        mem::Block block;

        public:

            Thread(
                Node* node,
                const std::string& name,
                const Code* code,
                const TypeDef& type_def
            ):
                node(node),
                name(name),
                stack(Stack(this)),
                code(code),
                block(mem::Block(type_def)),
                drivers(new const Driver*[code->drivers.size()])
            {}
        
            virtual ~Thread() {
                delete[] this->drivers;
            }

            void link_driver(block_t i, const Driver* driver) {
                this->drivers[i] = driver;
            }

            const std::string& get_name() const { return this->name; }
            const Code* get_code() const { return this->code; }
            const mem::Block* get_block() const { return &this->block; }
            Driver* get_driver(block_t block) const { return (Driver*) this->drivers[block]; }
            Node* get_node() const { return this->node; }
            State get_state() const { return this->state; }
            uint32_t get_sleep() const { return this->sleep; }
            
            void set_sleep(uint32_t time) {
                this->state = State::SLEEPING;
                this->sleep = time;
            }
            void set_waiting_driver(bool waiting) {
                if (waiting) {
                    this->state = State::WAITING_DRIVER;
                }
                else {
                    this->state = State::RUNNING;
                }
            }
            void set_ok() {
                this->state = State::OK;
            }

            void wakeup();
            void reset();
            void start();

            bool tick();

            void print_debug() const;

    };

}