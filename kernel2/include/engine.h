#pragma once
#include <map>
#include "node.h"

namespace qb {

    namespace engine {

        struct res_t {
            enum Code {
                OK = 0x00,
                DRIVER_ALREADY_EXISTS = 0x01,
                DRIVER_NOT_FOUND = 0x02,
                NODE_ALREADY_EXISTS = 0x03,
                NODE_NOT_FOUND = 0x04,
            } code: 8;
            union {
                Driver* driver;
                Node* node;
            } out;
        };
    }

    class Engine {

        protected:
            mem::Block block;
            std::unordered_map<std::string, Driver*> drivers;
            std::unordered_map<std::string, Node*> nodes;

        public:
            Engine(
                TypeDef type_def
            ): block(mem::Block(
                qb::TypeDef::block({
                    // TODO: merge schema
                    qb::TypeDef::_use(B_TYPE_BOOL),
                    qb::TypeDef::_use(B_TYPE_BOOL)
                })
            )) {
                this->block.data.__cpp_set<bool>(0, false);
                this->block.data.__cpp_set<bool>(1, true);
            }
                
            ~Engine() {
                for (auto pair : this->drivers) {
                    delete pair.second;
                }
                for (auto pair : this->nodes) {
                    delete pair.second;
                }
            }

            const std::unordered_map<std::string, Driver*>& get_drivers() const {
                return this->drivers;
            }
            const std::unordered_map<std::string, Node*>& get_nodes() const {
                return this->nodes;
            }
            const mem::Block* get_block() { return &this->block; }

            engine::res_t link_driver(Driver* driver);
            engine::res_t get_driver(std::string name) const;
            engine::res_t delete_driver(std::string name);

            engine::res_t link_node(Node* node);
            engine::res_t get_node(std::string name) const;
            engine::res_t delete_node(std::string name);

            void print_debug() const;
    };
    
}