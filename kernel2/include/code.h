#pragma once
#include <vector>
#include "memory.h"

#define HEADER_QUIMBLOS 'q', 'b', 0x00, 0x00

namespace qb {

    /*
        Operation Codes
    */
    
    enum OpCode {
        // 0x0* -> Node
        USE_DRIVER = 0x01,      // (driver:str)
        USE_REF = 0x02,         // (code:str, source:ptr)
        // 0x1* -> Parser
        ADD_TYPE = 0x06,        // (kind:8, ...type_def)
        ADD_CONST = 0x07,       // (type:_t, ...data)
        ADD_ARG = 0x08,         // (type:_t)
        ADD_VAR = 0x09,         // (type:_t)
        // 0x2* -> Memory Manipulation
        SET = 0x20,             // (flags:8, target:ptr, source:ptr)
        HOLD = 0x2A,            // (driver:str)
        RELEASE = 0x2B,         // (driver:str)
        // 0x3* -> Flow control 
        GOTO = 0x30,            // (addr:_t)
        IF = 0x31,              // (flags:8, source:ptr, true_addr:_t, false_addr: _t)
        // 0x4* -> Conditional data manipulation 
        SET_IF = 0x40,          // (flags:8, target:ptr, left:ptr, right:ptr, true:ptr?, false:ptr?)
        // 0x5* -> Math 
        MATH = 0x50,       // (flags:8, target:ptr, source:ptr)
            // NOT
            // AND
            // OR
            // NAND
            // XOR
            // ADD
            // SUB
            // MULT
            // DIV
            // MOD
            // POW
            // LN
        // 0xD* -> Context
        RETURN = 0xD0,          // (source:ptr)
        // 0xE* -> Thread
        SLEEP = 0xE0,           // (time:ptr)
        // 0xF* -> Engine
        PUBLISH = 0xF1,         // (topic:str, source:ptr)
        REBOOT = 0xFF,          // ()
    };
    
    /*
        Instruction
    */

    struct Instruction {
        const OpCode type;
    
        Instruction(OpCode type) : type(type) {}
        virtual ~Instruction() {}

        virtual const std::string to_str() const = 0;
    };

    /*
        Code
    */

    struct Code {
        struct Data {
            const type_t tdx;
            const byte_t* bytes;
            code_addr_t length;
        };
        
        const std::vector<std::string> drivers;
        const std::vector<TypeDef> types;
        const std::vector<Data> consts;
        const std::vector<type_t> args;
        const std::vector<type_t> vars;
        const std::vector<Instruction*> instructions;
        const std::vector<Code*> children;
        const mem::Reference* out_value;

        Code(
            const std::vector<std::string>& drivers,
            const std::vector<TypeDef> types,
            const std::vector<Data> consts,
            const std::vector<type_t>& args,
            const std::vector<type_t>& vars,
            const std::vector<Instruction*>& instructions,
            // const std::vector<Code*>& children
            const mem::Reference* out_value
        ) :
            drivers(drivers),
            types(types),
            consts(consts),
            args(args),
            vars(vars),
            instructions(instructions),
            // children(children),
            out_value(out_value) {}

        ~Code() {
            for (size_t i = 0; i < this->instructions.size(); i++) {
                delete this->instructions[i];
            }
        }

        std::string to_str() {
            std::stringstream ss;
            for (size_t i = 0; i < this->instructions.size(); i++) {
                ss << this->instructions[i]->to_str() << std::endl;
            }
            return ss.str();
        }
    };

    /*
        Instruction Declarations
    */

    namespace instruction {
        
        // Parser

        struct UseDriver: public Instruction {
            const std::string name;

            UseDriver(const std::string& name):
                Instruction(OpCode::USE_DRIVER),
                name(name)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "#block " << this->name;
                return ss.str();
            }
        };

        struct UseRef: public Instruction {
            const std::string name;
            const mem::Reference source;

