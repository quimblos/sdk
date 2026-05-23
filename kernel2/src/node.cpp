#include "node.h"

#define NODE_ERROR(CODE) \
    return { \
        .code = CODE \
    };

#define NODE_OK(OUT) \
    return { \
        .code = qb::node::res_t::Code::OK, \
        .out = OUT \
    };

qb::node::res_t qb::Node::link_thread(qb::Thread* thread) {
    auto name = thread->get_name();
    if (this->threads.contains(name)) {
        NODE_ERROR(qb::node::res_t::Code::THREAD_ALREADY_EXISTS)
    }
    this->threads[name] = thread;
    NODE_OK(thread);
}

qb::node::res_t qb::Node::get_thread(std::string name) const {
    if (!this->threads.contains(name)) {
        NODE_ERROR(qb::node::res_t::Code::THREAD_NOT_FOUND)
    }
    qb::Thread* thread = this->threads.at(name);
    NODE_OK(thread);
}

qb::node::res_t qb::Node::delete_thread(std::string name) {
    if (!this->threads.contains(name)) {
        NODE_ERROR(qb::node::res_t::Code::THREAD_NOT_FOUND)
    }
    qb::Thread* thread = this->threads.at(name);
    delete thread;
    this->threads.erase(name);
    NODE_OK(nullptr);
}
