#pragma once
#include <unordered_map>
#include <string>
#include "device.h"

namespace qb {
    

    class Runner;
    namespace engine {
        
        struct res_t {
            bool ok;
            std::string message;
            qb::Runner* runner;
        };
    }

    class Engine {
        protected:
            std::unordered_map<std::string, qb::Device*> devices;

        public:
            ~Engine() {
                for (std::pair<std::string, qb::Device*> pair : this->devices) {
                    delete pair.second;
                }
            }

            const std::unordered_map<std::string, qb::Device*>& get_devices() const;
            qb::Device* get_device(std::string name) const;
            engine::res_t put_device(qb::Device* device);
            engine::res_t delete_device(std::string name);
    };

}