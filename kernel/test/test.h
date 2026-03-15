#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>

#define __OK "\033[32mOK\033[0m "
#define __ERROR "\033[31mERROR."
#define CYAN "\033[36m"
#define PURPLE "\033[33m"
#define NC "\033[0m"

#define DEVICE_0 0x00
#define DEVICE_1 0x01
#define DEVICE_2 0x02
#define PORT_0 0x00
#define PORT_1 0x01
#define PORT_2 0x02

#define VAL(NAME, ...) qb::code_t bytes_##NAME[] = { __VA_ARGS__ }; \
    auto NAME = qb::Node::from_bytes(bytes_##NAME, 32, 0);

class LedDevice : public qb::Device {
    public:    
    LedDevice(): qb::Device("LED") {
        /* 0x00 */ this->add_node("dim", qb::node::u8());
    }
};

class ThreadRunner : public qb::Runner {
    
    public:
        ThreadRunner(qb::Engine& engine, std::string name, const qb::Script* script)
            : qb::Runner(engine, name, script) {};
            
        void destroy() {
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