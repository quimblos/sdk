#include "runtime.h"

#define QB_RUNTIME_DEBUG

#define RUNTIME_OK(ADDR) \
    return { \
        .code = qb::runtime::res_t::Code::OK, \
        .addr = ADDR \
    };

#define RUNTIME_ERROR(ERROR) \
    return { \
        .code = qb::runtime::res_t::Code::ERROR, \
        .addr = addr \
    };

#define RESOLVE_BLOCK(PROP, INSTR_TYPE) \
    auto PROP##_block = qb::runtime::resolve_block(context, ((qb::instruction::INSTR_TYPE*)instr)->PROP); \
    if (PROP##_block == nullptr) { \
        RUNTIME_ERROR(UNRESOLVED_REF_BLOCK) \
    } \
    auto PROP##_port = ((qb::instruction::INSTR_TYPE*)instr)->PROP.port;
    
#define RESOLVE_DRIVER(PROP, INSTR_TYPE) \
    auto PROP##_driver = ((qb::instruction::INSTR_TYPE*)instr)->PROP.block < BLOCK_CONTEXT \
        ? context->thread->get_driver(((qb::instruction::INSTR_TYPE*)instr)->PROP.block) \
        : nullptr;

#define REF_CPP_GET(PROP, TYPE) \
    auto PROP = PROP##_block->data.__cpp_get<TYPE>(PROP##_port);

