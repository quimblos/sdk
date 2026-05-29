#include "thread.h"
#include "runtime.h"

#define QB_THREAD_DEBUG

void qb::Thread::wakeup() {
    #ifdef QB_THREAD_DEBUG
        std::cout << "[wakeup]" << std::endl;
    #endif
    this->state = Thread::State::RUNNING;
    this->sleep = 0;
}

void qb::Thread::reset() {
    #ifdef QB_THREAD_DEBUG
        std::cout << "[reset]" << std::endl;
    #endif

    this->block.clear();
    this->stack.clear();
    this->stack.push(this->code);
    this->state = Thread::State::IDLE;
}

void qb::Thread::start() {
    #ifdef QB_THREAD_DEBUG
        std::cout << "[start]" << std::endl;
    #endif

    this->reset();
    this->wakeup();
}

bool qb::Thread::tick() {
    if (this->state != Thread::State::RUNNING) {
        #ifdef QB_THREAD_DEBUG
            std::cout << "[not running: " << this->state << "]" << std::endl;
        #endif
        return false;
    }

    bool out = this->stack.tick();
    return out;
}

void qb::Thread::print_debug() const {
    std::cout << "┌─── thread ───" << std::endl;
    std::cout << "│ name: " << this->name << std::endl;
    switch (this->state) {
        case qb::Thread::State::IDLE: std::cout << "│ state: IDLE" << std::endl; break;
        case qb::Thread::State::RUNNING: std::cout << "│ state: RUNNING" << std::endl; break;
        case qb::Thread::State::SLEEPING: std::cout << "│ state: SLEEPING" << std::endl; break;
        case qb::Thread::State::OK: std::cout << "│ state: OK" << std::endl; break;
        case qb::Thread::State::ERROR: std::cout << "│ state: ERROR" << std::endl; break;
    }
    std::cout << "│ sleep: " << this->sleep << std::endl;
    std::cout << "│ stack: " << +this->stack.get_size() << std::endl;
    if (this->block.data.size()) {
        std::cout << "│ #memory" << std::endl;
        std::cout << qb::runtime::block_to_str(&this->block, "│ ") << std::endl;
    }
    std::cout << "└───────────────" << std::endl;
}