#include "engine.h"
#include "parser.h"

const std::unordered_map<std::string, qb::Device*>& qb::Engine::get_devices() const {
    return this->devices;
}
const std::unordered_map<std::string, qb::Node*>& qb::Engine::get_constants() const {
    return this->constants;
}
const std::unordered_map<std::string, qb::Runner*>& qb::Engine::get_runners() const {
    return this->runners;
}

/* Device */

qb::Device* qb::Engine::get_device(std::string name) const {
    if (this->devices.contains(name))
    {
        return this->devices.at(name);
    }
    return nullptr;
}

qb::engine::res_t<void> qb::Engine::put_device(qb::Device& device) {
    auto name = device.get_name();
    this->devices.emplace(name, &device);
    return {
        .ok = true,
        .message = "Device updated",
        .runner = nullptr
    };
}
qb::engine::res_t<void> qb::Engine::delete_device(std::string name) {
    if (!this->devices.contains(name)) {
        return {
            .ok = false,
            .message = "Device doesn't exist",
            .runner = nullptr
        };
    }
    this->devices.erase(name);
    return {
        .ok = true,
        .message = "Device deleted",
        .runner = nullptr
    };
}

/* Constant */

const qb::Node* qb::Engine::get_constant(std::string name) const {
    if (this->constants.contains(name))
    {
        return this->constants.at(name);
    }
    return nullptr;
}

qb::engine::res_t<void> qb::Engine::put_constant(std::string name, qb::Node* data) {
    if (this->constants.contains(name)) {
        return {
            .ok = false,
            .message = "Constant already exists",
            .runner = nullptr
        };
    }
    this->constants.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(name),
        std::forward_as_tuple(data)
    );
    return {
        .ok = true,
        .message = "Constant updated",
        .runner = nullptr
    };
}

qb::engine::res_t<void> qb::Engine::delete_constant(std::string name) {
    if (!this->constants.contains(name)) {
        return {
            .ok = false,
            .message = "Constant doesn't exist",
            .runner = nullptr
        };
    }
    this->devices.erase(name);
    return {
        .ok = true,
        .message = "Constant deleted",
        .runner = nullptr
    };
}

/* Runner */

qb::engine::res_t<qb::Runner> qb::Engine::get_runner(std::string name) {
    if (!this->runners.contains(name)) {
        return {
            .ok = false,
            .message = "Runner doesn't exist",
            .runner = nullptr
        };
    }
    Runner* runner = this->runners.at(name);
    return {
        .ok = true,
        .runner = runner
    };
}

qb::engine::res_t<void> qb::Engine::delete_runner(std::string name) {
    if (!this->runners.contains(name)) {
        return {
            .ok = false,
            .message = "Runner doesn't exist",
            .runner = nullptr
        };
    }
    this->runners.erase(name);
    return {
        .ok = true,
        .message = "Runner deleted",
        .runner = nullptr
    };
}