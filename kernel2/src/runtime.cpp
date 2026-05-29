#include "runtime.h"

#define QB_RUNTIME_DEBUG

#define RESOLVE_BLOCK(PROP, INSTR_TYPE) \
    auto PROP##_block = qb::runtime::resolve_block(context, ((qb::instruction::INSTR_TYPE*)instr)->PROP); \
    if (PROP##_block == nullptr) { \
        out_error = new qb::runtime::Error({ \
            .code = qb::runtime::Error::UNRESOLVED_REF_BLOCK, \
            .addr = addr, \
            .instr = instr \
        }); \
        return 0xFFFF; \
    } \
    auto PROP##_port = ((qb::instruction::INSTR_TYPE*)instr)->PROP.port;

#define REF_CPP_GET(PROP, TYPE) \
    auto PROP = PROP##_block->data.__cpp_get<TYPE>(PROP##_port);

#define REF_GET(PROP) \
    auto PROP = PROP##_block->data.get(PROP##_port);

#define REF_GET_TYPE(PROP) \
    auto PROP##_type = PROP##_block->data.get_type(PROP##_port);

#define REF_SET(TARGET, SOURCE) \
    TARGET##_block->data.set(TARGET##_port, SOURCE);

#define CAST_TO_BUILTIN(NAME, TDX, CTYPE, PROP, ERROR) \
    auto TDX##_type = &TypeBlock::builtin_types[0xFF-TDX]; \
    auto TDX##_res = qb::op::cast(TDX##_type, PROP##_type, PROP, true); \
    if (TDX##_res.code != qb::op::res_t::Code::OK) { \
        out_error = new qb::runtime::Error({ \
            .code = qb::runtime::Error::ERROR, \
            .addr = addr, \
            .instr = instr \
        }); \
        return 0xFFFF; \
    } \
    auto NAME = *(CTYPE*) TDX##_res.out; \
    if (TDX##_res.temp) delete TDX##_res.out;

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
    }
    return nullptr;
}

qb::code_addr_t qb::runtime::run_instruction(qb::Context* context, qb::code_addr_t addr, qb::Instruction* instr, Error* out_error) {

    switch (instr->type) {
        case qb::OpCode::SET: {
            RESOLVE_BLOCK(target, Set);
            RESOLVE_BLOCK(source, Set);
            REF_GET(source);
            REF_SET(target, source);

            #ifdef QB_RUNTIME_DEBUG
                REF_GET_TYPE(source)
                REF_GET_TYPE(target)
                auto res = qb::op::cast_to_string(source_type, source, true);
                if (res.code != 0) {
                    LOG((uint8_t) res.code)
                }
                else {
                    auto target_ref = ((qb::instruction::Set*)instr)->target;
                    auto source_str = *(std::string*) res.out;
                    std::cout << "SET (" << target_type->to_str() << ") " << target_ref.to_str() << " = (" << source_type->to_str() << ')' <<source_str << std::endl;
                    if (res.temp) delete res.out;
                }
            #endif
            break;
        }
        case qb::OpCode::HOLD:
            std::cout << "HOLD" << std::endl;
            break;
        case qb::OpCode::RELEASE:
            std::cout << "RELEASE" << std::endl;
            break;
        case qb::OpCode::GOTO: {
            auto go_to = (qb::instruction::Goto*)instr;

            #ifdef QB_RUNTIME_DEBUG
                std::cout << "GOTO " << go_to << std::endl;
            #endif
            return go_to->addr;
        }
        case qb::OpCode::IF:
            std::cout << "IF" << std::endl;
            break;
        case qb::OpCode::SET_IF:
            std::cout << "SET_IF" << std::endl;
            break;
        case qb::OpCode::MATH:
            std::cout << "MATH" << std::endl;
            break;
        case qb::OpCode::RETURN:
            std::cout << "RETURN" << std::endl;
            break;
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
        case qb::OpCode::PUBLISH:
            std::cout << "PUBLISH" << std::endl;
            break;
        case qb::OpCode::REBOOT:
            std::cout << "REBOOT" << std::endl;
            break;
    }
    return addr + 1;
}

std::string qb::runtime::block_to_str(const qb::mem::Block* block, std::string prefix) {
    std::stringstream ss;

    auto n = block->data.size();
    for (port_t i = 0; i < n; i++) {

        auto type = block->data.get_type(i);
        ss << prefix << +i << ":" << type->to_str();
        if (block->is_const[i]) ss << " const";
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