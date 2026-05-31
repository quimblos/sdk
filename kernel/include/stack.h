#pragma once
#include "context.h"

namespace qb {

    class Stack {
        Thread* thread;
        Context* root = nullptr;
        uint8_t size = 0;
        
        public:
            Stack(
                Thread* thread
            ):
                thread(thread) {}

            ~Stack() {
                auto node = this->root;
                while (node != nullptr) {
                    auto next = (Context*) node->parent;
                    delete node;
                    node = next;
                }
            }

            Context* get_root() const { return this->root; };
            uint8_t get_size() const { return this->size; };

            bool tick();
            void clear();

            Context* push(const Code* code);
            void pop();
    };

}