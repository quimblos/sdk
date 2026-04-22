#pragma once
#include <vector>
#include "data.h"

#define HEADER_QUIMBLOS 'q', 'b', 0x00, 0x00

namespace qb {

    /*
        Operation Codes
    */
    
    enum InstructionType {
        // 0x0* -> Parser
        USE_DEVICE = 0x01,    // {name:str}
        USE_VAR = 0x02,      // {source:any}
        // 0x1* -> Data manipulation
        SET = 0x10,           // {target:ref} {source:any}
        HOLD = 0x1A,          // {device:u8}
        RELEASE = 0x1B,       // {device:u8}
        // 0x2* -> Flow control 
        GOTO = 0x20,          // {code:u16}
        BRANCH = 0x21,        // {source:any} {true_code:u16} {false_code:u16}
        // 0x3* -> Conditional data manipulation 
        SET_IF_EQ = 0x30,     // {target:ref} {left:any} {right:any} {true:any} {false:any}
        SET_IF_LT = 0x31,     // {target:ref} {left:any} {right:any} {true:any} {false:any}
        SET_IF_GT = 0x32,     // {target:ref} {left:any} {right:any} {true:any} {false:any}
        // 0x4* -> Boolean Arithmetic 
        NOT = 0x40,           // {target:reg} {source:any}
        AND = 0x41,           // {target:reg} {source:any}
        OR = 0x42,            // {target:reg} {source:any}
        // 0x5* -> Arithmetic 
        ADD = 0x50,           // {target:reg} {source:any}
        SUB = 0x51,           // {target:reg} {source:any}
        MULT = 0x52,          // {target:reg} {source:any}
        DIV = 0x53,           // {target:reg} {source:any}
        MOD = 0x54,           // {target:reg} {source:any}
        POW = 0x55,           // {target:reg} {source:any}
        // 0xE* -> Runner
        SLEEP = 0xE0,         // {time:any}
        LOG = 0xEA,           // {source:any}
        RETURN = 0xEF,        // {source:any}
        // 0xF* -> Engine
        RESET = 0xF0,         // !
        REBOOT = 0xFF,        // !
    };
    
    /*
        Response
    */

    struct Instruction;
    namespace instruction {
        struct res_t {
            uint8_t code;
            Instruction* instr;
            code_addr_t next_addr;
        };
    }

    /*
        Instructions
    */

    struct Instruction { 
        const InstructionType type;                // The type of instruction
    
        Instruction(InstructionType type) : type(type) {}
        virtual ~Instruction() {};

        virtual std::string to_str() const = 0;

        // Builds a instruction from a byte sequence
        static instruction::res_t make(code_t* bytes, code_addr_t code_len, code_addr_t addr);
    };

    namespace instruction {
        
        // Parser

        struct UseDevice: public Instruction {
            const std::string name;

            UseDevice(std::string name):
                Instruction(InstructionType::USE_DEVICE),
                name(name)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "use_device " << this->name;
                return ss.str();
            }
        };

        struct UseVar: public Instruction {
            const qb::Data* data;

            UseVar(qb::Data* data):
                Instruction(InstructionType::USE_VAR),
                data(data)
            {}
            ~UseVar() {
                delete data;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << "use_var " << this->data->to_str();
                return ss.str();
            }
        };
        
        // Data manipulation

        struct Set: public Instruction {
            const data::Reference target;
            const Data* data;

            Set(data::Reference* target, Data* data):
                Instruction(InstructionType::SET),
                target(*target), data(data)
            {}
            ~Set() {
                delete this->data;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << this->target.to_str() << " = " << this->data->to_str();
                return ss.str();
            }
        };

        struct Hold: public Instruction {
            const device_t device;

            Hold(device_t device):
                Instruction(InstructionType::HOLD),
                device(device)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "hold " << this->device;
                return ss.str();
            }
        };

        struct Release: public Instruction {
            const device_t device;

            Release(device_t device):
                Instruction(InstructionType::RELEASE),
                device(device)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "release " << this->device;
                return ss.str();
            }
        };

        // Flow Control

        struct Goto: public Instruction {
            const code_addr_t addr;

            Goto(code_addr_t addr):
                Instruction(InstructionType::GOTO),
                addr(addr)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "goto " << this->addr;
                return ss.str();
            }
        };

        struct Branch: public Instruction {
            const qb::Data* data;
            const code_addr_t addr_true;
            const code_addr_t addr_false;

            Branch(qb::Data* data, code_addr_t addr_true, code_addr_t addr_false):
                Instruction(InstructionType::BRANCH),
                data(data), addr_true(addr_true), addr_false(addr_false)
            {}
            ~Branch() {
                delete this->data;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << "if " << this->data->to_str() << " @" << this->addr_true << " else @" << this->addr_false;
                return ss.str();
            }
        };

        // ConditionalSet

        struct ConditionalSet: public Instruction {
            const data::Reference target;
            const Data* left;
            const Data* right;
            const Data* data_true;
            const Data* data_false;

            ConditionalSet(InstructionType type, data::Reference* target, Data* left, Data* right, Data* data_true, Data* data_false):
                Instruction(type),
                target(*target), left(left), right(right), data_true(data_true), data_false(data_false)
            {}
            ~ConditionalSet() {
                delete this->left;
                delete this->right;
                delete this->data_true;
                delete this->data_false;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << this->target.to_str() << " = " << this->left->to_str() << this->type << this->right->to_str() << " ? " << this->data_true->to_str() << " : " << this->data_false->to_str();
                return ss.str();
            }
        };
        
        // Arithmetic

        struct Arithmetic: public Instruction {
            const data::Reference target;
            const Data* data;

            Arithmetic(InstructionType type, data::Reference* target, Data* data):
                Instruction(type),
                target(*target), data(data)
            {}
            ~Arithmetic() {
                delete this->data;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << this->target.to_str();
                switch (this->type) {
                    case qb::InstructionType::NOT: ss << " = !"; break;
                    case qb::InstructionType::AND: ss << " &&= "; break;
                    case qb::InstructionType::OR: ss << " ||= "; break;
                    case qb::InstructionType::ADD: ss << " += "; break;
                    case qb::InstructionType::SUB: ss << " -= "; break;
                    case qb::InstructionType::MULT: ss << " *= "; break;
                    case qb::InstructionType::DIV: ss << " /= "; break;
                    case qb::InstructionType::MOD: ss << " %= "; break;
                    case qb::InstructionType::POW: ss << " ^= "; break;
                }
                ss << this->data->to_str();
                return ss.str();
            }
        };

        // Runner

        struct Sleep: public Instruction {
            const uint32_t time;

            Sleep(uint32_t time):
                Instruction(InstructionType::SLEEP),
                time(time)
            {}

            std::string to_str() const {
                std::stringstream ss;
                ss << "sleep " << this->time;
                return ss.str();
            }
        };
        
        struct Log: public Instruction {
            const Data* data;

            Log(Data* data):
                Instruction(InstructionType::LOG),
                data(data)
            {}
            ~Log() {
                delete this->data;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << "log " << this->data->to_str();
                return ss.str();
            }
        };
        
        struct Return: public Instruction {
            const Data* data;

            Return(Data* data):
                Instruction(InstructionType::RETURN),
                data(data)
            {}

            ~Return() {
                delete this->data;
            }

            std::string to_str() const {
                std::stringstream ss;
                ss << "return " << this->data->to_str();
                return ss.str();
            }
        };

    }

}