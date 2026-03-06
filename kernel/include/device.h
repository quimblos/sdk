#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include "data.h"

namespace qb {
    
    /**
     * Device
     */
    
    class Device {
        protected:
            std::string name;
            std::vector<qb::Data> registers;
            bool held = false;
        
        public:
            Device(std::string name):
                name(name)
            {};

            ~Device() {
                for (qb::Data port : this->registers) {
                    port.purge();
                }
            }
            
            const std::string& getName() const {
                return this->name;
            }

            bool has_i(uint8_t port) {
                return port < this->registers.size();
            }

            qb::Data& get(uint8_t port) {
                return this->registers.at(port);
            }

            void hold() {
                this->held = true;
            }

            void release() {
                this->held = false;
            }

            virtual void update() {}
        
        protected:

            void addRegister(const qb::Data& data) {
                this->registers.emplace_back(
                    data.type,
                    data.value
                );
            }

    };
    
}