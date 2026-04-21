#include "device.h"

qb::Device::Device(std::string name, std::vector<Data*> variables):
    name(name),
    variables(variables) {}

void qb::Device::hold() {
    this->held = true;
}

void qb::Device::release() {
    this->held = false;
}