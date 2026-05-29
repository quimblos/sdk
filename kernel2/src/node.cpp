#include "node.h"
#include "runtime.h"

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

void qb::Node::print_debug() const {
    std::cout << "┌─── node ───" << std::endl;
    std::cout << "│ name: " << this->name << std::endl;
    if (this->block.data.size()) {
        std::cout << "│ #memory" << std::endl;
        std::cout << qb::runtime::block_to_str(&this->block, "│ ") << std::endl;
    }
    std::cout << "└───────────────" << std::endl;
}