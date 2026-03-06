#include "runner.h"

// #define QB_RUNNER_DEBUG

qb::Runner::Runner(
    qb::Engine& engine,
    std::string name,
    const qb::Script* script
):
    engine(engine),
    name(name),
    script(script)
{
    for (const qb::DataType type : this->script->variables) {
        qb::Data port = qb::Data::initialize(type);
        this->variables.emplace_back(
            port.type,
            port.value
        );
    }
}

void qb::Runner::start() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[start]" << std::endl;
    #endif
    this->length = this->script->instructions.size();
    this->cursor = 0;
    this->reset();
}

void qb::Runner::reset() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[reset]" << std::endl;
    #endif
    for (qb::Data& port : this->variables) {
        port.reset();
    }
    this->wakeup();
}

void qb::Runner::wakeup() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[wakeup]" << std::endl;
    #endif
    this->state = qb::runner::State::RUNNING;
    this->output = "";
    this->sleep = 0;
}

bool qb::Runner::tick() {
    if (this->state != qb::runner::State::RUNNING) {
        std::cout << "Not running (" << this->state << ")" << std::endl;
        return false;
    }
    if (this->cursor >= this->length) {
        std::cout << "OK" << std::endl;
        this->state = qb::runner::State::OK;
        return false;
    }
    qb::Instruction* cmd = this->script->instructions.at(this->cursor);
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[tick] " << this->cursor << " -> " << cmd.next << " | ";
    #endif
    
    qb::code_addr_t next = 0xFFFF;

    #ifdef QB_RUNNER_DEBUG
        switch (cmd.code) {
            case qb::OpCode::SET: std::cout << "SET" << std::endl; break;
            case qb::OpCode::SET_AT: std::cout << "SET AT" << std::endl; break;
            case qb::OpCode::HOLD: std::cout << "HOLD" << std::endl; break;
            case qb::OpCode::RELEASE: std::cout << "RELEASE" << std::endl; break;
            case qb::OpCode::GOTO: std::cout << "GOTO" << std::endl; break;
            case qb::OpCode::IF_EQ: std::cout << "IF_EQ" << std::endl; break;
            case qb::OpCode::IF_GT: std::cout << "IF_GT" << std::endl; break;
            case qb::OpCode::IF_GTEQ: std::cout << "IF_GTEQ" << std::endl; break;
            case qb::OpCode::ADD: std::cout << "ADD" << std::endl; break;
            case qb::OpCode::SUB: std::cout << "SUB" << std::endl; break;
            case qb::OpCode::MULT: std::cout << "MULT" << std::endl; break;
            case qb::OpCode::DIV: std::cout << "DIV" << std::endl; break;
            case qb::OpCode::MOD: std::cout << "MOD" << std::endl; break;
            case qb::OpCode::LOG: std::cout << "LOG" << std::endl; break;
            case qb::OpCode::SLEEP: std::cout << "SLEEP" << std::endl; break;
            case qb::OpCode::STOP: std::cout << "STOP" << std::endl; break;
            case qb::OpCode::ERROR: std::cout << "ERROR" << std::endl; break;
        }
    #endif

    next = cmd->run(*this);
    this->update();

    if (next == 0xFFFF) {
        this->state = qb::runner::State::OK;
        return false;
    }

    this->cursor = next;
    return true;
}

void qb::Runner::update() {
    for (uint8_t i = 0; i < this->variables.size(); i++) {
        std::cout << "r" << +i << " ";
        this->variables.at(i).log();
        std::cout << std::endl;
    }
}