#pragma once
#include <unordered_map>
#include "driver.h"

namespace qb {

    namespace node {

        struct res_t {
            enum Code {
                OK = 0x00,
                THREAD_ALREADY_EXISTS,
                THREAD_NOT_FOUND,
                LINK_DRIVER_FAILED
            } code: 8;
            union {
                Thread* thread = nullptr;
            } out;
        };
    }

    class Engine;
    class Node {

        protected:
            Engine* engine;
            std::string name;

            mem::Block block;
            std::unordered_map<std::string, Thread*> threads;

        public:
            Node(
                Engine* engine,
                std::string name,
                const TypeDef& type_def
            ):
                engine(engine),
                name(name),
                block(mem::Block(type_def)) {}
                
            ~Node() {
                for (auto pair : this->threads) {
                    delete pair.second;
                }
            }

            Engine* get_engine() const { return this->engine; }
            const mem::Block* get_block() const { return &this->block; }
            const std::string& get_name() const { return this->name; }
            const std::unordered_map<std::string, Thread*>& get_threads() const { return this->threads; }

            node::res_t link_thread(Thread* thread);
            node::res_t get_thread(std::string name) const;
            node::res_t delete_thread(std::string name);

            void print_debug() const;
    };
    
}