#pragma once
#include "parser.h"
#include "stack.h"

namespace qb {

    class Thread {

        // Engine
        // const Engine* engine;
        const std::string name;

        // State
        enum State {
            IDLE = 0x00,
            RUNNING = 0x01,
            SLEEPING = 0x10,
            OK = 0xF0,
            ERROR = 0xFF
        } state = State::IDLE;
        uint32_t sleep = 0;

        // Execution
        exec::Stack stack;

        public:

            Thread(
                // const Engine* engine,
                const std::string& name
            ):
                // engine(engine),
                name(name) {}
        
            const std::string& get_name() { return this->name; }
            State get_state() { return this->state; }
            uint32_t get_sleep() { return this->sleep; }

            void wakeup();
            void reset();
            void start();

            bool tick();

    };

}