#include "runner.h"

#define QB_LOG_DEBUG true

qb::Runner::Runner(
    qb::Engine& engine,
    std::string name,
    const qb::Script* script
):
    engine(engine),
    name(name),
    script(script)
{}

void qb::Runner::start() {
    this->length = this->script->cmds.size();
    this->cursor = 0;

    this->state = qb::runner::State::RUNNING;
    this->output = "";
    this->sleep = 0;
}

void qb::Runner::wakeup() {
    this->state = qb::runner::State::RUNNING;
    this->output = "";
    this->sleep = 0;
}

bool qb::Runner::tick() {
    if (this->state != qb::runner::State::RUNNING) {
        return false;
    }
    if (this->cursor >= this->length) {
        this->state = qb::runner::State::OK;
        return false;
    }
    qb::Cmd cmd = this->script->cmds.at(this->cursor);
    #ifdef QB_LOG_DEBUG
        std::cout << "[tick] " << this->cursor << " -> " << (cmd.next-1) << " | ";
    #endif
    
    switch (cmd.code) {
        case qb::CmdCode::SET:
            #ifdef QB_LOG_DEBUG
                std::cout << "SET" << std::endl;
            #endif
            cmd.device->set(cmd.reg_i, *cmd.value);
            break;
        case qb::CmdCode::GOTO:
            #ifdef QB_LOG_DEBUG
                std::cout << "GOTO" << std::endl;
            #endif
            
            break;
        case qb::CmdCode::IF_EQ:
            #ifdef QB_LOG_DEBUG
                std::cout << "IF_EQ" << std::endl;
            #endif
            
            break;
        case qb::CmdCode::IF_GT:
            #ifdef QB_LOG_DEBUG
                std::cout << "IF_GT" << std::endl;
            #endif
            
            break;
        case qb::CmdCode::IF_GTEQ:
            #ifdef QB_LOG_DEBUG
                std::cout << "IF_GTEQ" << std::endl;
            #endif
            
            break;
        case qb::CmdCode::LOG:
            #ifdef QB_LOG_DEBUG
                std::cout << "LOG" << std::endl;
            #endif
            this->output = cmd.value->as_string();
            break;
        case qb::CmdCode::SLEEP:
            #ifdef QB_LOG_DEBUG
                std::cout << "SLEEP" << std::endl;
            #endif
            this->state = qb::runner::State::SLEEPING;
            this->sleep = cmd.value->as_u32();
            break;
        case qb::CmdCode::STOP:
            #ifdef QB_LOG_DEBUG
                std::cout << "STOP" << std::endl;
            #endif
            this->state = qb::runner::State::OK;
            return true;
        case qb::CmdCode::ERROR:
            #ifdef QB_LOG_DEBUG
                std::cout << "ERROR" << std::endl;
            #endif
            this->state = qb::runner::State::ERROR;
            this->output = cmd.value->as_string();
            return false;
    }

    if (cmd.next == 0) {
        this->state = qb::runner::State::OK;
        return false;
    }

    this->cursor = cmd.next-1;
    return true;
}