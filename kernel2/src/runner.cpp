#include "runner.h"

#define QB_RUNNER_DEBUG

#ifdef QB_RUNNER_DEBUG
    #include <iostream>
#endif

#define ASSERT_TARGET() \
    if (target.type == qb::DataType::VOID) { \
        return this->_return(qb::data::error(0, "Unresolved target reference")); \
    }

#define ASSERT_SOURCE(NAME) \
    if (NAME.type == qb::DataType::VOID) { \
        return this->_return(qb::data::error(0, "Unresolved " #NAME " reference")); \
    }

#define ASSERT_OPERATOR_RES(RES) \
    if (RES.error != nullptr) { \
        return this->_return(qb::data::error(0, *RES.error)); \
    }

qb::Runner::Runner(qb::Engine& engine, std::string name, qb::Program* program):
    engine(&engine),
    name(name),
    program(program),
    length(program->instructions.size())
{}

qb::Runner::~Runner() {
    for (qb::Data* var : this->variables) {
        delete var;
    }
    if (this->output) {
        delete this->output;
    }
}

void qb::Runner::wakeup() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[wakeup]" << std::endl;
    #endif
    this->state = runner::State::RUNNING;
    this->output = nullptr;
    this->sleep = 0;
}

void qb::Runner::reset() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[reset]" << std::endl;
    #endif

    for (qb::Data* var : this->variables) {
        delete var;
    }

    this->variables.clear();
    for (qb::Data* var : this->program->variables) {
        this->variables.push_back(var->copy());
    }

    this->devices.clear();
    for (uint8_t i = 0; i < this->program->devices.size(); i++) {
        std::string device_name = this->program->devices.at(i);
        qb::Device* device = this->engine->get_device(device_name);
        if (device == nullptr) {
            this->_return(qb::data::error(0, "Device not found"));
            return;
        }
        this->devices.push_back(device);
    }

    this->wakeup();
}

void qb::Runner::start() {
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[start]" << std::endl;
    #endif
    this->cursor = 0;
    this->reset();
}

bool qb::Runner::tick() {
    if (this->state != runner::State::RUNNING) {
        #ifdef QB_RUNNER_DEBUG
            std::cout << "[not running: " << this->state << "]" << std::endl;
        #endif
        return false;
    }
    if (this->cursor >= this->length) {
        #ifdef QB_RUNNER_DEBUG
            std::cout << "[done]" << std::endl;
        #endif
        this->state = runner::State::OK;
        return false;
    }

    qb::Instruction* instr = this->program->instructions.at(this->cursor);
    
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[tick] " << this->cursor << "/" << this->length << "\t" << instr->to_str() << std::endl;
    #endif

    qb::code_addr_t next = this->run_instruction(instr);

    if (next >= this->length) {
        if (this->state == runner::State::RUNNING) {
            this->state = runner::State::OK;
        }
        return false;
    }

    this->cursor = next;
    return true;
}

qb::data_t qb::Runner::resolve_data(const qb::Data* data) const {

    void* value;
    switch (data->type) {
        case DataType::_NULL: value = nullptr; break;
        case DataType::ERROR: value = (void*) data; break;
        case DataType::BOOL: value = &((qb::data::Numeric<bool>*) data)->value; break;
        case DataType::UINT8: value = &((qb::data::Numeric<uint8_t>*) data)->value; break;
        case DataType::INT8: value = &((qb::data::Numeric<int8_t>*) data)->value; break;
        case DataType::UINT16: value = &((qb::data::Numeric<uint16_t>*) data)->value; break;
        case DataType::INT16: value = &((qb::data::Numeric<int16_t>*) data)->value; break;
        case DataType::UINT32: value = &((qb::data::Numeric<uint32_t>*) data)->value; break;
        case DataType::INT32: value = &((qb::data::Numeric<int32_t>*) data)->value; break;
        case DataType::FLOAT32: value = &((qb::data::Numeric<float>*) data)->value; break;
        case DataType::STRING: value = &((qb::data::String*) data)->value; break;
        case DataType::ARRAY: value = (void*) data; break;
        case DataType::REF: return this->resolve_ref((qb::data::Reference*) data);
        default: return UNRESOLVED;
    }

    return { .type = data->type, .value = value };
}

qb::data_t qb::Runner::resolve_ref(const qb::data::Reference* ref) const {

    qb::Data* data;
    // Runner Variable
    if (ref->device == DEVICE_RUNNER) {
        if (ref->port >= this->variables.size()) return UNRESOLVED;
        data = this->variables.at(ref->port);
    }
    // Device Variable
    else {
        qb::Device* device = this->devices.at(ref->device);
        data = device->get_variable(ref->port);
        if (data == nullptr) return UNRESOLVED;
    }

    // Array
    if (data->type == qb::DataType::ARRAY) {
        auto array = (qb::data::Array<void>*) data;
        if (ref->index >= array->length) return UNRESOLVED;

        void* item = array->ptr_at(ref->index);

        // Dereference Array Item
        if (ref->deref) {
            if (array->item_type != qb::DataType::REF) return UNRESOLVED;
            return this->resolve_ref((qb::data::Reference*) item);
        }
        return { .type = array->item_type, .value = item };
    }

    // Dereference
    if (ref->deref) {
        if (data->type != qb::DataType::REF) return UNRESOLVED;
        return this->resolve_ref((qb::data::Reference*) data);
    }

    return this->resolve_data(data);
}

