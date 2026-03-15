#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <stdint.h>
#include "memory.h"

#define COLOR_GRAY "\033[30m"
#define COLOR_NC "\033[0m"

namespace qb {
    
    /**
     * Device
     */
    
    class Device {
        protected:
            std::string name;
            std::vector<std::pair<std::string, qb::Node*>> nodes;
            bool held = false;
        
        public:
            Device(std::string name):
                name(name)
            {};

            ~Device() {
                for (auto node : this->nodes) {
                    delete node.second;
                }
            }
            
            const std::string& get_name() const {
                return this->name;
            }

            bool has_node(port_t port) const {
                return port < this->nodes.size();
            }

            std::vector<std::pair<std::string, qb::Node*>> get_nodes() const {
                return this->nodes;
            }

            qb::Node* get_node(port_t port) const {
                return this->nodes.at(port).second;
            }
            
            std::string get_node_alias(port_t port) const {
                return this->nodes.at(port).first;
            }

            void hold() {
                this->held = true;
            }

            void release() {
                this->held = false;
            }

            virtual void update() {
                std::stringstream ss;
                ss << COLOR_GRAY;

                ss << "[" << this->name << "]" << std::endl;
                for (size_t i = 0; i < this->nodes.size(); i++) {
                    auto node = this->nodes.at(i);
                    ss << i << " | " <<  node.first << " " << node.second->to_str() << std::endl;
                }

                ss << COLOR_NC;
                std::cout << ss.str();
            }
        
        protected:

            void add_node(std::string name, qb::Node* node) {
                this->nodes.push_back(std::pair(name, node));
            }

    };
    
}