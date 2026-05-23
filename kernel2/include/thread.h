#pragma once
#include "parser.h"
#include "stack.h"

namespace qb {

    class Thread {

        public:
            enum State {
                IDLE = 0x00,
                RUNNING = 0x01,
                SLEEPING = 0x10,
                OK = 0xF0,
                ERROR = 0xFF
            };

        private:

        // Metadata
        const std::string name;

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
                TypeSolver& solver,
                const std::string& name,
                const Code* code,
                std::vector<type_t> tdxs
            ):
                name(name),
                stack(Stack(solver)),
                code(code),
                block(mem::Block(solver, tdxs)) {}
        
            const std::string& get_name() { return this->name; }
            State get_state() { return this->state; }
            uint32_t get_sleep() { return this->sleep; }

            void wakeup();
            void reset();
            void start();

            bool tick();

    };

}