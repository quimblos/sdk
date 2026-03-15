#include <iostream>
#include "runner.h"
#include "operator.h"

using namespace qb;

#define QB_RUNNER_DEBUG

Runner::Runner(
    Engine& engine,
    std::string name,
    const Script* script
):
    engine(engine),
    name(name),
    script(script)
{
    for (Node* node : this->script->nodes) {
        this->nodes.push_back(node->copy());
    }
}

void Runner::start() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[start]" << std::endl;
    #endif
    this->length = this->script->instructions.size();
    this->cursor = 0;
    this->reset();
}

void Runner::reset() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[reset]" << std::endl;
    #endif
    for (Node* node : this->nodes) {
        node->clear();
    }
    this->wakeup();
}

void Runner::wakeup() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[wakeup]" << std::endl;
    #endif
    this->state = runner::State::RUNNING;
    this->output = "";
    this->sleep = 0;
}

bool Runner::tick() {
    if (this->state != runner::State::RUNNING) {
        std::cout << "Not running (" << this->state << ")" << std::endl;
        return false;
    }
    if (this->cursor >= this->length) {
        std::cout << "OK" << std::endl;
        this->state = runner::State::OK;
        return false;
    }

    Instruction* cmd = this->script->instructions.at(this->cursor);
    
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[tick] " << this->cursor << " | " << cmd->to_str() << std::endl;
    #endif

    code_addr_t next = cmd->run(*this);
    this->update();

    if (next == 0xFFFF) {
        this->state = runner::State::OK;
        return false;
    }

    this->cursor = next;
    return true;
}

Device* Runner::resolve_device(device_t device) {
    return this->script->devices.at(device);
}

runner::target_t Runner::resolve_target(OpBind bind, node::Pointer* target) {
    Device* device = nullptr;
    Node* node;

    if (target->device == 0xFF) {
        node = this->script->nodes.at(target->port);
    }
    // else if (target->device == 0xFE) {
    // }
    else  {
        device = this->script->devices.at(target->device);
        node = device->get_node(target->port);
    }

    if (bind == OpBind::REF_NODE || bind == OpBind::REF_REF) {

        auto pointer = (node::Pointer*) node;
        if (pointer->device == 0xFF) {
            node = this->script->nodes.at(pointer->port);
        }
        // else if (pointer->device == 0xFE) {
        // }
        else  {
            device = this->script->devices.at(pointer->device);
            node = device->get_node(pointer->port);
        }
    }
    
    return {
        .device = device,
        .node = node,
        .index = target->index
    };
}

Node* Runner::resolve_source(OpBind bind, Node* source) {
    if (
        source->type != Type::PTR
        || bind == OpBind::NODE_NODE
        || bind == OpBind::REF_NODE
    ) {
        return source;
    }

    auto pointer = (node::Pointer*) source;
    if (pointer->device == 0xFF) {
        return this->script->nodes.at(pointer->port);
    }
    // else if (pointer->device == 0xFE) {
    // }
    else  {
        Device* device = this->script->devices.at(pointer->device);
        return device->get_node(pointer->port);
    }
}

/*
    Instructions
*/

// Node Manipulation

code_addr_t instruction::Set::run(Runner& runner) {
    auto target = runner.resolve_target(this->bind, this->target);
    Node* source = runner.resolve_source(this->bind, this->source);
    
    auto res = _operator::copy_to(target.node, source, target.index);
    if (target.device != nullptr) {
        target.device->update();
    }
    return this->next;
}

code_addr_t instruction::Hold::run(Runner& runner) {
    Device* device = runner.resolve_device(this->device);
    device->hold();
    return this->next;
}

code_addr_t instruction::Release::run(Runner& runner) {
    Device* device = runner.resolve_device(this->device);
    device->release();
    return this->next;
}

// Flow Control

code_addr_t instruction::GoTo::run(Runner& runner) {
    return this->next;
}

code_addr_t instruction::IfEq::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::IfGt::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::IfGtEq::run(Runner& runner) {
    // TODO
    return this->next;
}

// Arithmetics

code_addr_t instruction::Add::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::Sub::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::Mult::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::Div::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::Mod::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::Pow::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::Floor::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::Ceil::run(Runner& runner) {
    // TODO
    return this->next;
}

// Log

code_addr_t instruction::Log::run(Runner& runner) {
    // TODO
    return this->next;
}

// Runner

code_addr_t instruction::Sleep::run(Runner& runner) {
    Runner::set_state(runner, runner::State::SLEEPING);
    Runner::set_sleep(runner, this->ms);
    return this->next;
}

code_addr_t instruction::Return::run(Runner& runner) {
    Runner::set_state(runner, runner::State::OK);
    // Runner::set_output(runner, this->msg);
    return this->next;
}

// Engine

code_addr_t instruction::Reset::run(Runner& runner) {
    // TODO
    return this->next;
}

code_addr_t instruction::Reboot::run(Runner& runner) {
    // TODO
    return this->next;
}
