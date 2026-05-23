#pragma once
#include <unordered_map>
#include "driver.h"

namespace qb {

    namespace node {

        struct res_t {
            enum Code {
                OK = 0x00,
                THREAD_ALREADY_EXISTS = 0x01,
                THREAD_NOT_FOUND = 0x02
            } code: 8;
            union {
                Thread* thread = nullptr;
            } out;
        };
    }

    class Node {

        protected:
            std::string name;
            std::unordered_map<std::string, Thread*> threads;

        public:
            Node(std::string name): name(name) {}
            ~Node() {
                for (auto pair : this->threads) {
                    delete pair.second;
                }
            }

            const std::string& get_name() const { return this->name; }
            const std::unordered_map<std::string, Thread*>& get_threads() const { return this->threads; }

            node::res_t link_thread(Thread* thread);
            node::res_t get_thread(std::string name) const;
            node::res_t delete_thread(std::string name);
    };
    
}