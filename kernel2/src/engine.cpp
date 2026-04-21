#include "engine.h"

const std::unordered_map<std::string, qb::Device*>& qb::Engine::get_devices() const {
    return this->devices;
}

qb::Device* qb::Engine::get_device(std::string name) const {
    if (this->devices.contains(name))
    {
        return this->devices.at(name);
    }
    return nullptr;
}

qb::engine::res_t qb::Engine::put_device(qb::Device* device) {
    auto name = device->get_name();
    this->devices.emplace(name, device);
    return {
        .ok = true,
        .message = "Device updated"
    };
}
qb::engine::res_t qb::Engine::delete_device(std::string name) {
    if (!this->devices.contains(name)) {
        return {
            .ok = false,
            .message = "Device doesn't exist"
        };
    }
    this->devices.erase(name);
    return {
        .ok = true,
        .message = "Device deleted"
    };
}
