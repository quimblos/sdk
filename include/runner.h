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
            std::vector<qb::Data> registers;

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
                for (qb::Data& data : this->registers) {
                    data.purge();
                }
            }           

            virtual void update();
            virtual void destroy() {}

        private:
            qb::Data& get(qb::Cmd& cmd);
    };
}

#include "engine.h"