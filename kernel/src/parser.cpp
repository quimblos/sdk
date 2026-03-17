#include <iostream>
#include "parser.h"
#include "hex.h"

#define QB_PARSER_DEBUG

using namespace qb;

const std::string E_UNEXPECTED_EOF = "Unexpected end of file";

#define ASSERT_N_BYTES(N) \
    if (addr > code_len-N) \
        return { \
            .code = QB_PARSER_R_FAILED_UNEXPECTED_EOF, \
            .error_addr = addr \
        };

#define ASSERT_N_BYTES_TARGET(N) \
    if (addr > code_len-N) \
        return { \
            .code = QB_PARSER_R_FAILED_UNEXPECTED_EOF, \
            .bind = OpBind::NODE_NODE, \
            .device = 0, \
            .port = 0, \
            .index = 0, \
            .next_addr = 0 \
        };

#define ASSERT_DEVICE(D) \
    if (D < 0xFE && D >= devices.size()) return { \
        .code = QB_PARSER_R_FAILED_DEVICE_INDEX, \
        .error_addr = addr \
    }; \

#define RESOLVE_BIND(B) \
    ((uint8_t) B) >= 0x10 ? (OpBind) ((uint8_t)B - 0x10) : B

#define ASSERT_TARGET() \
    if (target.code > 0) return { \
        .code = QB_PARSER_R_FAILED_TARGET, \
        .error_addr = addr \
    }; \
    addr = target.next_addr;

#define ASSERT_SOURCE() \
    if (source.code > 0) return { \
        .code = QB_PARSER_R_FAILED_DATA, \
        .error_addr = addr \
    }; \
    addr = source.next_addr;

#define ASSERT_ALIAS() \
    if (alias.code > 0) return { \
        .code = QB_PARSER_R_FAILED_DATA, \
        .error_addr = addr \
    }; \
    addr = alias.next_addr;


/* Debug Logs */

void log_op_code(code_addr_t addr, OpCode code) {
    std::cout << "[parser] @" << addr << " ";
    switch (code) {
        case OpCode::USE_DEVICE: std::cout << "USE_DEVICE "; break;
        case OpCode::USE_NODE: std::cout << "USE_NODE "; break;
        case OpCode::SET: std::cout << "SET "; break;
        case OpCode::HOLD: std::cout << "HOLD "; break;
        case OpCode::RELEASE: std::cout << "RELEASE "; break;
        case OpCode::GOTO: std::cout << "GOTO "; break;
        case OpCode::IF_EQ: std::cout << "IF_EQ "; break;
        case OpCode::IF_LT: std::cout << "IF_LT "; break;
        case OpCode::IF_GT: std::cout << "IF_GT "; break;
        case OpCode::ADD: std::cout << "ADD "; break;
        case OpCode::SUB: std::cout << "SUB "; break;
        case OpCode::MULT: std::cout << "MULT "; break;
        case OpCode::DIV: std::cout << "DIV "; break;
        case OpCode::MOD: std::cout << "MOD "; break;
        case OpCode::POW: std::cout << "POW "; break;
        case OpCode::LOG: std::cout << "LOG "; break;
        case OpCode::SLEEP: std::cout << "SLEEP "; break;
        case OpCode::RETURN: std::cout << "RETURN "; break;
        case OpCode::RESET: std::cout << "RESET "; break;
        case OpCode::REBOOT: std::cout << "REBOOT "; break;
    }
}

/* Parse Target */

struct ParsedTarget {
    uint8_t code;
    OpBind bind;
    device_t device;
    port_t port;
    index_t index;
    code_addr_t next_addr;
};

ParsedTarget parse_target(code_t* bytes, code_addr_t code_len, code_addr_t addr) {
    ASSERT_N_BYTES_TARGET(1)
    OpBind bind = (OpBind) bytes[addr];
    addr += 1;
    ASSERT_N_BYTES_TARGET(1)
    device_t device = (device_t) bytes[addr];
    addr += 1;
    ASSERT_N_BYTES_TARGET(1)
    port_t port = (port_t) bytes[addr];
    addr += 1;
    
    index_t index = 0;
    if (bind < 0x10) {
        ASSERT_N_BYTES_TARGET(2)
        index = (index_t) (bytes[addr] << 8) + bytes[addr+1];
        addr += 2;
    }

    return {
        .code = QB_PARSER_R_OK,
        .bind = bind,
        .device = device,
        .port = port,
        .index = index,
        .next_addr = addr
    };
}

/* Parse */

