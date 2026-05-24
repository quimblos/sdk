#include "stack.h"

qb::Method* qb::Stack::push(const Code* code){
    auto parent = this->root;
    auto method = new Method(parent, code);
    this->root = method;
    this->size += 1;
    return method;
}

void qb::Stack::pop(){
    auto parent = this->root->parent;
    delete this->root;
    this->root = (qb::Method*) parent;
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