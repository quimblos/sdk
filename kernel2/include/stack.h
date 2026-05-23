#pragma once
#include "method.h"

namespace qb {

    class Stack {
        TypeSolver* solver;
        Method* root = nullptr;
        uint8_t size = 0;
        
        public:
            Stack(TypeSolver& solver)
                : solver(&solver) {}

            ~Stack() {
                auto node = this->root;
                while (node != nullptr) {
                    auto next = (Method*) node->parent;
                    delete node;
                    node = next;
                }
            }

            Method* get_root() { return this->root; };
            uint8_t get_size() { return this->size; };

            bool tick();
            void clear();

            Method* push(const Code* code);
            void pop();
    };

}