qb::code_addr_t qb::Runner::run_instruction(qb::Instruction* instr) {
    switch (instr->type) {
        
        // Data Manipulation

        case qb::InstructionType::SET: {
            auto it = (qb::instruction::Set*) instr;
            auto target = this->resolve_ref(&it->target);
            ASSERT_TARGET()
            auto source = this->resolve_data(it->data);
            ASSERT_SOURCE(source)
            auto res = qb::_operator::assign(&target, &source);
            ASSERT_OPERATOR_RES(res)
            return this->cursor + 1;
        }

        case qb::InstructionType::HOLD: {
            auto it = (qb::instruction::Hold*) instr;
            this->devices[it->device]->hold();
            return this->cursor + 1;
        }

        case qb::InstructionType::RELEASE: {
            auto it = (qb::instruction::Release*) instr;
            this->devices[it->device]->release();
            return this->cursor + 1;
        }

        // Flow Control

        case qb::InstructionType::GOTO: {
            auto it = (qb::instruction::Goto*) instr;
            return it->addr;
        }

        case qb::InstructionType::BRANCH: {
            auto it = (qb::instruction::Branch*) instr;
            auto source = this->resolve_data(it->data);
            ASSERT_SOURCE(source)
            auto res = qb::_operator::cast(qb::DataType::BOOL, &source);
            ASSERT_OPERATOR_RES(res)
            bool value = *(bool*) res.data->value;
            qb::_operator::clean_heap(&res);

            if (value) return it->addr_true;
            return it->addr_false;
        }

        // Conditional Data Manipulation

        case qb::InstructionType::SET_IF_EQ:
        case qb::InstructionType::SET_IF_LT:
        case qb::InstructionType::SET_IF_GT: {
            auto it = (qb::instruction::ConditionalSet*) instr;
            auto target = this->resolve_ref(&it->target);
            ASSERT_TARGET()
            auto left = this->resolve_data(it->left);
            ASSERT_SOURCE(left)
            auto right = this->resolve_data(it->right);
            ASSERT_SOURCE(right)
            auto data_true = this->resolve_data(it->data_true);
            ASSERT_SOURCE(data_true)
            auto data_false = this->resolve_data(it->data_false);
            ASSERT_SOURCE(data_false)
            auto compare_res = qb::_operator::compare(&left, &right);
            ASSERT_OPERATOR_RES(compare_res)
            uint8_t diff = *(uint8_t*) compare_res.data->value;
            qb::_operator::clean_heap(&compare_res);

            std::cout << "left: " << it->left->to_str() << std::endl;
            std::cout << "left: " << +(*(uint8_t*) left.value) << std::endl;
            std::cout << "right: " << it->right->to_str() << std::endl;
            std::cout << "right: " << +(*(uint8_t*) right.value) << std::endl;
            std::cout << "diff: " << +diff << std::endl;

            bool cond = false;
            if (instr->type == qb::InstructionType::SET_IF_EQ) {
                cond = diff == 0;
            }

            auto assign_res = qb::_operator::assign(&target, cond ? &data_true : &data_false);
            ASSERT_OPERATOR_RES(assign_res)
            return this->cursor + 1;
        }

        // Arithmetic

        case qb::InstructionType::AND:
        case qb::InstructionType::OR:
        case qb::InstructionType::XOR:
        case qb::InstructionType::ADD:
        case qb::InstructionType::SUB:
        case qb::InstructionType::MULT:
        case qb::InstructionType::DIV:
        case qb::InstructionType::MOD:
        case qb::InstructionType::POW: {
            // TODO
            return this->cursor + 1;
        }

        // Runner

        case qb::InstructionType::SLEEP: {
            auto it = (qb::instruction::Sleep*) instr;
            this->state = qb::runner::State::SLEEPING;
            this->sleep = it->time;
            return this->cursor + 1;
        }

        case qb::InstructionType::LOG: {
            auto it = (qb::instruction::Log*) instr;
            std::cout << it->data->to_str() << std::endl;
            return this->cursor + 1;
        }

        case qb::InstructionType::RETURN: {
            auto it = (qb::instruction::Return*) instr;
            return this->_return(it->data->copy());
        }

        // Engine

        case qb::InstructionType::RESET: {
            // TODO
            return this->cursor + 1;
        }

        case qb::InstructionType::REBOOT: {
            // TODO
            return this->cursor + 1;
        }

        default:
            return this->_return(qb::data::error(0, "Unknown instruction type"));
    }
}

qb::code_addr_t qb::Runner::_return(qb::Data* out) {
    if (out->type == qb::DataType::ERROR) {
        this->state = qb::runner::State::ERROR;
    }
    else {
        this->state = qb::runner::State::OK;
    }
    this->output = out;
    #ifdef QB_RUNNER_DEBUG
        std::cout << "[return] " << this->output->to_str() << std::endl;
    #endif
    return 0xFFFF;
}