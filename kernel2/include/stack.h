#pragma once
#include "code.h"

namespace qb {

    namespace exec {

        struct Routine {
            const Routine* parent;
            const Code* code;
            mem::Block block;
            code_addr_t length;
            code_addr_t cursor;

            Routine(
                TypeSolver& solver,
                const Routine* parent,
                const Code* code
            ):
                parent(parent),
                code(code),
                block(Routine::make_block(solver, code))
            {}

            ~Routine() {
                if (this->parent != nullptr) {
                    delete this->parent;
                }
            }

            static mem::Block make_block(TypeSolver& solver, const Code* code);
        };
    
        class Stack {
            TypeSolver* solver;
            Routine* root = nullptr;
            
            public:
                Stack(TypeSolver* solver)
                    : solver(solver) {}

                ~Stack() {
                    if (this->root != nullptr) {
                        delete this->root;
                    }
                }

                Routine* push(const Code* code);
                Routine* pop(const Code* code);
        };

    }

}