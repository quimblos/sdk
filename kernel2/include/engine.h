#pragma once
#include <unordered_map>
#include <string>
#include "program.h"
#include "runner.h"

namespace qb {
    
    namespace engine {
        struct res_t {
            bool ok;
            std::string message;
        };
    }

    class Engine {
        protected:
            std::unordered_map<std::string, qb::Device*> devices;
            std::unordered_map<std::string, qb::Runner*> runners;

        public:
            ~Engine() {
                for (auto pair : this->devices) {
                    delete pair.second;
                }
                for (auto pair : this->runners) {
                    delete pair.second;
                }
            }

            engine::res_t link_device(qb::Device* device);
            qb::Device* get_device(std::string name) const;
            engine::res_t delete_device(std::string name);
            const std::unordered_map<std::string, qb::Device*>& get_devices() const;

            engine::res_t link_runner(qb::Runner* runner);
            qb::Runner* get_runner(std::string name) const;
            engine::res_t delete_runner(std::string name);
            const std::unordered_map<std::string, qb::Runner*>& get_runners() const;
    };

}