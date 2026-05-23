#include "engine.h"

#define ENGINE_ERROR(CODE) \
    return { \
        .code = CODE \
    };

#define ENGINE_OK(OUT) \
    return { \
        .code = qb::engine::res_t::Code::OK, \
        .out = OUT \
    };

qb::engine::res_t qb::Engine::link_driver(qb::Driver* driver) {
    auto name = driver->get_name();
    if (this->drivers.contains(name)) {
        ENGINE_ERROR(qb::engine::res_t::Code::DRIVER_ALREADY_EXISTS)
    }
    this->drivers[name] = driver;
    ENGINE_OK({ .driver = driver });
}

qb::engine::res_t qb::Engine::get_driver(std::string name) const {
    if (!this->drivers.contains(name)) {
        ENGINE_ERROR(qb::engine::res_t::Code::DRIVER_NOT_FOUND)
    }
    qb::Driver* driver = this->drivers.at(name);
    ENGINE_OK({ .driver = driver });
}

qb::engine::res_t qb::Engine::delete_driver(std::string name) {
    if (!this->drivers.contains(name)) {
        ENGINE_ERROR(qb::engine::res_t::Code::DRIVER_NOT_FOUND)
    }
    qb::Driver* driver = this->drivers.at(name);
    delete driver;
    this->drivers.erase(name);
    ENGINE_OK(nullptr);
}

qb::engine::res_t qb::Engine::link_node(qb::Node* node) {
    auto name = node->get_name();
    if (this->nodes.contains(name)) {
        ENGINE_ERROR(qb::engine::res_t::Code::NODE_ALREADY_EXISTS)
    }
    this->nodes[name] = node;
    ENGINE_OK({ .node = node });
}

qb::engine::res_t qb::Engine::get_node(std::string name) const {
    if (!this->nodes.contains(name)) {
        ENGINE_ERROR(qb::engine::res_t::Code::NODE_NOT_FOUND)
    }
    qb::Node* node = this->nodes.at(name);
    ENGINE_OK({ .node = node });
}

qb::engine::res_t qb::Engine::delete_node(std::string name) {
    if (!this->nodes.contains(name)) {
        ENGINE_ERROR(qb::engine::res_t::Code::NODE_NOT_FOUND)
    }
    qb::Node* node = this->nodes.at(name);
    delete node;
    this->nodes.erase(name);
    ENGINE_OK(nullptr);
}
