#include "engine.h"
#include "parser.h"

const std::unordered_map<std::string, qb::Device*>& qb::Engine::getDevices() const {
    return this->devices;
}
const std::unordered_map<std::string, qb::Data>& qb::Engine::getConstants() const {
    return this->constants;
}
const std::unordered_map<std::string, qb::Runner*>& qb::Engine::getRunners() const {
    return this->runners;
}

/* Device */

qb::Device* qb::Engine::getDevice(std::string name) {
    if (this->devices.contains(name))
    {
        return this->devices.at(name);
    }
    return nullptr;
}

qb::engine::res_t<void> qb::Engine::putDevice(qb::Device& device) {
    auto name = device.getName();
    this->devices.emplace(name, &device);
    return {
        .ok = true,
        .message = "Device updated",
        .runner = nullptr
    };
}
qb::engine::res_t<void> qb::Engine::deleteDevice(std::string name) {
    // TODO: check if it exists first
    this->devices.erase(name);
    return {
        .ok = true,
        .message = "Device deleted",
        .runner = nullptr
    };
}

/* Constant */

const qb::Data* qb::Engine::getConstant(std::string name) const {
    if (this->constants.contains(name))
    {
        return &this->constants.at(name);
    }
    return nullptr;
}

qb::engine::res_t<void> qb::Engine::putConstant(std::string name, const qb::Data& data) {
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
        std::forward_as_tuple(
            data.type,
            data.value
        )
    );
    return {
        .ok = true,
        .message = "Constant updated",
        .runner = nullptr
    };
}

qb::engine::res_t<void> qb::Engine::deleteConstant(std::string name) {
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

qb::engine::res_t<qb::Runner> qb::Engine::getRunner(std::string name) {
    if (!this->constants.contains(name)) {
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