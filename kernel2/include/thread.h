#pragma once

namespace qb {

    class Thread {

        // Engine
        const Engine* engine;
        const std::string name;

        // Code
        const Code* code;
        const code_addr_t code_size;

        // Memory
        const mem::Block block_const;
        mem::Block block_var;

        // State
        enum State {
            IDLE = 0x00,
            RUNNING = 0x01,
            SLEEPING = 0x10,
            OK = 0xF0,
            ERROR = 0xFF
        } state = State::IDLE;
        code_addr_t cursor = 0;
        uint32_t sleep = 0;

        public:

            Thread(
                const Engine* engine,
                const std::string& name,
                const Code* code
            ):
                engine(engine),
                name(name),
                code(code),
                block_const(Thread::make_const(engine->solver, code)),
                block_var(Thread::make_var(engine->solver, code)) {}
        
                const std::string& get_name() { return this->name; }
                State get_state() { return this->state; }
                code_addr_t get_cursor() { return this->cursor; }
                uint32_t get_sleep() { return this->sleep; }

                void wakeup();
                void reset();
                void start();

                bool tick();

        private:

            static const mem::Block& make_const(const TypeSolver& solver, const Code* code);
            static mem::Block& make_var(const TypeSolver& solver, const Code* code);
    };

}