#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include "parser.h"

#define QB_PARSER_DEBUG

using namespace qb;

const std::string E_UNEXPECTED_EOF = "Unexpected end of file";

#define ASSERT_N_BYTES(N) if (i > n-N) return { .ok = false, .message = E_UNEXPECTED_EOF }
#define ASSERT_N_BYTES_DATA(N) if (i > n-N) return { .ok = false, .message = E_UNEXPECTED_EOF }

parser::res_t parser::dump(Script& script) {
    // TODO
    return {
        .ok = true
    };
}

struct ParsedData {
    bool ok;
    std::string message;
    qb::Data data;
    code_addr_t i;
};

ParsedData parse_data(qb::DataType type, uint8_t* bytes, code_addr_t n, code_addr_t i) {
    if (type == qb::DataType::UNKNOWN) {
        return {
            .ok = false,
            .message = "Unknown Type",
            .data = qb::Data::_void(),
            .i = i
        };
    }

    qb::Data data;
    switch (type) {
        case qb::DataType::UNKNOWN: break;
        case qb::DataType::VOID:
            data = qb::Data::_void();
            break;
        case qb::DataType::BOOL:
        case qb::DataType::UINT8:
        case qb::DataType::INT8:
            ASSERT_N_BYTES_DATA(1);
            data = qb::Data::parse(type, bytes+i);
            i += 1;
            break;
        case qb::DataType::UINT8_ARR:
        case qb::DataType::INT8_ARR:
            {
                ASSERT_N_BYTES_DATA(2);
                uint16_t length = (bytes[i] << 8) + bytes[i+1];
                i += 2;
                ASSERT_N_BYTES_DATA(length);
                data = qb::Data::parse_arr(type, length, bytes+i);
                i += length;
            }
            break;
        case qb::DataType::UINT16:
        case qb::DataType::INT16:
            ASSERT_N_BYTES_DATA(2);
            data = qb::Data::parse(type, bytes+i);
            i += 2;
            break;
        case qb::DataType::UINT16_ARR:
        case qb::DataType::INT16_ARR:
            {
                ASSERT_N_BYTES_DATA(2);
                uint16_t length = (bytes[i] << 8) + bytes[i+1];
                i += 2;
                ASSERT_N_BYTES_DATA(length*2);
                data = qb::Data::parse_arr(type, length, bytes+i);
                i += length*2;
            }
            break;             
        case qb::DataType::UINT32:
        case qb::DataType::INT32:
            ASSERT_N_BYTES_DATA(4);
            data = qb::Data::parse(type, bytes+i);
            i += 4;
            break;
        case qb::DataType::UINT32_ARR:
        case qb::DataType::INT32_ARR:
            {
                ASSERT_N_BYTES_DATA(2);
                uint16_t length = (bytes[i] << 8) + bytes[i+1];
                i += 2;
                ASSERT_N_BYTES_DATA(length*4);
                data = qb::Data::parse_arr(type, length, bytes+i);
                i += length*4;
            }
            break;
        case qb::DataType::FLOAT32:
            ASSERT_N_BYTES_DATA(4);
            data = qb::Data::parse(type, bytes+i);
            i += 4;
            break;
        case qb::DataType::FLOAT32_ARR:
            {
                ASSERT_N_BYTES_DATA(2);
                uint16_t length = (bytes[i] << 8) + bytes[i+1];
                i += 2;
                ASSERT_N_BYTES_DATA(length*4);
                data = qb::Data::parse_arr(type, length, bytes+i);
                i += length*4;
            }
            break;
        case qb::DataType::STRING_SHORT:
            {
                ASSERT_N_BYTES_DATA(1);
                uint8_t length = *((uint8_t*) bytes+i);
                i += 1;
                ASSERT_N_BYTES_DATA(length);
                data = qb::Data::parse(type, bytes+i-1);
                i += length;
            }
            break;
        case qb::DataType::STRING:
            {
                ASSERT_N_BYTES_DATA(2);
                uint16_t length = (bytes[i] << 8) + bytes[i+1];
                i += 2;
                ASSERT_N_BYTES_DATA(length);
                data = qb::Data::parse(type, bytes+i-2);
                i += length;
            }
            break;
        default:
            break;
    }

    return {
        .ok = true,
        .message = "",
        .data = data,
        .i = i
    };
}

struct ParsedPort {
    bool ok;
    std::string message;
    qb::Device* device;
};

