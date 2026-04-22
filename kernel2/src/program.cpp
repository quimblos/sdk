#include "program.h"
#include <iostream>
#include <sstream>

qb::Program::Program(
    std::string name,
    std::vector<qb::Instruction*> instructions,
    std::vector<program::Port> ports
):
    name(name)
{
    for (const auto it : instructions) {
        if (it->type == qb::InstructionType::USE_DEVICE) {
            auto instr = (qb::instruction::UseDevice*) it;
            this->devices.push_back(instr->name);
            delete instr;
        }
        else if (it->type == qb::InstructionType::USE_VAR) {
            auto instr = (qb::instruction::UseVar*) it;
            this->variables.push_back(instr->data->copy());
            delete instr;
        }
        else {
            this->instructions.push_back(it);
        }
    }
}

qb::Program::~Program() {
    for (const auto it : this->variables) {
        delete it;
    }
    for (const auto it : this->instructions) {
        delete it;
    }
}

qb::program::res_t qb::Program::make(std::string name, code_t* bytes, code_addr_t code_len) {

    std::vector<qb::Instruction*> instructions;

    code_addr_t addr = 0;
    while (addr < code_len) {
        auto res = qb::Instruction::make(bytes, code_len, addr);
        if (res.code > 0) {
            for (qb::Instruction* it: instructions) {
                delete it;
            }
            return {
                .code = 1,
                .error_addr = addr
            };
        }

        instructions.push_back(res.instr);
        addr = res.next_addr;
    }

    auto program = new qb::Program(name, instructions, {});

    return {
        .code = 0,
        .program = program,
        .error_addr = 0
    };

}

qb::program::res_t qb::Program::make(std::string name, std::string hex) {
    auto bytecode = qb::hex_to_bytecode(hex);
    auto res = qb::Program::make(name, bytecode.bytes, bytecode.length);
    delete[] bytecode.bytes;
    return res;
}

std::string qb::Program::describe() {
    std::stringstream ss;
    ss << "[program: " << this->name << "]" << std::endl;
    ss << " #devices" << std::endl;
    for (port_t i = 0; i < this->devices.size(); i++) {
        ss << "  " << +i << ": " << this->devices.at(i) << std::endl;
    }
    ss << " #variables" << std::endl;
    for (port_t i = 0; i < this->variables.size(); i++) {
        ss << "  " << +i << ": " << this->variables.at(i)->to_str() << std::endl;
    }
    ss << " #instructions" << std::endl;
    for (port_t i = 0; i < this->instructions.size(); i++) {
        ss << "  " << +i << ": " << this->instructions.at(i)->to_str() << std::endl;
    }
    return ss.str();
}