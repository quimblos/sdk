#include "device.h"

qb::Device::Device(std::string name, std::vector<std::pair<std::string, Data*>> variables):
    name(name),
    variables(variables) {}

qb::Device::~Device() {
    for (auto var : this->variables) {
        delete var.second;
    }
}

void qb::Device::tick(qb::Device& device) {
    if (device.held) return;
    device.on_tick();
}

void qb::Device::hold() {
    this->held = true;
}

void qb::Device::release() {
    this->held = false;
}

bool qb::Device::is_held() const {
    return this->held;
}
std::string qb::Device::get_name() const {
    return this->name;
}

std::vector<std::pair<std::string, qb::Data*>> qb::Device::get_variables() const {
    return this->variables;
}
qb::Data* qb::Device::get_variable(port_t port) const {
    if (port >= this->variables.size()) return nullptr;
    return this->variables.at(port).second;
}
bool qb::Device::has_variable(port_t port) const {
    return port < this->variables.size();
}
std::string qb::Device::get_variable_name(port_t port) const {
    if (port >= this->variables.size()) return nullptr;
    return this->variables.at(port).first;
}