#define REF_GET(PROP) \
    if (PROP##_port >= PROP##_block->data.size()) \
        RUNTIME_ERROR(UNRESOLVED_REF_PORT) \
    auto PROP = PROP##_block->data.get(PROP##_port);

#define REF_GET_TYPE(PROP) \
    auto PROP##_type = PROP##_block->data.get_type(PROP##_port);

#define REF_SET(TARGET, SOURCE) \
    TARGET##_block->data.set(TARGET##_port, SOURCE);

#define CAST_TO_BUILTIN(NAME, TDX, CTYPE, PROP, ERROR) \
    auto TDX##_type = &TypeBlock::builtin_types[0xFF-TDX]; \
    auto TDX##_res = qb::op::cast(TDX##_type, PROP##_type, PROP, true); \
    if (TDX##_res.code != qb::op::res_t::Code::OK) { \
        RUNTIME_ERROR(ERROR) \
    } \
    auto NAME = *(CTYPE*) TDX##_res.out; \
    if (TDX##_res.temp) delete TDX##_res.out;

#define RUN_BLOCK_OP(OP...) \
    if (target_driver != nullptr) { \
        if (target_driver->is_held(context->thread)) { \
            ((qb::Thread*)context->thread)->set_waiting_driver(true); \
            RUNTIME_OK(addr); \
        } \
    } \
    auto res = OP; \
    if (res.code != qb::op::res_t::Code::OK) { \
        LOG(res.code) \
        RUNTIME_ERROR(FAILED_MATH) \
    } \
    if (context->thread->get_state() == qb::Thread::State::WAITING_DRIVER) { \
        ((qb::Thread*)context->thread)->set_waiting_driver(false); \
    } \
    if (target_driver != nullptr) { \
        target_driver->render(target_port); \
    }

qb::mem::Block* qb::runtime::resolve_block(qb::Context* context, const qb::mem::Reference& ref) {
    switch (ref.block) {
        case BLOCK_ENGINE:
            return (qb::mem::Block*) context->thread->get_node()->get_engine()->get_block();
        case BLOCK_NODE:
            return (qb::mem::Block*) context->thread->get_node()->get_block();
        case BLOCK_THREAD:
            return (qb::mem::Block*) context->thread->get_block();
        case BLOCK_CONTEXT:
            return (qb::mem::Block*) &context->block;
        default:
            return (qb::mem::Block*) context->thread->get_driver(ref.block)->get_block();
    }
    return nullptr;
}

qb::runtime::res_t qb::runtime::run_instruction(qb::Context* context, qb::code_addr_t addr, qb::Instruction* instr) {

    switch (instr->type) {
        
        /* Set */
        case qb::OpCode::SET: {
            RESOLVE_BLOCK(target, Set)
            RESOLVE_BLOCK(source, Set)
            RESOLVE_DRIVER(target, Set)

            RUN_BLOCK_OP(qb::op::assign(*target_block, target_port, *source_block, source_port))
            
            #ifdef QB_RUNTIME_DEBUG
                REF_GET_TYPE(source)
                REF_GET_TYPE(target)
                REF_GET(source);
                auto res_debug = qb::op::cast_to_string(source_type, source, true);
                if (res_debug.code != 0) {
                    LOG((uint8_t) res_debug.code)
                }
                else {
                    auto target_ref = ((qb::instruction::Set*)instr)->target;
                    auto source_str = *(std::string*) res_debug.out;
                    std::cout << "SET (" << target_type->to_str() << ")" << target_ref.to_str() << " = (" << source_type->to_str() << ')' <<source_str << std::endl;
                    if (res_debug.temp) delete (std::string*) res_debug.out;
                }
            #endif
            break;
        }

        /* Hold */
        case qb::OpCode::HOLD: {
            auto driver_res = context->thread->get_node()->get_engine()->get_driver(((qb::instruction::Hold*)instr)->driver);
            if (driver_res.code != qb::engine::res_t::Code::OK) {
                RUNTIME_ERROR(UNRESOLVED_DRIVER);
            }
            auto driver = driver_res.out.driver;
            driver->hold((qb::Thread*) context->thread);
            #ifdef QB_RUNTIME_DEBUG
                std::cout << "HOLD " << driver->get_name() << std::endl;
            #endif
            break;
        }

        /* Release */
        case qb::OpCode::RELEASE: {
            auto driver_res = context->thread->get_node()->get_engine()->get_driver(((qb::instruction::Release*)instr)->driver);
            if (driver_res.code != qb::engine::res_t::Code::OK) {
                RUNTIME_ERROR(UNRESOLVED_DRIVER);
            }
            auto driver = driver_res.out.driver;
            driver->release((qb::Thread*) context->thread);
            #ifdef QB_RUNTIME_DEBUG
                std::cout << "RELEASE " << driver->get_name() << std::endl;
            #endif
            break;
        }

        /* Goto */
        case qb::OpCode::GOTO: {
            auto go_to = (qb::instruction::Goto*)instr;

            #ifdef QB_RUNTIME_DEBUG
                std::cout << "GOTO " << go_to->addr << std::endl;
            #endif
            RUNTIME_OK(go_to->addr);
        }

        /* If */
        case qb::OpCode::IF: {
            RESOLVE_BLOCK(source, If);
            REF_GET(source);
            REF_GET_TYPE(source);
            CAST_TO_BUILTIN(source_bool, B_TYPE_BOOL, bool, source, FAILED_CAST_TO_BOOL);
            
            auto _if = (qb::instruction::If*)instr;
            if (source_bool) {
                RUNTIME_OK(_if->addr_true)
            }
            else {
                RUNTIME_OK(_if->addr_false)
            }
            
            #ifdef QB_RUNTIME_DEBUG
                auto res_debug = qb::op::cast_to_string(source_type, source, true);
                if (res_debug.code != 0) {
                    LOG((uint8_t) res_debug.code)
                }
                else {
                    auto source_str = *(std::string*) res_debug.out;
                    std::cout << "IF (" << source_type->to_str() << ") " << source_str << " GOTO " << _if->addr_true << " ELSE " << _if->addr_false << std::endl;
                }
            #endif
            break;
        }

        case qb::OpCode::SET_IF: {
            RESOLVE_BLOCK(target, SetIf)
            RESOLVE_BLOCK(left, SetIf)
            RESOLVE_BLOCK(right, SetIf)
            RESOLVE_BLOCK(data_true, SetIf)
            RESOLVE_BLOCK(data_false, SetIf)
            RESOLVE_DRIVER(target, SetIf)

            auto compare_res = qb::op::compare(
                ((qb::instruction::SetIf*)instr)->flags.op,
                *left_block, left_port, *right_block, right_port
            );
            if (compare_res.code != qb::op::res_t::Code::OK) {
                LOG(compare_res.code)
                RUNTIME_ERROR(FAILED_COMPARE)
            }

            if (*(bool*)compare_res.out) {
                RUN_BLOCK_OP(qb::op::assign(*target_block, target_port, *data_true_block, data_true_port))
            }
            else {
                RUN_BLOCK_OP(qb::op::assign(*target_block, target_port, *data_false_block, data_false_port))
            }
            delete (bool*) compare_res.out;
         
            #ifdef QB_RUNTIME_DEBUG
                REF_GET_TYPE(target)
                REF_GET_TYPE(left)
                REF_GET_TYPE(right)
                REF_GET_TYPE(data_true)
                REF_GET_TYPE(data_false)
                REF_GET(left);
                REF_GET(right);
                REF_GET(data_true);
                REF_GET(data_false);
                auto res_debug_left = qb::op::cast_to_string(left_type, left, true);
                if (res_debug_left.code != 0) {
                    LOG((uint8_t) res_debug_left.code)
                }
                else {
                    auto res_debug_right = qb::op::cast_to_string(right_type, right, true);
                    if (res_debug_right.code != 0) {
                        LOG((uint8_t) res_debug_right.code)
                    }
                    else {
                        auto res_debug_data_true = qb::op::cast_to_string(data_true_type, data_true, true);
                        if (res_debug_data_true.code != 0) {
                            LOG((uint8_t) res_debug_data_true.code)
                        }
                        else {
                            auto res_debug_data_false = qb::op::cast_to_string(data_false_type, data_false, true);
                            if (res_debug_data_false.code != 0) {
                                LOG((uint8_t) res_debug_data_false.code)
                            }
                            else {
                                auto target_ref = ((qb::instruction::SetIf*)instr)->target;
                                auto left_str = *(std::string*) res_debug_left.out;
                                auto right_str = *(std::string*) res_debug_right.out;
                                auto data_true_str = *(std::string*) res_debug_data_true.out;
                                auto data_false_str = *(std::string*) res_debug_data_false.out;
                                std::cout << "SET (" << target_type->to_str() << ")" << target_ref.to_str();
                                std::cout << " = [(" << left_type->to_str() << ')' << left_str;
                                switch (((qb::instruction::SetIf*)instr)->flags.op) {
                                    case qb::instruction::CompareOp::EQ: std::cout << " == "; break;
                                    case qb::instruction::CompareOp::GT: std::cout << " > "; break;
                                    case qb::instruction::CompareOp::LT: std::cout << " < "; break;
                                }
                                std::cout << "(" << right_type->to_str() << ')' << right_str << "]";
                                std::cout << " ? (" << data_true_type->to_str() << ')' << data_true_str;
                                std::cout << " : (" << data_true_type->to_str() << ')' << data_false_str;
                                std::cout << std::endl;
                                if (res_debug_left.temp) delete (std::string*) res_debug_left.out;
                                if (res_debug_right.temp) delete (std::string*) res_debug_right.out;
                                if (res_debug_data_true.temp) delete (std::string*) res_debug_data_true.out;
                                if (res_debug_data_false.temp) delete (std::string*) res_debug_data_false.out;
                            }
                        }
                    }
                }
            #endif
            break;
        }

        /* Math */
        case qb::OpCode::MATH: {
            RESOLVE_BLOCK(target, Math)
            RESOLVE_BLOCK(source, Math)
            RESOLVE_DRIVER(target, Math)
            
            RUN_BLOCK_OP(qb::op::math(
                ((qb::instruction::Math*)instr)->flags.op,
                *target_block, target_port, *source_block, source_port
            ))
            
            #ifdef QB_RUNTIME_DEBUG
                REF_GET_TYPE(source)
                REF_GET_TYPE(target)
                REF_GET(source);
                auto res_debug = qb::op::cast_to_string(source_type, source, true);
                if (res_debug.code != 0) {
                    LOG((uint8_t) res_debug.code)
                }
                else {
                    auto target_ref = ((qb::instruction::Math*)instr)->target;
                    auto source_str = *(std::string*) res_debug.out;
                    std::cout << "MATH (" << target_type->to_str() << ")" << target_ref.to_str();
                    switch (((qb::instruction::Math*)instr)->flags.op) {
                        case qb::instruction::Math::Flags::Op::NOT: std::cout << " != "; break;
                        case qb::instruction::Math::Flags::Op::AND: std::cout << " and= "; break;
                        case qb::instruction::Math::Flags::Op::OR: std::cout << " or= "; break;
                        // case qb::instruction::Math::Flags::Op::NAND: std::cout << " NAND= "; break;
                        // case qb::instruction::Math::Flags::Op::XOR: std::cout << " XOR= "; break;
                        case qb::instruction::Math::Flags::Op::ADD: std::cout << " += "; break;
                        case qb::instruction::Math::Flags::Op::SUB: std::cout << " -= "; break;
                        case qb::instruction::Math::Flags::Op::MULT: std::cout << " *= "; break;
                        case qb::instruction::Math::Flags::Op::DIV: std::cout << " /= "; break;
                        case qb::instruction::Math::Flags::Op::MOD: std::cout << " %= "; break;
                        case qb::instruction::Math::Flags::Op::POW: std::cout << " ^= "; break;
                        case qb::instruction::Math::Flags::Op::LN: std::cout << " = ln "; break;
                        // case qb::instruction::Math::Flags::Op::EML: std::cout << " EML= "; break;
                    }
                    std::cout << "(" << source_type->to_str() << ')' <<source_str << std::endl;
                    if (res_debug.temp) delete (std::string*) res_debug.out;
                }
            #endif
            break;
        }

        /* Return */
        case qb::OpCode::RETURN: {
            RESOLVE_BLOCK(source, Return);
            auto res = qb::op::assign(context->block, 0, *source_block, source_port);
            if (res.code != qb::op::res_t::Code::OK) {
                LOG(res.code)
                RUNTIME_ERROR(FAILED_RETURN)
            }
            #ifdef QB_RUNTIME_DEBUG
                REF_GET_TYPE(source)
                REF_GET(source);
                auto res_debug = qb::op::cast_to_string(source_type, source, true);
                if (res_debug.code != 0) {
                    LOG((uint8_t) res_debug.code)
                }
                else {
                    auto source_str = *(std::string*) res_debug.out;
                    std::cout << "RETURN" << " (" << source_type->to_str() << ')' <<source_str << std::endl;
                    if (res_debug.temp) delete (std::string*) res_debug.out;
                }
            #endif
            break;
        }

        /* Sleep */
        case qb::OpCode::SLEEP: {
            RESOLVE_BLOCK(time, Sleep);
            REF_GET(time);
            REF_GET_TYPE(time);
            CAST_TO_BUILTIN(time_u32, B_TYPE_U32, uint32_t, time, FAILED_CAST_TO_TIME);
            
            ((qb::Thread*)context->thread)->set_sleep(time_u32);
            
            #ifdef QB_RUNTIME_DEBUG
                std::cout << "SLEEP " << time_u32 << "ms" << std::endl;
            #endif
            break;
        }

        /* Publish */
        case qb::OpCode::PUBLISH:
            std::cout << "TODO: PUBLISH" << std::endl;
            break;

        /* Reboot */
        case qb::OpCode::REBOOT:
            std::cout << "TODO: REBOOT" << std::endl;
            break;
    }
    RUNTIME_OK((qb::code_addr_t)(addr + 1))
}

std::string qb::runtime::block_to_str(const qb::mem::Block* block, std::string prefix) {
    std::stringstream ss;

    auto n = block->data.size();
    for (port_t i = 0; i < n; i++) {

        auto type = block->data.get_type(i);
        ss << prefix << +i << ":" << type->to_str();
        if (block->data.type->schema.of_struct.is_const[i]) ss << " const";
        ss << " = ";

        auto value = block->data.get(i);
        auto res = qb::op::cast_to_string(type, value, true);
        if (res.code != 0) {
            ss << "??";
        }
        else {
            ss << *(std::string*) res.out;
            if (res.temp) delete (std::string*) res.out;
        }
        if (i < n-1)
            ss << std::endl;
    }

    return ss.str();
}

bool qb::runtime::link_drivers(const qb::Engine* engine, qb::Thread* thread) {
    auto code = thread->get_code();
    for (qb::block_t i = 0; i < code->drivers.size(); i++) {
        auto res = engine->get_driver(code->drivers[i]);
        if (res.code != qb::engine::res_t::Code::OK) {
            return false;
        }
        thread->link_driver(i, res.out.driver);
    }
    return true;
}