parser::res_t parser::parse(Engine& engine, std::string name, std::string hex) {
    Bytecode bytecode = hex_to_bytecode(hex);
    
    std::vector<Device*> devices;
    std::vector<Node*> nodes;
    std::vector<Instruction*> instructions;
    std::vector<std::string> node_aliases;
    
    std::unordered_map<code_addr_t, code_addr_t> addr_map;

    code_t* bytes = bytecode.bytes;

    // Header
    if (
        bytes[0] != 'q'
        || bytes[1] != 'b'
        || bytes[2] != 0x00
        || bytes[3] != 0x00
    ) {
        return {
            .code = QB_PARSER_R_FAILED_INVALID_HEADER,
            .error_addr = 0
        };
    }

    code_addr_t code_len = bytecode.length;
    for (code_addr_t addr = 4; addr < code_len;) {
        
        // [0]: OpCode
        OpCode code = (OpCode) bytes[addr];
        addr += 1;
        #ifdef QB_PARSER_DEBUG
            log_op_code(addr-1, code);
        #endif

        if (code != OpCode::USE_DEVICE && code != OpCode::USE_NODE) {
            addr_map.emplace(addr-1, instructions.size());
        }

        switch (code) {
            case OpCode::USE_DEVICE:
                {
                    auto source = Node::parse(qb::Type::STRING_SHORT, bytes, code_len, addr);
                    ASSERT_SOURCE()

                    std::string name = ((node::String*) source.value)->data;
                    Device* device = engine.get_device(name);
                    devices.push_back(device);
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                    delete source.value;
                }
                break;
            case OpCode::USE_NODE:
                {
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()

                    nodes.push_back(source.value);
                    node_aliases.push_back(std::to_string(nodes.size()-1));

                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::USE_NODE_ALIASED:
                {
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    auto alias = Node::parse(qb::Type::STRING_SHORT, bytes, code_len, addr);
                    ASSERT_ALIAS()

                    nodes.push_back(source.value);
                    node_aliases.push_back(((node::String*)alias.value)->data);

                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::SET:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()

                    instructions.push_back(new instruction::Set(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value
                    ));

                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::HOLD:
                {
                    ASSERT_N_BYTES(1);
                    device_t device = (device_t) bytes[addr];
                    addr += 1;
                    ASSERT_DEVICE(device)

                    instructions.push_back(new instruction::Hold(
                        device
                    ));
                }
                break;
            case OpCode::RELEASE:
                {
                    ASSERT_N_BYTES(1);
                    device_t device = (device_t) bytes[addr];
                    addr += 1;
                    ASSERT_DEVICE(device)

                    instructions.push_back(new instruction::Release(
                        device
                    ));
                }
                break;
            case OpCode::GOTO:
                {
                    ASSERT_N_BYTES(2);
                    code_addr_t goto_true = (bytes[addr] << 8) + bytes[addr+1];
                    addr += 2;

                    instructions.push_back(new instruction::GoTo(
                        goto_true
                    ));
                }
                break;
            case OpCode::IF_EQ:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    ASSERT_N_BYTES(2);
                    code_addr_t goto_true = (bytes[addr] << 8) + bytes[addr+1];
                    addr += 2;
                    ASSERT_N_BYTES(2);
                    code_addr_t goto_false = (bytes[addr] << 8) + bytes[addr+1];
                    addr += 2;

                    instructions.push_back(new instruction::IfEq(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value,
                        goto_true,
                        goto_false
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::IF_LT:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    ASSERT_N_BYTES(2);
                    code_addr_t goto_true = (bytes[addr] << 8) + bytes[addr+1];
                    addr += 2;
                    ASSERT_N_BYTES(2);
                    code_addr_t goto_false = (bytes[addr] << 8) + bytes[addr+1];
                    addr += 2;

                    instructions.push_back(new instruction::IfLt(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value,
                        goto_true,
                        goto_false
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::IF_GT:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    ASSERT_N_BYTES(2);
                    code_addr_t goto_true = (bytes[addr] << 8) + bytes[addr+1];
                    addr += 2;
                    ASSERT_N_BYTES(2);
                    code_addr_t goto_false = (bytes[addr] << 8) + bytes[addr+1];
                    addr += 2;

                    instructions.push_back(new instruction::IfGt(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value,
                        goto_true,
                        goto_false
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::ADD:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    
                    instructions.push_back(new instruction::Add(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::SUB:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    
                    instructions.push_back(new instruction::Sub(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::MULT:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    
                    instructions.push_back(new instruction::Mult(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::DIV:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    
                    instructions.push_back(new instruction::Div(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::MOD:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    
                    instructions.push_back(new instruction::Mod(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::POW:
                {
                    auto target = parse_target(bytes, code_len, addr);
                    ASSERT_TARGET()
                    ASSERT_DEVICE(target.device)
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()
                    
                    instructions.push_back(new instruction::Pow(
                        RESOLVE_BIND(target.bind),
                        target.device,
                        target.port,
                        target.index,
                        source.value
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::LOG:
                {
                    ASSERT_N_BYTES(1);
                    device_t device = bytes[addr];
                    addr += 1;
                    ASSERT_DEVICE(device)

                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()

                    instructions.push_back(new instruction::Log(
                        device,
                        source.value
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::SLEEP:
                {
                    ASSERT_N_BYTES(4);
                    uint32_t ms = (bytes[addr] << 24) + (bytes[addr+1] << 16) + (bytes[addr+2] << 8) + bytes[addr+3];
                    addr += 4;
                    
                    instructions.push_back(new instruction::Sleep(
                        ms
                    ));

                    #ifdef QB_PARSER_DEBUG
                        std::cout << ms;
                    #endif
                }
                break;
            case OpCode::RETURN:
                {
                    auto source = Node::from_bytes(bytes, code_len, addr);
                    ASSERT_SOURCE()

                    instructions.push_back(new instruction::Return(
                        source.value
                    ));
                    
                    #ifdef QB_PARSER_DEBUG
                        std::cout << source.value->to_str();
                    #endif
                }
                break;
            case OpCode::RESET:
                {
                    instructions.push_back(new instruction::Reset());
                }
                break;
            case OpCode::REBOOT:
                {
                    instructions.push_back(new instruction::Reboot());
                }
                break;
            default:
                return {
                    .code = QB_PARSER_R_FAILED_INVALID_OPCODE,
                    .error_addr = addr
                };
        }

        #ifdef QB_PARSER_DEBUG
            std::cout << std::endl;
        #endif
    }

    delete[] bytecode.bytes;

    // Chain instructions (next/next_false)

    auto n = instructions.size();
    for (code_addr_t addr = 0; addr < n; addr++) {
        
        auto instruction = instructions.at(addr);

        bool advance = true;
        code_addr_t* next_true = &instruction->next;
        code_addr_t* next_false = nullptr;

        switch (instruction->code) {
            case qb::OpCode::SET:
            case qb::OpCode::HOLD:
            case qb::OpCode::RELEASE:
                break;
            case qb::OpCode::GOTO:
                advance = false;
                break;
            case qb::OpCode::IF_EQ:
                advance = false;
                next_false = &((instruction::IfEq*) instruction)->next_false;
                break;
            case qb::OpCode::IF_LT:
                advance = false;
                next_false = &((instruction::IfLt*) instruction)->next_false;
                break;
            case qb::OpCode::IF_GT:
                advance = false;
                next_false = &((instruction::IfGt*) instruction)->next_false;
                break;
            case qb::OpCode::ADD:
            case qb::OpCode::SUB:
            case qb::OpCode::MULT:
            case qb::OpCode::DIV:
            case qb::OpCode::MOD:
            case qb::OpCode::POW:
            case qb::OpCode::LOG:
            case qb::OpCode::SLEEP:
            case qb::OpCode::RETURN:
            case qb::OpCode::RESET:
            case qb::OpCode::REBOOT:
                break;
        }


        if (*next_true == 0xFFFF) {
            if (advance && addr < n-1) {
                *next_true = addr+1;
            }
        }
        else {
            if (*next_true >= n) {
                return {
                    .code = QB_PARSER_R_FAILED_UNBOUND_GOTO,
                    .error_addr = addr
                };
            }
        }

        if (next_false != nullptr) {
            if (*next_false == 0xFFFF) {
                if (advance && addr < n-1) {
                    *next_false = addr+1;
                }
            }
            else {
                if (*next_false >= n) {
                    return {
                        .code = QB_PARSER_R_FAILED_UNBOUND_GOTO,
                        .error_addr = addr
                    };
                }
            }
        }
   
    }

    Script* script = new Script(name, devices, nodes, instructions, node_aliases);

    return {
        .code = QB_PARSER_R_OK,
        .error_addr = 0,
        .script = script
    };
}

/* Dump */

parser::res_t parser::dump(Script& script) {
    // TODO
    return {
        .code = QB_PARSER_R_OK,
        .error_addr = 0
    };
}