#include "program.h"
#include <iostream>
#include <sstream>

#define QB_PROGRAM_PARSE_DEBUG

#ifdef QB_PROGRAM_PARSE_DEBUG
    #include <iostream>
#endif

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

    #ifdef QB_PROGRAM_PARSE_DEBUG
        std::cout << "parser length: " << code_len << std::endl;
    #endif

    if (code_len < 4) {
        #ifdef QB_PROGRAM_PARSE_DEBUG
            std::cout << "parser invalid header" << std::endl;
        #endif
        return {
            .code = QB_CODE_R_PARSE_FAILED_INVALID_HEADER,
            .error_addr = 0
        };
    }

    if (bytes[0] != 'q' || bytes[1] != 'b' || bytes[2] != 0 || bytes[3] != 0) {
        #ifdef QB_PROGRAM_PARSE_DEBUG
            std::cout << "parser invalid header" << std::endl;
        #endif
        return {
            .code = QB_CODE_R_PARSE_FAILED_INVALID_HEADER,
            .error_addr = 0
        };
    }

    code_addr_t addr = 4;
    while (addr < code_len) {
        #ifdef QB_PROGRAM_PARSE_DEBUG
            std::cout << "parser@" << addr << " = " << +bytes[addr] << std::endl;
        #endif
        auto res = qb::Instruction::make(bytes, code_len, addr);
        if (res.code > 0) {
            #ifdef QB_PROGRAM_PARSE_DEBUG
                std::cout << "parser error@" << addr << ", code: " << +res.code << std::endl;
            #endif
            for (qb::Instruction* it: instructions) {
                delete it;
            }
            return {
                .code = res.code,
                .error_addr = addr
            };
        }

        #ifdef QB_PROGRAM_PARSE_DEBUG
            std::cout << res.instr->to_str() << std::endl;
        #endif
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