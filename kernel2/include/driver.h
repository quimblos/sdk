#pragma once
#include <string>
#include <sstream>
#include "thread.h"

namespace qb {

    namespace driver {

        struct res_t {
            enum Code {
                OK = 0x00,
                NOT_HELD = 0x01,
                HELD_BY_OTHER_THREAD = 0x02
            } code;

            union {
                Thread* thread = nullptr;
            } out;
        };

    }

    class Driver {

        protected:
            std::string name;
            mem::Block block;

            Thread* held_by = nullptr;

        public:

            Driver(
                TypeSolver& solver,
                std::string name,
                std::vector<type_t> tdxs
            ):
                name(name),
                block(mem::Block(solver, tdxs)) {}

            virtual driver::res_t hold(Thread* thread) {
                if (this->held_by == nullptr) {
                    this->held_by = thread;
                    return { .code = driver::res_t::Code::OK };
                }
                else if (this->held_by == thread) {
                    return { .code = driver::res_t::Code::OK };
                }
                else {
                    return { .code = driver::res_t::Code::HELD_BY_OTHER_THREAD, .out = this->held_by };
                }
            }

            virtual driver::res_t release(Thread* thread) {
                if (this->held_by == nullptr) {
                    return { .code = driver::res_t::Code::NOT_HELD };
                }
                else if (this->held_by == thread) {
                    this->held_by = nullptr;
                    return { .code = driver::res_t::Code::OK };
                }
                else {
                    return { .code = driver::res_t::Code::HELD_BY_OTHER_THREAD, .out = this->held_by };
                }
            }

            virtual void log(const Type* type, data_t target) {
                // TODO
            }

            // Getters

            std::string describe() {
                std::stringstream ss;
                // ss << COLOR_GRAY;
                ss << "[" << this->name << "]" << std::endl;
                return ss.str();
            }

            bool is_held(Thread* thread = nullptr) const {
                if (this->held_by == nullptr)
                    return false;
                return this->held_by != thread;
            }
            
            std::string get_name() const { return this->name; };
            const mem::Block& get_block() const { return this->block; };
    };
    
}