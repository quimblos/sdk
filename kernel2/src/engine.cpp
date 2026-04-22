#include "engine.h"

// Devices

qb::engine::res_t qb::Engine::link_device(qb::Device* device) {
    auto name = device->get_name();
    if (this->devices.contains(name))
    {
        return {
            .ok = false,
            .message = "There's already a Device with the same name"
        };
    }
    this->devices.emplace(name, device);
    return {
        .ok = true,
        .message = "Device added"
    };
}

qb::Device* qb::Engine::get_device(std::string name) const {
    if (this->devices.contains(name))
    {
        return this->devices.at(name);
    }
    return nullptr;
}

qb::engine::res_t qb::Engine::delete_device(std::string name) {
    if (!this->devices.contains(name)) {
        return {
            .ok = false,
            .message = "Device doesn't exist"
        };
    }
    
    qb::Device* device = this->devices.at(name);
    delete device;
    this->devices.erase(name);

    return {
        .ok = true,
        .message = "Device deleted"
    };
}

const std::unordered_map<std::string, qb::Device*>& qb::Engine::get_devices() const {
    return this->devices;
}

// Runners

qb::engine::res_t qb::Engine::link_runner(qb::Runner* runner) {
    auto name = runner->get_name();
    if (this->runners.contains(name)) {
        return {
            .ok = false,
            .message = "There's already a Runner with the same name"
        };
    }
    this->runners.emplace(name, runner);
    return {
        .ok = true,
        .message = "Runner added"
    };
}

qb::Runner* qb::Engine::get_runner(std::string name) const {
    if (this->runners.contains(name))
    {
        return this->runners.at(name);
    }
    return nullptr;
}

qb::engine::res_t qb::Engine::delete_runner(std::string name) {
    if (!this->runners.contains(name)) {
        return {
            .ok = false,
            .message = "Runner doesn't exist"
        };
    }
    qb::Runner* runner = this->runners.at(name);
    delete runner;
    this->runners.erase(name);
    return {
        .ok = true,
        .message = "Runner deleted"
    };
}

const std::unordered_map<std::string, qb::Runner*>& qb::Engine::get_runners() const {
    return this->runners;
}
