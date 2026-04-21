#pragma once
#include <vector>
#include "data.h"

namespace qb {
    
    class Device {
    
        protected:
            const std::string name;
            const std::vector<Data*> variables;

            bool held = false;

            virtual void tick() {}

        public:
            Device(std::string name, std::vector<Data*> variables);
            ~Device() {
                for (qb::Data* var : this->variables) {
                    delete var;
                }
            }

            void hold();
            void release();           

            bool is_held() const {
                return this->held;
            }
            std::string get_name() const {
                return this->name;
            }
            Data* get_variable(port_t port) const {
                if (port >= this->variables.size()) return nullptr;
                return this->variables.at(port);
            }
    };
    
}