ParsedPort parse_port(std::vector<qb::Device*>& devices, qb::Script* script, uint8_t device_i, uint8_t port) {
    if (device_i == 0xFF) {
        if (port >= script->variables.size()) {
            return {
                .ok = false,
                .message = "Register out of range"
            };
        }
        return {
            .ok = true,
            .message = "",
            .device = nullptr
        };
    }
    else {
        if (device_i >= devices.size()) {
            return {
                .ok = false,
                .message = "Device out of range"
            };
        }
        qb::Device* device = devices.at(device_i);
        if (!device->has_i(port)) {
            return {
                .ok = false,
                .message = "Register out of range"
            };
        }
        return {
            .ok = true,
            .message = "",
            .device = device
        };
    }
}


parser::res_t parser::parse(qb::Engine& engine, std::string name, std::string hex) {
    auto bytecode = parser::hexToBytecode(hex);

    qb::Script* script = new qb::Script();
    script->name = name;

    uint8_t new_device_i = 0;
    uint8_t current_code = 0;
    qb::code_addr_t addr = 0;
    
    std::vector<qb::Device*> devices;
    std::unordered_map<qb::code_addr_t, qb::code_addr_t> gotos_true;
    std::unordered_map<qb::code_addr_t, qb::code_addr_t> gotos_false;
    
    code_t* bytes = bytecode.bytes;

    // Header
    if (
        bytes[0] != 'q'
        || bytes[1] != 'b'
        || bytes[2] != 0x00
        || bytes[3] != 0x00
    ) {
        return {
            .ok = false,
            .message = "Invalid quimblos header"
        };
    }

    size_t n = bytecode.length;
    for (size_t i = 4; i < n;) {
        
        // [0]: OpCode

        if (!current_code) {
            current_code = bytes[i];
            i += 1;
            // code_addresses.push_back(i);
            #ifdef QB_PARSER_TRACE
                std::cout << "[parser] @" << i-1 << " ";
                switch (current_code) {
                    case qb::OpCode::USE_DEVICE: std::cout << "USE_DEVICE"; break;
                    case qb::OpCode::USE_PORT: std::cout << "USE_PORT"; break;
                    case qb::OpCode::SET: std::cout << "SET"; break;
                    case qb::OpCode::SET_AT: std::cout << "SET_AT"; break;
                    case qb::OpCode::HOLD: std::cout << "HOLD"; break;
                    case qb::OpCode::RELEASE: std::cout << "RELEASE"; break;
                    case qb::OpCode::GOTO: std::cout << "GOTO"; break;
                    case qb::OpCode::IF_EQ: std::cout << "IF_EQ"; break;
                    case qb::OpCode::IF_GT: std::cout << "IF_GT"; break;
                    case qb::OpCode::IF_GTEQ: std::cout << "IF_GTEQ"; break;
                    case qb::OpCode::ADD: std::cout << "ADD"; break;
                    case qb::OpCode::SUB: std::cout << "SUB"; break;
                    case qb::OpCode::MULT: std::cout << "MULT"; break;
                    case qb::OpCode::DIV: std::cout << "DIV"; break;
                    case qb::OpCode::MOD: std::cout << "MOD"; break;
                    case qb::OpCode::LOG: std::cout << "LOG"; break;
                    case qb::OpCode::SLEEP: std::cout << "SLEEP"; break;
                    case qb::OpCode::STOP: std::cout << "STOP"; break;
                    case qb::OpCode::ERROR: std::cout << "ERROR"; break;
                    case qb::OpCode::RESET: std::cout << "RESET"; break;
                    case qb::OpCode::REBOOT: std::cout << "REBOOT"; break;
                    default: std::cout << "?";break;
                }
                std::cout << std::endl;
            #endif
            continue;
        }

        // [1..X] Arguments
        uint8_t device_i = 0x00;
        uint8_t port = 0x00;
        uint8_t log_code = 0x00;
        
        qb::DataType idx_type = qb::DataType::VOID;
        qb::DataType type = qb::DataType::UNKNOWN;
        qb::Data* index;

        switch (current_code) {
            case qb::OpCode::USE_DEVICE:
                {
                    type = qb::DataType::STRING_SHORT;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "USE DEVICE ";
                    #endif
                }
                break;
            case qb::OpCode::USE_PORT:
                {
                    ASSERT_N_BYTES(1);
                    type = (qb::DataType) bytes[i];
                    i += 1;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "USE PORT ";
                    #endif
                }
                break;
            case qb::OpCode::SET:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    port = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "SET (dev:" << ((uint16_t) device_i) << ", port:" << ((uint16_t) port) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::SET_AT:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    port = bytes[i+1];
                    idx_type = (qb::DataType) bytes[i+2];
                    i += 3;

                    auto parsed_index = parse_data(idx_type, bytes, n, i);
                    if (parsed_index.ok) {
                        index = &parsed_index.data;
                        i = parsed_index.i;
                    }
                    else {
                        return {
                            .ok = false,
                            .message = "Invalid pointer index"
                        };
                    }
                    
                    ASSERT_N_BYTES(1);
                    type = (qb::DataType) bytes[i];
                    i += 1;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "SET_AT (dev:" << ((uint16_t) device_i) << ", port:" << ((uint16_t) port) << ", index: ";
                        index->log();
                        std::cout << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::HOLD:
                {
                    ASSERT_N_BYTES(1);
                    device_i = bytes[i];
                    type = qb::DataType::VOID;
                    i += 1;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "HOLD (dev:" << ((uint16_t) device_i) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::RELEASE:
                {
                    ASSERT_N_BYTES(1);
                    device_i = bytes[i];
                    type = qb::DataType::VOID;
                    i += 1;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "RELEASE (dev:" << ((uint16_t) device_i) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::GOTO:
                {
                    ASSERT_N_BYTES(2);
                    qb::code_addr_t goto_true = (bytes[i] << 8) + bytes[i+1];
                    gotos_true.emplace(addr, goto_true);
                    type = qb::DataType::VOID;
                    i += 2;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "GOTO (addr:" << ((uint16_t) goto_true) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::IF_EQ:
                {
                    ASSERT_N_BYTES(7);
                    device_i = bytes[i];
                    port = bytes[i+1];
                    qb::code_addr_t goto_true = (bytes[i+2] << 8) + bytes[i+3];
                    gotos_true.emplace(addr, goto_true);
                    qb::code_addr_t goto_false = (bytes[i+4] << 8) + bytes[i+5];
                    gotos_false.emplace(addr, goto_false);
                    type = (qb::DataType) bytes[i+6];
                    i += 7;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "IF_EQ (dev:" << ((uint16_t) device_i) << ", port:" << ((uint16_t) port) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::IF_GT:
                {
                    ASSERT_N_BYTES(7);
                    device_i = bytes[i];
                    port = bytes[i+1];
                    qb::code_addr_t goto_true = (bytes[i+2] << 8) + bytes[i+3];
                    gotos_true.emplace(addr, goto_true);
                    qb::code_addr_t goto_false = (bytes[i+4] << 8) + bytes[i+5];
                    gotos_false.emplace(addr, goto_false);
                    type = (qb::DataType) bytes[i+6];
                    i += 7;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "IF_GT (dev:" << ((uint16_t) device_i) << ", port:" << ((uint16_t) port) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::IF_GTEQ:
                {
                    ASSERT_N_BYTES(7);
                    device_i = bytes[i];
                    port = bytes[i+1];
                    qb::code_addr_t goto_true = (bytes[i+2] << 8) + bytes[i+3];
                    gotos_true.emplace(addr, goto_true);
                    qb::code_addr_t goto_false = (bytes[i+4] << 8) + bytes[i+5];
                    gotos_false.emplace(addr, goto_false);
                    type = (qb::DataType) bytes[i+6];
                    i += 7;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "IF_GTEQ (dev:" << ((uint16_t) device_i) << ", port:" << ((uint16_t) port) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::ADD:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    port = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "SUM (dev:" << ((uint16_t) device_i) << ", port:" << ((uint16_t) port) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::SUB:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    port = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "SUB (dev:" << ((uint16_t) device_i) << ", port:" << ((uint16_t) port) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::MULT:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    port = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "MULT (dev:" << ((uint16_t) device_i) << ", port:" << ((uint16_t) port) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::DIV:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    port = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "DIV (dev:" << ((uint16_t) device_i) << ", port:" << ((uint16_t) port) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::MOD:
                {
                    ASSERT_N_BYTES(2);
                    device_i = bytes[i];
                    log_code = bytes[i+1];
                    type = qb::DataType::STRING;
                    i += 2;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "MOD (dev:" << ((uint16_t) device_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::LOG:
                {
                    ASSERT_N_BYTES(1);
                    device_i = bytes[i];
                    type = qb::DataType::STRING;
                    i += 1;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "LOG (dev:" << ((uint16_t) device_i) << ") ";
                    #endif
                }
                break;
            case qb::OpCode::SLEEP:
                {
                    type = qb::DataType::UINT32;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "SLEEP ";
                    #endif
                }
                break;
            case qb::OpCode::STOP:
                {
                    ASSERT_N_BYTES(1);
                    log_code = bytes[i];
                    type = qb::DataType::STRING;
                    i += 1;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "STOP ";
                    #endif
                }
                break;
            case qb::OpCode::ERROR:
                {
                    ASSERT_N_BYTES(1);
                    log_code = bytes[i];
                    type = qb::DataType::STRING;
                    i += 1;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "ERROR (code:" << log_code << ") ";
                    #endif
                }
                break;
            case qb::OpCode::RESET:
                {
                    type = qb::DataType::VOID;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "RESET ";
                    #endif
                }
                break;
            case qb::OpCode::REBOOT:
                {
                    type = qb::DataType::VOID;
                    #ifdef QB_PARSER_DEBUG
                        std::cout << "[parser] " << "REBOOT ";
                    #endif
                }
                break;
            default:
                return {
                    .ok = false,
                    .message = "Unknown OpCode"
                };
        }

        // (edge case: create variables)
        if (current_code == qb::OpCode::USE_PORT) {
            script->variables.emplace_back(type);
            current_code = 0;
            continue;
        }

        // [X..Y] Data
        qb::Data* data = nullptr;

        if (type == qb::DataType::UNKNOWN) {
            std::cout << "!" << std::endl;
            return {
                .ok = false,
                .message = "Unknown DataType"
            };
        }
        
        auto parsed_data = parse_data(type, bytes, n, i);
        if (parsed_data.ok) {
            data = &parsed_data.data;
            i = parsed_data.i;
        }
        else {
            return {
                .ok = false,
                .message = "Failed to parse"
            };
        }
        #ifdef QB_PARSER_DEBUG
            if (data != nullptr) {
                data->log();
            }
            std::cout << std::endl;
        #endif
        
        // (Resolve Device+Port)
        qb::Device* device = nullptr;

        switch (current_code) {
            case qb::OpCode::USE_DEVICE:
                {
                    device = engine.getDevice(data->as_string());
                    if (device == nullptr) {
                        return {
                            .ok = false,
                            .message = "Device out of range"
                        };
                    }
                    devices.push_back(device);
                    data->purge();
                }
                break;
            case qb::OpCode::USE_PORT:
                break;
            case qb::OpCode::SET:
            case qb::OpCode::SET_AT:
            case qb::OpCode::HOLD:
            case qb::OpCode::RELEASE:
            case qb::OpCode::IF_EQ:
            case qb::OpCode::IF_GT:
            case qb::OpCode::IF_GTEQ:
            case qb::OpCode::ADD:
            case qb::OpCode::SUB:
            case qb::OpCode::MULT:
            case qb::OpCode::DIV:
            case qb::OpCode::MOD:
                {
                    auto parsed_port = parse_port(devices, script, device_i, port);
                    if (parsed_port.ok) {
                        device = parsed_port.device;
                    }
                    else {
                        return {
                            .ok = false,
                            .message = parsed_port.message
                        };
                    }
                }
                break;
            case qb::OpCode::GOTO:
            case qb::OpCode::LOG:
            case qb::OpCode::SLEEP:
            case qb::OpCode::STOP:
            case qb::OpCode::ERROR:
            case qb::OpCode::RESET:
            case qb::OpCode::REBOOT:
                break;
        }
        
        // (Create Instruction)
        qb::Instruction* instruction = nullptr;

        switch (current_code) {
            case qb::OpCode::USE_DEVICE:
                break;
            case qb::OpCode::USE_PORT:
                break;
            case qb::OpCode::SET:
                instruction = new qb::instruction::Set(device, port, *data);
                break;
            case qb::OpCode::SET_AT:
                instruction = new qb::instruction::SetAt(device, port, *index, *data);
                break;
            case qb::OpCode::HOLD:
                instruction = new qb::instruction::Hold(device);
                break;
            case qb::OpCode::RELEASE:
                instruction = new qb::instruction::Release(device);
                break;
            case qb::OpCode::GOTO:
                // address is resolved later
                instruction = new qb::instruction::Goto();
                break;
            case qb::OpCode::IF_EQ:
                // addresses are resolved later
                instruction = new qb::instruction::IfEq(device, port, *data);
                break;
            case qb::OpCode::IF_GT:
                // addresses are resolved later
                instruction = new qb::instruction::IfGt(device, port, *data);
                break;
            case qb::OpCode::IF_GTEQ:
                // addresses are resolved later
                instruction = new qb::instruction::IfGtEq(device, port, *data);
                break;
            case qb::OpCode::ADD:
                instruction = new qb::instruction::Add(device, port, *data);
                break;
            case qb::OpCode::SUB:
                instruction = new qb::instruction::Sub(device, port, *data);
                break;
            case qb::OpCode::MULT:
                instruction = new qb::instruction::Mult(device, port, *data);
                break;
            case qb::OpCode::DIV:
                instruction = new qb::instruction::Div(device, port, *data);
                break;
            case qb::OpCode::MOD:
                instruction = new qb::instruction::Mod(device, port, *data);
                break;
            case qb::OpCode::LOG:
                instruction = new qb::instruction::Log(device, *data);
                break;
            case qb::OpCode::SLEEP:
                instruction = new qb::instruction::Sleep(data->as_u32());
                data->purge();
                break;
            case qb::OpCode::STOP:
                instruction = new qb::instruction::Stop(data->as_string());
                data->purge();
                break;
            case qb::OpCode::ERROR:
                instruction = new qb::instruction::Error(data->as_string());
                data->purge();
                break;
            case qb::OpCode::RESET:
                instruction = new qb::instruction::Reset();
                break;
            case qb::OpCode::REBOOT:
            instruction = new qb::instruction::Reboot();
                break;
        }

        if (instruction != nullptr) {
            script->instructions.push_back(instruction);
            addr++;
        }

        current_code = 0;
    }

    // Link instructions
    qb::code_addr_t n_cmds = addr;

    for (qb::code_addr_t i = 0; i < n_cmds; i++) {
        qb::Instruction* cmd = script->instructions.at(i);
        
        qb::code_addr_t next_true = 0;
        qb::code_addr_t next_false = 0;

        if (gotos_true.contains(i)) {
            const qb::code_addr_t to = gotos_true.at(i);
            if (to == 0xFFFF) {
                if (i == n_cmds-1) next_true = 0xFFFF;
                else next_true = i+1;
            }
            else {
                if (to >= n_cmds) {
                    return {
                        .ok = false,
                        .message = "Invalid GOTO",
                        .script = nullptr
                    };
                }
                next_true = to;
            }
        }
        else {
            if (i == n_cmds-1) next_true = 0xFFFF;
            else next_true = i+1;
        }

        if (gotos_false.contains(i)) {
            const qb::code_addr_t to = gotos_false.at(i);
            if (to == 0xFFFF) {
                if (i == n_cmds-1) next_false = 0xFFFF;
                else next_false = i+1;
            }
            else {
                if (to >= n_cmds) {
                    return {
                        .ok = false,
                        .message = "Invalid GOTO",
                        .script = nullptr
                    };
                }
                next_false = to;
            }
        }

        cmd->next = next_true;
        
        if (
            current_code == qb::OpCode::IF_EQ
            || current_code == qb::OpCode::IF_GT
            || current_code == qb::OpCode::IF_GTEQ
        ) {
            ((qb::instruction::IfEq*)cmd)->next_false = next_false;
        }
    }
    
    delete[] bytecode.bytes;

    return {
        .ok = true,
        .script = script
    };
}

qb::Bytecode qb::parser::hexToBytecode(std::string hex) {
    size_t length = hex.length()/2;
    std::string s2;
    std::istringstream ss (hex);

    // std::cout << "length: " << length << std::endl;
    qb::code_t* bytes = new qb::code_t[length];
    for (size_t i = 0; (ss >> std::setw(2) >> s2); i++) {
        uint16_t u;                       
        std::istringstream ss2 (s2);    
        ss2 >> std::setbase(16) >> u;   
        bytes[i] = (code_t) static_cast<uint8_t>(u & 0xFF);
        // std::cout << "s2: " << s2 << ", u: " << u << ", b: " << unsigned(bytes[i]) << ", bb: " << ((uint16_t) bytes[i]) << std::endl;
    }
    
    return {
        .length = length,
        .bytes = bytes
    };
}

std::string qb::parser::bytecodeToHex(qb::Bytecode) {
    return "";
}

std::string qb::parser::vectorToHex(std::vector<qb::code_t> vec) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    std::vector<uint8_t>::const_iterator it;

    for (it = vec.begin(); it != vec.end(); it++) {
        ss << std::setw(2) << static_cast<unsigned>(*it);
    }

    return ss.str();
}