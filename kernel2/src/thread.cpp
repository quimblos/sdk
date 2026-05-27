#include "thread.h"

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

    return this->stack.tick();
}