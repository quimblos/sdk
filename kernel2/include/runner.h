#pragma once
#include "program.h"
#include "engine.h"
#include "operator.h"

#define DEVICE_RUNNER 0b01111111
#define DEVICE_ENGINE 0b01111110

namespace qb {

    namespace runner {
        enum State {
            IDLE = 0x00,
            RUNNING = 0x01,
            SLEEPING = 0x10,
            OK = 0xF0,
            ERROR = 0xFF
        };

        struct res_t {
            bool ok;
            std::string message;
        };
    }
    
    class Runner {

        protected:
            // State
            runner::State state = runner::State::IDLE;
            code_addr_t cursor = 0;
            uint32_t sleep = 0;
            Data* output = nullptr;

        public:
            // Engine
            const Engine* engine;

            // Program
            const std::string name;
            const Program* program;
            const code_addr_t length;

            // Runtime
            std::vector<Device*> devices;
            std::vector<Data*> variables;

        public:
            Runner(Engine& engine, std::string name, Program* program);
            ~Runner();
    
            void wakeup();
            void reset();
            void start();

            bool tick();

            runner::State get_state() { return this->state; }
            code_addr_t get_cursor() { return this->cursor; }
            code_addr_t get_sleep() { return this->sleep; }
            Data* get_output() { return this->output; }
        
        protected:
            data_t resolve_data(const Data* ref) const;
            data_t resolve_ref(const data::Reference* ref) const;

            code_addr_t run_instruction(Instruction* instruction);
            code_addr_t _return(Data* out);
                   
    };

}