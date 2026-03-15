#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include "memory.h"
#include "device.h"
#include "runner.h"

/**
 * Quimblos Engine
 */

namespace qb {

    namespace engine {
        
        template <class T>
        struct res_t {
            bool ok;
            std::string message;
            T* runner;
        };
    }
        
    class Engine {
        protected:
            std::unordered_map<std::string, qb::Device*> devices;
            std::unordered_map<std::string, qb::Node*> constants;
            std::unordered_map<std::string, qb::Runner*> runners;
    
        public:
            ~Engine() {
                for (std::pair<std::string, qb::Node*> pair : this->constants) {
                    delete pair.second;
                }
                for (std::pair<std::string, qb::Runner*> pair : this->runners) {
                    pair.second->destroy();
                    delete pair.second;
                }
            }

            const std::unordered_map<std::string, qb::Device*>& get_devices() const;
            const std::unordered_map<std::string, qb::Node*>& get_constants() const;
            const std::unordered_map<std::string, qb::Runner*>& get_runners() const;

            qb::Device* get_device(std::string name) const;
            engine::res_t<void> put_device(qb::Device& device);
            engine::res_t<void> delete_device(std::string name);
            
            const qb::Node* get_constant(std::string name) const;
            engine::res_t<void> put_constant(std::string name, qb::Node* data);
            engine::res_t<void> delete_constant(std::string name);
        
            engine::res_t<qb::Runner> get_runner(std::string name);
            template <class T>
            engine::res_t<T> make_runner(std::string name, const qb::Script* script) {
                if (this->runners.contains(name)) {
                    return {
                        .ok = false,
                        .message = "There's already a Runner with the same name",
                        .runner = (T*) this->runners.at(name)
                    };
                }
                T* runner = new T(
                    *this,
                    name,
                    script
                );
                this->runners.emplace(
                    name,
                    runner
                );
                return {
                    .ok = true,
                    .message = "Runner created",
                    .runner = (T*) runner
                };
            }
            engine::res_t<void> delete_runner(std::string name);
        
    };
}