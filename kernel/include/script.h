#pragma once
#include <sstream>
#include "data.h"
#include "cmd.h"
#include "device.h"

/**
 * Script
 */

namespace qb {
    struct Script {
        std::string name;
        std::vector<qb::Device*> devices;
        std::vector<qb::DataType> variable_types;
        std::vector<qb::Cmd> cmds;

        ~Script() {
            for (qb::Cmd cmd : this->cmds) {
                if (cmd.value != nullptr) {
                    cmd.value->purge();
                    delete cmd.value;
                }
            }
        }
    };
}