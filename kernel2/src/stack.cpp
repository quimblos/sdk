#include "stack.h"

qb::Context* qb::Stack::push(const Code* code){
    auto parent = this->root;
    auto context = new Context(this->thread, parent, code);
    this->root = context;
    this->size += 1;
    return context;
}

void qb::Stack::pop(){
    auto parent = this->root->parent;
    delete this->root;
    this->root = (qb::Context*) parent;
    this->size -= 1;
}

bool qb::Stack::tick() {
    return this->root->tick();
}

void qb::Stack::clear() {
    while (this->root) {
        this->pop();
    }
}