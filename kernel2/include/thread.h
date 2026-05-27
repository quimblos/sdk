#pragma once
#include "parser.h"
#include "stack.h"

namespace qb {

    class Node;
    class Thread {

        public:
            enum State {
                IDLE = 0x00,
                RUNNING = 0x01,
                SLEEPING = 0x10,
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
                block(mem::Block(type_def)) {}
        
            virtual ~Thread() {}

            const std::string& get_name() const { return this->name; }
            const mem::Block* get_block() const { return &this->block; }
            Node* get_node() const { return this->node; }
            State get_state() const { return this->state; }
            uint32_t get_sleep() const { return this->sleep; }

            void wakeup();
            void reset();
            void start();

            bool tick();

    };

}