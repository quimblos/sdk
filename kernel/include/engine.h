#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include "data.h"
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
            std::unordered_map<std::string, qb::Data> constants;
            std::unordered_map<std::string, qb::Runner*> runners;
    
        public:
            ~Engine() {
                for (std::pair<std::string, qb::Data> pair : this->constants) {
                    pair.second.purge();
                }
                for (std::pair<std::string, qb::Runner*> pair : this->runners) {
                    pair.second->destroy();
                    delete pair.second;
                }
            }

            const std::unordered_map<std::string, qb::Device*>& getDevices() const;
            const std::unordered_map<std::string, qb::Data>& getConstants() const;
            const std::unordered_map<std::string, qb::Runner*>& getRunners() const;

            qb::Device* getDevice(std::string name);
            engine::res_t<void> putDevice(qb::Device& device);
            engine::res_t<void> deleteDevice(std::string name);
            
            const qb::Data* getConstant(std::string name) const;
            engine::res_t<void> putConstant(std::string name, const qb::Data& data);
            engine::res_t<void> deleteConstant(std::string name);
            
            template <class T>
            engine::res_t<T> makeRunner(std::string name, const qb::Script* script) {
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
            
            engine::res_t<qb::Runner> getRunner(std::string name);
    };
}