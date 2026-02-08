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

            qb::code_addr_t length = 0;
            qb::code_addr_t cursor = 0;
    
            runner::State state = runner::State::IDLE;
            std::string output = "";
            uint32_t sleep = 0;
    
            void start();
            void wakeup();
            bool tick();
            
        public:
            Runner(qb::Engine& engine, std::string name, const qb::Script* script);
            ~Runner() {
                delete this->script;
            }           

            virtual void destroy() {}
    };
}

#include "engine.h"