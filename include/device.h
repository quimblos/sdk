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
        
        public:
            Device(std::string name):
                name(name)
            {};

            ~Device() {
                for (qb::Data reg : this->registers) {
                    reg.purge();
                }
            }
            
            const std::string& getName() const {
                return this->name;
            }

            virtual void set(uint8_t reg_i, qb::Data& value) {
                this->registers.at(reg_i).set(value);
            }
        
        protected:

            void addRegister(const qb::Data& data) {
                this->registers.emplace_back(
                    data.type,
                    data.value
                );
            }

    };
    
}