            UseRef(const std::string& name, const mem::Reference& source):
                Instruction(OpCode::USE_REF),
                name(name),
                source(source)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "#block " << this->name;
                return ss.str();
            }
        };

        struct AddType: public Instruction {
            const TypeDef type_def;

            AddType(const TypeDef& type_def):
                Instruction(OpCode::ADD_TYPE),
                type_def(type_def)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "type:...";
                return ss.str();
                // auto type = Type({
                //     .kind = this->kind,
                //     .flags = this->flags,
                //     .schema = this->schema == nullptr ? {} : *this->schema,
                // });
                // return type.to_str();
            }
        };

        struct AddConst: public Instruction {
            const type_t tdx;
            const byte_t* bytes;
            code_addr_t length;

            AddConst(type_t tdx, const byte_t* bytes, code_addr_t length):
                Instruction(OpCode::ADD_CONST),
                tdx(tdx),
                bytes(bytes),
                length(length)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "const:" << +this->type << " = ...";
                return ss.str();
            }
        };

        struct AddArg: public Instruction {
            const type_t tdx;

            AddArg(type_t tdx):
                Instruction(OpCode::ADD_ARG),
                tdx(tdx)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "arg:" << +this->tdx;
                return ss.str();
            }
        };

        struct AddVar: public Instruction {
            const type_t tdx;

            AddVar(type_t tdx):
                Instruction(OpCode::ADD_VAR),
                tdx(tdx)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "var:" << +this->tdx;
                return ss.str();
            }
        };
        
        // Data manipulation

        struct Set: public Instruction {
            const struct Flags {
                const bool deref_target: 1 = false;
                const bool deref_source: 1 = false;
                const bool explicit_cast: 1 = false;
                uint8_t _: 5 = 0;
            } flags;
            const mem::Reference target;
            const mem::Reference source;

            Set(const Flags& flags, const mem::Reference& target, const mem::Reference& source):
                Instruction(OpCode::SET),
                flags(flags),
                target(target),
                source(source)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                if (this->flags.deref_target) ss << '*';
                ss << this->target.to_str() << " = ";
                if (this->flags.deref_source) ss << '*';
                ss << this->source.to_str();
                return ss.str();
            }
        };

        struct Hold: public Instruction {
            const std::string driver;

            Hold(const std::string& driver):
                Instruction(OpCode::HOLD),
                driver(driver)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "hold @" << this->driver;
                return ss.str();
            }
        };

        struct Release: public Instruction {
            const std::string driver;

            Release(const std::string& driver):
                Instruction(OpCode::RELEASE),
                driver(driver)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "release @" << this->driver;
                return ss.str();
            }
        };

        // Flow Control

        struct Goto: public Instruction {
            const code_addr_t addr;

            Goto(code_addr_t addr):
                Instruction(OpCode::GOTO),
                addr(addr)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "goto " << +this->addr;
                return ss.str();
            }
        };

        struct If: public Instruction {
            const struct Flags {
                const bool deref_source: 1 = false;
                const uint8_t _: 7 = 0;
            } flags;
            const mem::Reference source;
            const code_addr_t addr_true;
            const code_addr_t addr_false;

            If(const Flags& flags, const mem::Reference& source, code_addr_t addr_true, code_addr_t addr_false):
                Instruction(OpCode::IF),
                flags(flags),
                source(source),
                addr_true(addr_true),
                addr_false(addr_false)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "if " << this->source.to_str() << " @" << +this->addr_true << " else @" << +this->addr_false;
                return ss.str();
            }
        };

        // SetIf

        struct SetIf: public Instruction {
            const struct Flags {
                const bool deref_target: 1 = false;
                const bool deref_left: 1 = false;
                const bool deref_right: 1 = false;
                const bool deref_data: 1 = false;
                const bool has_true: 1 = false;
                const bool has_false: 1 = false;
                const enum Op {
                    EQ = 0x00,
                    GT = 0x01,
                    LT = 0x02,
                } op : 2;
            } flags;
            const mem::Reference target;
            const mem::Reference left;
            const mem::Reference right;
            const mem::Reference data_true;
            const mem::Reference data_false;

            SetIf(const Flags& flags, const mem::Reference& target, const mem::Reference& left, const mem::Reference& right, const mem::Reference& data_true, const mem::Reference& data_false):
                Instruction(OpCode::SET_IF),
                flags(flags),
                target(target),
                left(left),
                right(right),
                data_true(data_true),
                data_false(data_false)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                if (this->flags.deref_target) ss << '*';
                ss << this->target.to_str() << " = ";
                if (this->flags.deref_left) ss << '*';
                ss << this->left.to_str();
                switch (this->flags.op) {
                    case Flags::Op::EQ: ss << " == "; break;
                    case Flags::Op::GT: ss << " > "; break;
                    case Flags::Op::LT: ss << " < "; break;
                }
                if (this->flags.deref_right) ss << '*';
                ss << this->right.to_str() << " ? ";
                if (this->flags.deref_data) ss << '*';
                ss << this->data_true.to_str();
                ss << " : ";
                if (this->flags.deref_data) ss << '*';
                ss << this->data_false.to_str();
                return ss.str();
            }
        };
        
        // Math

        struct Math: public Instruction {
            const struct Flags {
                const bool deref_target: 1 = false;
                const bool deref_source: 1 = false;
                const enum Op {
                    NOT = 0x00,
                    AND = 0x01,
                    OR = 0x02,
                    // NAND = 0x00,
                    // XOR = 0x00,
                    ADD = 0x10,
                    SUB = 0x11,
                    MULT = 0x12,
                    DIV = 0x13,
                    MOD = 0x14,
                    POW = 0x15,
                    LN = 0x16,
                    // EML = 0x20,
                } op : 6;
            } flags;
            const mem::Reference target;
            const mem::Reference source;

            Math(const Flags& flags, const mem::Reference& target, const mem::Reference& source):
                Instruction(qb::OpCode::MATH),
                flags(flags),
                target(target),
                source(source)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << this->target.to_str();
                switch (this->flags.op) {
                    case Flags::Op::NOT: ss << " = !" << this->target.to_str(); return ss.str();
                    case Flags::Op::AND: ss << " &&= "; break;
                    case Flags::Op::OR: ss << " ||= "; break;
                    case Flags::Op::ADD: ss << " += "; break;
                    case Flags::Op::SUB: ss << " -= "; break;
                    case Flags::Op::MULT: ss << " *= "; break;
                    case Flags::Op::DIV: ss << " /= "; break;
                    case Flags::Op::MOD: ss << " %= "; break;
                    case Flags::Op::POW: ss << " ^= "; break;
                    case Flags::Op::LN: ss << " = ln(" << this->target.to_str() << ")"; return ss.str();
                }
                ss << this->source.to_str();
                return ss.str();
            }
        };
        
        // Context

        struct Return: public Instruction {
            const mem::Reference source;

            Return(const mem::Reference& source):
                Instruction(OpCode::RETURN),
                source(source)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "return " << this->source.to_str();
                return ss.str();
            }
        };

        // Thread

        struct Sleep: public Instruction {
            const mem::Reference time;

            Sleep(const mem::Reference& time):
                Instruction(OpCode::SLEEP),
                time(time)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "sleep " << this->time.to_str();
                return ss.str();
            }
        };

        // Engine
        
        struct Publish: public Instruction {
            const std::string topic;
            const mem::Reference source;

            Publish(const std::string& topic, const mem::Reference& source):
                Instruction(OpCode::PUBLISH),
                topic(topic),
                source(source)
            {}

            const std::string to_str() const {
                std::stringstream ss;
                ss << "publish@" << this->topic << " " << this->source.to_str();
                return ss.str();
            }
        };
        
    }

}