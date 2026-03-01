#include <thread>
#include <chrono>
#include <signal.h>
#include "sdk.h"

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