#pragma once
#include <sstream>
#include "memory.h"
#include "code.h"
#include "device.h"

#define COLOR_GRAY "\033[30m"
#define COLOR_NC "\033[0m"


/**
 * Script
 */

namespace qb {

    struct Script {
        std::string name;
        std::vector<qb::Device*> devices;
        std::vector<qb::Node*> nodes;
        std::vector<qb::Instruction*> instructions;

        std::vector<std::string> node_aliases;

        ~Script() {
            for (qb::Node* node : this->nodes) {
                delete node;
            }
            for (qb::Instruction* cmd : this->instructions) {
                cmd->purge();
                delete cmd;
            }
        }

        std::string describe() const {
            std::stringstream ss;
            ss << COLOR_GRAY;

            for (size_t i = 0; i < this->devices.size(); i++) {
                auto device = this->devices.at(i);
                ss << "#device:" << i << " " << device->get_name() << std::endl;
                auto nodes = device->get_nodes();
                for (size_t j = 0; j < nodes.size(); j++) {
                    auto node = nodes.at(j);
                    ss << "  #node:" << j << " " << node.first << " " << node.second->to_str() << std::endl;
                }
            }
            if (this->nodes.size()) {
                ss << "#device:$ [script]" << std::endl;
            }
            for (size_t i = 0; i < this->nodes.size(); i++) {
                auto node = this->nodes.at(i);
                ss << "  #node:" << i << " " << (this->node_aliases.at(i)) << " " << node->to_str() << std::endl;
            }
            for (size_t i = 0; i < this->instructions.size(); i++) {
                auto instruction = this->instructions.at(i);
                ss << (i < 10 ? " " : "") << i << " | " << instruction->to_str();
                if (
                    instruction->code != OpCode::GOTO
                    && instruction->code != OpCode::IF_EQ
                    && instruction->code != OpCode::IF_GT
                    && instruction->code != OpCode::IF_GTEQ
                ) {
                    ss << "\t@" << instruction->next;
                }
                ss << std::endl;
            }

            ss << COLOR_NC;
            return ss.str();
        }
    };
}