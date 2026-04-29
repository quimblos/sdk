#pragma once
#include <iostream>
#include <vector>
#include "data.h"
#include "cli.h"

namespace qb {
    
    class Device {
    
        protected:
            const std::string name;
            const std::vector<std::pair<std::string, Data*>> variables;

            bool held = false;

            virtual void tick() {}

        public:
            Device(std::string name, std::vector<std::pair<std::string, Data*>> variables);
            ~Device();

            static void tick(Device& device);

            void hold();
            void release();       

            // Virtual Behaviors

            virtual void log(Data* data) {
                std::cout << COLOR_GRAY << "[" << this->name << "] " << data->to_str() << COLOR_NC << std::endl;
            }

            virtual void on_tick() {
                std::cout << COLOR_GRAY;
                std::cout << "[" << this->name << "]" << std::endl;
                for (size_t i = 0; i < this->variables.size(); i++) {
                    auto var = this->variables.at(i);
                    std::cout << i << " | " <<  var.first << " " << var.second->to_str() << std::endl;
                }
                std::cout << COLOR_NC;
            }

            // Getters

            bool is_held() const;
            std::string get_name() const;

            std::vector<std::pair<std::string, Data*>> get_variables() const;
            Data* get_variable(port_t port) const;
            bool has_variable(port_t port) const;
            std::string get_variable_name(port_t port) const;
    };
    
}