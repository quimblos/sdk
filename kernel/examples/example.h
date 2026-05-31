#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include "quimblos.h"

/* Thread */

class NativeThread : public qb::Thread {
    
    public:
        NativeThread(qb::Node* node, std::string name, const qb::Code* code, const qb::TypeDef& type_def)
            : qb::Thread(node, name, code, type_def) {};
            
        ~NativeThread() {
            delete this->std_thread;
        }

        void run() {
            this->running = true;
            this->std_thread = new std::thread(&NativeThread::loop, this);
            this->std_thread->join();
        }

        void sigint() {
            std::cout << "SIGINT detected, stopping." << std::endl;
            this->running = false;
        }

    private:
        std::thread *std_thread;
        bool running = false;
        
        static void loop(NativeThread* thread) {
            thread->start();
            while (thread->running && thread->tick()) {
                if (thread->state == qb::Thread::State::SLEEPING) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(thread->sleep));
                    thread->wakeup();
                }
            }
        }

};

/* Devices */

class LED8Driver : public qb::Driver {
    public:    

    LED8Driver(): qb::Driver("LED8", {
        qb::TypeDef::_use(B_TYPE_U8)
    }) {}
    
    void render(qb::port_t port) {
        auto val = *this->block.data.__cpp_get<uint8_t>(0);

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

// /* Invoke Context */

NativeThread* thread;

int invoke(std::string name, std::string hex) {
    std::cout << "- Creating Engine..." << std::endl;
    qb::Engine engine({});

    std::cout << "- Creating and linking LED8 driver..." << std::endl;
    auto driver_led8 = new LED8Driver();
    engine.link_driver(driver_led8);

    std::cout << "- Parsing hexcode: " << hex << std::endl;
    auto parser_res = qb::parser::code(hex);
    if (parser_res.code > 0) {
        std::cout << "[error] " << qb::i18n::parser.at(parser_res.code) << std::endl; \
        return -1;
    }
    qb::Code* code = parser_res.out.code;

    std::cout << "- Parsed code: " << std::endl << std::endl;
    std::cout << code->to_str() << std::endl;

    std::cout << "- Creating and linking '" << name << "' Node to Engine... " << std::endl;
    auto node = new qb::Node(&engine, name, {});
    engine.link_node(node);
    
    std::cout << "- Creating and linking 'main' Thread to '" << name << "' Node... " << std::endl;
    thread = new NativeThread(node, "main", code, {});
    node->link_thread(thread);

    std::cout << "- Joining and Running 'main' Thread... " << std::endl;
    signal(SIGINT, [](int signum) { thread->sigint(); });
    thread->run();

    delete code;
    return 0;
}
