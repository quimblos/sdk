#pragma once
#include <sstream>
#include "data.h"
#include "code.h"
#include "device.h"

/**
 * Script
 */

namespace qb {

    struct Script {
        std::string name;
        std::vector<qb::Device*> devices;
        std::vector<qb::DataType> variables;
        std::vector<qb::Instruction*> instructions;

        ~Script() {
            for (qb::Instruction* cmd : this->instructions) {
                cmd->purge();
                delete cmd;
            }
        }
    };
}