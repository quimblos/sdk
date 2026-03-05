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
    for (const qb::DataType type : this->script->variable_types) {
        qb::Data reg = qb::Data::of_type(type);
        this->variables.emplace_back(
            reg.type,
            reg.value
        );
    }
}

void qb::Runner::start() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[start]" << std::endl;
    #endif
    this->length = this->script->cmds.size();
    this->cursor = 0;
    this->reset();
}

void qb::Runner::reset() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[reset]" << std::endl;
    #endif
    for (qb::Data& reg : this->variables) {
        reg.reset();
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
    qb::Cmd cmd = this->script->cmds.at(this->cursor);
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[tick] " << this->cursor << " -> " << cmd.next << " | ";
    #endif
    
    qb::code_addr_t next = cmd.next;

    switch (cmd.code) {
        case qb::CmdCode::SET:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "SET" << std::endl;
            #endif
            this->get(cmd).set(*cmd.value);
            if (cmd.device) cmd.device->update();
            else this->update();
            break;
        case qb::CmdCode::GOTO:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "GOTO" << std::endl;
            #endif
            break;
        case qb::CmdCode::IF_EQ:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "IF_EQ" << std::endl;
            #endif
            {
                int8_t d = this->get(cmd).compare(*cmd.value);
                if (d != 0) next = cmd.next_false;
            }
            break;
        case qb::CmdCode::IF_GT:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "IF_GT" << std::endl;
            #endif
            {
                int8_t d = this->get(cmd).compare(*cmd.value);
                if (d <= 0) next = cmd.next_false;
            }
            break;
        case qb::CmdCode::IF_GTEQ:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "IF_GTEQ" << std::endl;
            #endif
            {
                int8_t d = this->get(cmd).compare(*cmd.value);
                if (d < 0) next = cmd.next_false;
            }
            break;
        case qb::CmdCode::SUM:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "SUM" << std::endl;
            #endif
            this->get(cmd).sum(*cmd.value);
            if (cmd.device) cmd.device->update();
            else this->update();
            break;
        case qb::CmdCode::SUB:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "SUB" << std::endl;
            #endif
            this->get(cmd).sub(*cmd.value);
            if (cmd.device) cmd.device->update();
            else this->update();
            break;
        case qb::CmdCode::MULT:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "MULT" << std::endl;
            #endif
            this->get(cmd).mult(*cmd.value);
            if (cmd.device) cmd.device->update();
            else this->update();
            break;
        case qb::CmdCode::DIV:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "DIV" << std::endl;
            #endif
            this->get(cmd).div(*cmd.value);
            if (cmd.device) cmd.device->update();
            else this->update();
            break;
        case qb::CmdCode::LOG:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "LOG" << std::endl;
            #endif
            this->output = cmd.value->as_string();
            break;
        case qb::CmdCode::SLEEP:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "SLEEP " << cmd.value->as_u32() << std::endl;
            #endif
            this->state = qb::runner::State::SLEEPING;
            this->sleep = cmd.value->as_u32();
            break;
        case qb::CmdCode::STOP:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "STOP" << std::endl;
            #endif
            this->state = qb::runner::State::OK;
            return true;
        case qb::CmdCode::ERROR:
            #ifdef QB_RUNNER_DEBUG
                std::cout << "ERROR" << std::endl;
            #endif
            this->state = qb::runner::State::ERROR;
            this->output = cmd.value->as_string();
            return false;
    }

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

qb::Data& qb::Runner::get(qb::Cmd& cmd) {
    if (cmd.device == nullptr) {
        return this->variables.at(cmd.reg_i);
    }
    else {
        return cmd.device->get(cmd.reg_i);
    }
}