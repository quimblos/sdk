#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include "quimblos.h"

/* Runner */

class ThreadRunner : public qb::Runner {
    
    public:
        ThreadRunner(qb::Engine* engine, std::string name, const qb::Program* program)
            : qb::Runner(engine, name, program) {};
            
        ~ThreadRunner() {
            delete this->thread;
        }

        void run() {
            this->keepRunning = true;
            thread = new std::thread(&ThreadRunner::loop, this);
            thread->join();
        }

        void sigint() {
            std::cout << "SIGINT detected, stopping." << std::endl;
            this->keepRunning = false;
        }

    private:
        std::thread *thread;
        bool keepRunning = false;
        
        static void loop(ThreadRunner* runner) {
            runner->start();
            while (runner->keepRunning && runner->tick()) {
                if (runner->state == qb::runner::State::SLEEPING) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(runner->sleep));
                    runner->wakeup();
                }
            }
        }

};

/* Devices */

class LED8 : public qb::Device {
    public:    

    LED8(): qb::Device("LED8", {
        /* 0x00 */ { "on", qb::data::u8() }
    }) {}
    
    void on_tick() {
        qb::Device::on_tick();
        auto data = qb::data::as_u8(this->variables.at(0).second);
        uint8_t val = data->value;

        std::cout << COLOR_YELLOW;
        for (uint8_t i = 0; i < 8; i++) {
            std::cout << ((val >> (7-i) & 1) ? COLOR_YELLOW : COLOR_GRAY) << "╔═╗ ";
        }
        std::cout << std::endl;
        for (uint8_t i = 0; i < 8; i++) {
            std::cout << ((val >> (7-i) & 1) ? COLOR_YELLOW : COLOR_GRAY) << "╚═╝ ";
        }
        std::cout << COLOR_NC << std::endl;
    }
};

/* Invoke Method */

ThreadRunner* runner;

int invoke(std::string name, std::string hex) {
    std::cout << "- Creating Engine..." << std::endl;
    qb::Engine engine;

    std::cout << "- Linking LED8 device..." << std::endl;
    LED8* device_led8 = new LED8();
    engine.link_device(device_led8);

    std::cout << "- Parsing hexcode: " << hex << std::endl;

    auto program_res = qb::Program::make(name, hex);
    if (program_res.code > 0) {
        std::cout << "[error] program parsing error:" << +program_res.code << std::endl;
        return -1;
    }
    qb::Program* program = program_res.program;
    std::cout << program->describe() << std::endl;

    std::cout << "- Creating " << name << " Runner... " << std::endl;
    runner = new ThreadRunner(&engine, name, program);
    
    std::cout << "- Linking " << name << " Runner... " << std::endl;
    engine.link_runner(runner);

    std::cout << "- Running... " << std::endl;
    signal(SIGINT, [](int signum) { runner->sigint(); });
    runner->run();

    delete program;
    return 0;
}
