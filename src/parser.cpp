#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include "parser.h"

#define QB_LOG_DEBUG

using namespace qb;

const std::string E_UNEXPECTED_EOF = "Unexpected end of file";

#define ASSERT_N_BYTES(N) if (i > n-N) return { .ok = false, .message = E_UNEXPECTED_EOF }

parser::res_t parser::dump(Script& script) {
    return {
        .ok = true
    };
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
        
        if (!current_code) {
            current_code = bytes[i];
            i += 1;
            // code_addresses.push_back(i);
            #ifdef QB_LOG_TRACE
                std::cout << "[parser] @" << i-1;
                switch (current_code) {
                    case qb::CmdCode::USE_DEVICE:
                        std::cout << "+USE_DEVICE"; break;
                    case qb::CmdCode::SET:
                        std::cout << "+SET"; break;
                    case qb::CmdCode::GOTO:
                        std::cout << "+GOTO"; break;
                    case qb::CmdCode::IF_EQ:
                        std::cout << "+IF_EQ"; break;
                    case qb::CmdCode::IF_GT:
                        std::cout << "+IF_GT"; break;
                    case qb::CmdCode::IF_GTEQ:
                        std::cout << "+IF_GTEQ"; break;
                    case qb::CmdCode::ADD:
                        std::cout << "+ADD"; break;
                    case qb::CmdCode::SUB:
                        std::cout << "+SUB"; break;
                    case qb::CmdCode::MULT:
                        std::cout << "+MULT"; break;
                    case qb::CmdCode::DIV:
                        std::cout << "+DIV"; break;
                    case qb::CmdCode::MOD:
                        std::cout << "+MOD"; break;
                    case qb::CmdCode::LOG:
                        std::cout << "+LOG"; break;
                    case qb::CmdCode::SLEEP:
                        std::cout << "+SLEEP"; break;
                    case qb::CmdCode::STOP:
                        std::cout << "+STOP"; break;
                    case qb::CmdCode::ERROR:
                        std::cout << "+ERROR"; break;
                    case qb::CmdCode::RESET:
                        std::cout << "+RESET"; break;
                    case qb::CmdCode::REBOOT:
                        std::cout << "+REBOOT"; break;
                    default:
                        std::cout << "+?";break;
                }
                std::cout << std::endl;
            #endif
            continue;
        }

        uint8_t device_i = 0x00;
        uint8_t reg_i = 0x00;
        uint8_t log_code = 0x00;
        qb::DataType type = qb::DataType::UNKNOWN;

        // Header

        switch (current_code) {
            case qb::CmdCode::USE_DEVICE:
                {
                    type = qb::DataType::CONST;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "USE DEVICE ";
                    #endif
                }
                break;
            case qb::CmdCode::USE_REG:
                {
                    ASSERT_N_BYTES(1);
                    type = (qb::DataType) bytes[i];
                    i += 1;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "USE REG ";
                    #endif
                }
                break;
            case qb::CmdCode::SET:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    reg_i = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "SET (dev:" << ((uint16_t) device_i) << ", reg:" << ((uint16_t) reg_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::GOTO:
                {
                    ASSERT_N_BYTES(2);
                    qb::code_addr_t goto_true = (bytes[i] << 8) + bytes[i+1];
                    gotos_true.emplace(addr, goto_true);
                    type = qb::DataType::VOID;
                    i += 2;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "GOTO (addr:" << ((uint16_t) goto_true) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::IF_EQ:
                {
                    ASSERT_N_BYTES(7);
                    device_i = bytes[i];
                    reg_i = bytes[i+1];
                    qb::code_addr_t goto_true = (bytes[i+2] << 8) + bytes[i+3];
                    gotos_true.emplace(addr, goto_true);
                    qb::code_addr_t goto_false = (bytes[i+4] << 8) + bytes[i+5];
                    gotos_false.emplace(addr, goto_false);
                    type = (qb::DataType) bytes[i+6];
                    i += 7;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "IF_EQ (dev:" << ((uint16_t) device_i) << ", reg:" << ((uint16_t) reg_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::IF_GT:
                {
                    ASSERT_N_BYTES(7);
                    device_i = bytes[i];
                    reg_i = bytes[i+1];
                    qb::code_addr_t goto_true = (bytes[i+2] << 8) + bytes[i+3];
                    gotos_true.emplace(addr, goto_true);
                    qb::code_addr_t goto_false = (bytes[i+4] << 8) + bytes[i+5];
                    gotos_false.emplace(addr, goto_false);
                    type = (qb::DataType) bytes[i+6];
                    i += 7;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "IF_GT (dev:" << ((uint16_t) device_i) << ", reg:" << ((uint16_t) reg_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::IF_GTEQ:
                {
                    ASSERT_N_BYTES(7);
                    device_i = bytes[i];
                    reg_i = bytes[i+1];
                    qb::code_addr_t goto_true = (bytes[i+2] << 8) + bytes[i+3];
                    gotos_true.emplace(addr, goto_true);
                    qb::code_addr_t goto_false = (bytes[i+4] << 8) + bytes[i+5];
                    gotos_false.emplace(addr, goto_false);
                    type = (qb::DataType) bytes[i+6];
                    i += 7;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "IF_GTEQ (dev:" << ((uint16_t) device_i) << ", reg:" << ((uint16_t) reg_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::ADD:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    reg_i = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "ADD (dev:" << ((uint16_t) device_i) << ", reg:" << ((uint16_t) reg_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::SUB:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    reg_i = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "SUB (dev:" << ((uint16_t) device_i) << ", reg:" << ((uint16_t) reg_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::MULT:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    reg_i = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "MULT (dev:" << ((uint16_t) device_i) << ", reg:" << ((uint16_t) reg_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::DIV:
                {
                    ASSERT_N_BYTES(3);
                    device_i = bytes[i];
                    reg_i = bytes[i+1];
                    type = (qb::DataType) bytes[i+2];
                    i += 3;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "DIV (dev:" << ((uint16_t) device_i) << ", reg:" << ((uint16_t) reg_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::MOD:
                {
                    ASSERT_N_BYTES(2);
                    device_i = bytes[i];
                    log_code = bytes[i+1];
                    type = qb::DataType::STRING;
                    i += 2;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "MOD (dev:" << ((uint16_t) device_i) << ", type:" << ((uint16_t) type) << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::SLEEP:
                {
                    type = qb::DataType::UINT32;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "SLEEP ";
                    #endif
                }
                break;
            case qb::CmdCode::STOP:
                {
                    ASSERT_N_BYTES(1);
                    log_code = bytes[i];
                    type = qb::DataType::STRING;
                    i += 1;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "STOP ";
                    #endif
                }
                break;
            case qb::CmdCode::ERROR:
                {
                    ASSERT_N_BYTES(1);
                    log_code = bytes[i];
                    type = qb::DataType::STRING;
                    i += 1;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "ERROR (code:" << log_code << ") ";
                    #endif
                }
                break;
            case qb::CmdCode::RESET:
                {
                    type = qb::DataType::VOID;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "RESET ";
                    #endif
                }
                break;
            case qb::CmdCode::REBOOT:
                {
                    type = qb::DataType::VOID;
                    #ifdef QB_LOG_DEBUG
                        std::cout << "[parser] " << "REBOOT ";
                    #endif
                }
                break;
            default:
                return {
                    .ok = false,
                    .message = "Unknown CmdCode"
                };
        }

        // Script register

        if (current_code == qb::CmdCode::USE_REG) {
            script->registers.emplace_back(type);
            current_code = 0;
            continue;
        }

        // Value

        if (type == qb::DataType::UNKNOWN) {
            std::cout << "!" << std::endl;
            return {
                .ok = false,
                .message = "Unknown DataType"
            };
        }

        qb::Data* value = nullptr;
        switch (type) {
            case qb::DataType::VOID:
                value = qb::Data::_void().ref();
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::UINT8:
                ASSERT_N_BYTES(1);
                value = qb::Data::u8(bytes[i]).ref();
                i += 1;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::INT8:
                ASSERT_N_BYTES(1);
                value = qb::Data::i8(bytes[i]).ref();
                i += 1;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::BITMASK8:
                ASSERT_N_BYTES(1);
                value = qb::Data::u8(bytes[i]).ref();
                value->type = qb::DataType::BITMASK8;
                i += 1;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::UINT16:
                ASSERT_N_BYTES(2);
                value = qb::Data::u16((bytes[i] << 8) + bytes[i+1]).ref();
                i += 2;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::UINT8_XY:
                ASSERT_N_BYTES(2);
                value = qb::Data::u8xy(bytes[i], bytes[i+1]).ref();
                i += 2;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::INT16:
                ASSERT_N_BYTES(2);
                value = qb::Data::i16((bytes[i] << 8) + bytes[i+1]).ref();
                i += 2;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::INT8_XY:
                ASSERT_N_BYTES(2);
                value = qb::Data::i8xy(bytes[i], bytes[i+1]).ref();
                i += 2;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::UINT32:
                ASSERT_N_BYTES(4);
                value = qb::Data::u32((bytes[i] << 24) + (bytes[i+1] << 16) + (bytes[i+2] << 8) + bytes[i+3]).ref();
                i += 4;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::UINT16_XY:
                ASSERT_N_BYTES(4);
                value = qb::Data::u16xy((bytes[i] << 24) + (bytes[i+1] << 16), (bytes[i+2] << 8) + bytes[i+3]).ref();
                i += 4;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::UINT8_XYZW:
                ASSERT_N_BYTES(4);
                value = qb::Data::u8xyzw(bytes[i], bytes[i+1], bytes[i+2], bytes[i+3]).ref();
                i += 4;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::INT32:
                ASSERT_N_BYTES(4);
                value = qb::Data::i32((bytes[i] << 24) + (bytes[i+1] << 16) + (bytes[i+2] << 8) + bytes[i+3]).ref();
                i += 4;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::INT16_XY:
                ASSERT_N_BYTES(4);
                value = qb::Data::i16xy((bytes[i] << 24) + (bytes[i+1] << 16), (bytes[i+2] << 8) + bytes[i+3]).ref();
                i += 4;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::INT8_XYZW:
                ASSERT_N_BYTES(4);
                value = qb::Data::u8xyzw(bytes[i], bytes[i+1], bytes[i+2], bytes[i+3]).ref();
                i += 4;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::FLOAT32:
                ASSERT_N_BYTES(2);
                value = qb::Data::f32((bytes[i] << 24) + (bytes[i+1] << 16) + (bytes[i+2] << 8) + bytes[i+3]).ref();
                i += 2;
                #ifdef QB_LOG_DEBUG
                    value->log();
                #endif
                break;
            case qb::DataType::STRING:
                {
                    ASSERT_N_BYTES(2);
                    uint16_t length = *((uint16_t*) bytes+i);
                    i += 2;
                    ASSERT_N_BYTES(length);
                    value = qb::Data::string(length, (const char*) bytes+i).ref();
                    i += length;
                    #ifdef QB_LOG_DEBUG
                        value->log();
                    #endif
                }
                break;
            case qb::DataType::CONST:
                {
                    ASSERT_N_BYTES(1);
                    uint8_t length = *((uint8_t*) bytes+i);
                    i += 1;
                    ASSERT_N_BYTES(length);
                    value = qb::Data::string(length, (const char*) bytes+i).ref();
                    i += length;
                    #ifdef QB_LOG_DEBUG
                        value->log();
                    #endif
                }
                break;
        }
        std::cout << std::endl;
        
        // Use Device / Get Registers

        qb::Device* device = nullptr;
        switch (current_code) {
            case qb::CmdCode::USE_DEVICE:
                {
                    device = engine.getDevice(value->as_string());
                    if (device == nullptr) {
                        return {
                            .ok = false,
                            .message = "Device out of range"
                        };
                    }
                    devices.push_back(device);
                    
                    value->purge();
                    delete value;
                }
                break;
            case qb::CmdCode::SET:
            case qb::CmdCode::IF_EQ:
            case qb::CmdCode::IF_GT:
            case qb::CmdCode::IF_GTEQ:
            case qb::CmdCode::ADD:
            case qb::CmdCode::SUB:
            case qb::CmdCode::MULT:
            case qb::CmdCode::DIV:
            case qb::CmdCode::MOD:
                {
                    if (device_i == 0xFF) {
                        if (reg_i >= script->registers.size()) {
                            return {
                                .ok = false,
                                .message = "Register out of range"
                            };
                        }
                    }
                    else {
                        if (device_i >= devices.size()) {
                            return {
                                .ok = false,
                                .message = "Device out of range"
                            };
                        }
                        device = devices.at(device_i);
                        if (!device->has_i(reg_i)) {
                            return {
                                .ok = false,
                                .message = "Register out of range"
                            };
                        }
                    }
                }
                break;
            case qb::CmdCode::GOTO:
            case qb::CmdCode::LOG:
            case qb::CmdCode::SLEEP:
            case qb::CmdCode::STOP:
            case qb::CmdCode::ERROR:
            case qb::CmdCode::RESET:
            case qb::CmdCode::REBOOT:
                break;
        }
        // Command

        // (don't include parser commands)
        if (
            current_code != qb::CmdCode::USE_DEVICE
        ) {
            script->cmds.emplace_back(
                (qb::CmdCode) current_code,
                device,
                reg_i,
                value,
                0,
                0
            );
            addr++;
        }

        current_code = 0;
    }

    // Check code pointers (GOTO)
    qb::code_addr_t n_cmds = addr;

    for (qb::code_addr_t i = 0; i < n_cmds; i++) {
        qb::Cmd& cmd = script->cmds.at(i);
        if (gotos_true.contains(i)) {
            const qb::code_addr_t to = gotos_true.at(i);
            if (to == 0xFFFF) {
                if (i == n_cmds-1) cmd.next = 0xFFFF;
                else cmd.next = i+1;
            }
            else {
                if (to >= n_cmds) {
                    return {
                        .ok = false,
                        .message = "Invalid GOTO",
                        .script = nullptr
                    };
                }
                cmd.next = to;
            }
        }
        else {
            if (i == n_cmds-1) cmd.next = 0xFFFF;
            else cmd.next = i+1;
        }

        if (gotos_false.contains(i)) {
            const qb::code_addr_t to = gotos_false.at(i);
            if (to == 0xFFFF) {
                if (i == n_cmds-1) cmd.next_false = 0xFFFF;
                else cmd.next_false = i+1;
            }
            else {
                if (to >= n_cmds) {
                    return {
                        .ok = false,
                        .message = "Invalid GOTO",
                        .script = nullptr
                    };
                }
                cmd.next_false = to;
            }
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