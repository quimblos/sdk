#include <iostream>
#include "static_checker.h"

using namespace qb;

/* Check */

static_checker::res_t static_checker::resolve_pointer(const Script* script, code_addr_t addr, device_t device_i, port_t* port, Node** target) {

    bool is_device_node = device_i < 0xFE;
    bool is_engine_node = device_i == 0xFE;
    bool is_script_node = device_i == 0xFF;

    if (is_device_node) {
        if (device_i >= script->devices.size()) {
            return {
                .code = QB_STATIC_CHECKER_R_INVALID_DEVICE,
                .error_addr = addr
            };
        }
        Device* device = script->devices.at(device_i);
        if (port != nullptr) {
            *target = device->get_node(*port);
        }
    }
    else if (is_script_node) {
        if (port != nullptr) {
            if (*port >= script->nodes.size()) {
                return {
                    .code = QB_STATIC_CHECKER_R_INVALID_NODE,
                    .error_addr = addr
                };
            }
            *target = script->nodes.at(*port);
        }
    }
    else if (is_engine_node) {
        // TODO
    }

    if (port != nullptr && *target == nullptr) {
        return {
            .code = QB_STATIC_CHECKER_R_INVALID_NODE,
            .error_addr = addr
        };
    }

    return {
        .code = QB_STATIC_CHECKER_R_OK,
        .error_addr = 0
    };
}

static_checker::res_t static_checker::check(Engine& engine, const Script* script) {
    
    // Type Casting

    for (code_addr_t addr = 0; addr < script->instructions.size(); addr++) {
        
        auto instruction = script->instructions.at(addr);

        OpBind bind;
        device_t* device = nullptr;
        node::Pointer* target = nullptr;
        Node* source = nullptr;

        switch (instruction->code) {
            case qb::OpCode::SET:
                bind = ((instruction::Set*) instruction)->bind;
                target = ((instruction::Set*) instruction)->target;
                source = ((instruction::Set*) instruction)->source;
                break;
            case qb::OpCode::HOLD:
                device = &((instruction::Hold*) instruction)->device;
                break;
            case qb::OpCode::RELEASE:
                device = &((instruction::Release*) instruction)->device;
                break;
            case qb::OpCode::GOTO: break;
            case qb::OpCode::IF_EQ:
                bind = ((instruction::IfEq*) instruction)->bind;
                target = ((instruction::IfEq*) instruction)->target;
                source = ((instruction::IfEq*) instruction)->source;
                break;
            case qb::OpCode::IF_LT:
                bind = ((instruction::IfLt*) instruction)->bind;
                target = ((instruction::IfLt*) instruction)->target;
                source = ((instruction::IfLt*) instruction)->source;
                break;
            case qb::OpCode::IF_GT:
                bind = ((instruction::IfGt*) instruction)->bind;
                target = ((instruction::IfGt*) instruction)->target;
                source = ((instruction::IfGt*) instruction)->source;
                break;
            case qb::OpCode::ADD:
                bind = ((instruction::Add*) instruction)->bind;
                target = ((instruction::Add*) instruction)->target;
                source = ((instruction::Add*) instruction)->source;
                break;
            case qb::OpCode::SUB:
                bind = ((instruction::Sub*) instruction)->bind;
                target = ((instruction::Sub*) instruction)->target;
                source = ((instruction::Sub*) instruction)->source;
                break;
            case qb::OpCode::MULT:
                bind = ((instruction::Mult*) instruction)->bind;
                target = ((instruction::Mult*) instruction)->target;
                source = ((instruction::Mult*) instruction)->source;
                break;
            case qb::OpCode::DIV:
                bind = ((instruction::Div*) instruction)->bind;
                target = ((instruction::Div*) instruction)->target;
                source = ((instruction::Div*) instruction)->source;
                break;
            case qb::OpCode::MOD:
                bind = ((instruction::Mod*) instruction)->bind;
                target = ((instruction::Mod*) instruction)->target;
                source = ((instruction::Mod*) instruction)->source;
                break;
            case qb::OpCode::POW:
                bind = ((instruction::Pow*) instruction)->bind;
                target = ((instruction::Pow*) instruction)->target;
                source = ((instruction::Pow*) instruction)->source;
                break;
            case qb::OpCode::LOG:
                device = &((instruction::Log*) instruction)->device;
                break;
            case qb::OpCode::SLEEP: break;
            case qb::OpCode::RETURN: break;
            case qb::OpCode::RESET: break;
            case qb::OpCode::REBOOT: break;
        }

        // opcode has no device and no target, nothing to be checked at this stage
        if (device == nullptr) {
            if (target == nullptr) {
                continue;
            }
            else {
                device = &target->device;
            }
        }
        port_t* port;
        if (target != nullptr) {
            port = &target->port;
        }

        if (instruction->code == qb::OpCode::HOLD || instruction->code == qb::OpCode::RELEASE) {
            if (*device >= 0xFE) {
                return {
                    .code = QB_STATIC_CHECKER_R_INVALID_DEVICE,
                    .error_addr = addr
                };
            }
        }

        Node* node;
        auto node_res = static_checker::resolve_pointer(script, addr, *device, port, &node);
        if (node_res.code != QB_STATIC_CHECKER_R_OK) {
            return node_res;
        }

        if (target != nullptr) {
            if (bind == OpBind::REF_NODE || bind == OpBind::REF_REF) {
                if (node->type != Type::PTR) {
                    return {
                        .code = QB_STATIC_CHECKER_R_INVALID_TARGET,
                        .error_addr = addr
                    };
                }

                auto pointer = (node::Pointer*) node;
                auto target_ref_res = static_checker::resolve_pointer(script, addr, pointer->device, &pointer->port, &node);
                if (target_ref_res.code != QB_STATIC_CHECKER_R_OK) {
                    return target_ref_res;
                }
            }

            if (source != nullptr) {
                bool type_match = false;
                switch (node->type) {
                    case Type::VOID: break;
                    case Type::ERROR:
                        type_match = (
                            source->type == Type::ERROR
                            || source->type == Type::STRING
                        ); break;
                    case Type::PTR:
                        type_match = (
                            source->type == Type::PTR
                        ); break;
                    case Type::BOOL:
                    case Type::UINT8:
                    case Type::INT8:
                    case Type::UINT16:
                    case Type::INT16:
                    case Type::UINT32:
                    case Type::INT32:
                    case Type::FLOAT32:
                        type_match = (
                            source->type == Type::BOOL
                            || source->type == Type::UINT8
                            || source->type == Type::INT8
                            || source->type == Type::UINT16
                            || source->type == Type::INT16
                            || source->type == Type::UINT32
                            || source->type == Type::INT32
                            || source->type == Type::FLOAT32
                        ); break;
                    case Type::STRING:
                        type_match = (
                            source->type == Type::ERROR
                            || source->type == Type::STRING
                        ); break;
                    case Type::ARRAY: break;
                }

                if (!type_match) {
                    return {
                        .code = QB_STATIC_CHECKER_R_TYPE_MISMATCH,
                        .error_addr = addr
                    };
                }
            }
        }        
        
    }

    return {
        .code = QB_STATIC_CHECKER_R_OK,
        .error_addr = 0
    };

}