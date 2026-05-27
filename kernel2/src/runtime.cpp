#include "runtime.h"

qb::runtime::Reference qb::runtime::resolve_ref(const qb::Context& context, const qb::mem::Reference& ref) {
    const qb::mem::Block* block = nullptr;
    switch (ref.block) {
        case BLOCK_ENGINE:
            block = context.thread->get_node()->get_engine()->get_block();
            break;
        case BLOCK_NODE:
            block = context.thread->get_node()->get_block();
            break;
        case BLOCK_THREAD:
            block = context.thread->get_block();
            break;
        case BLOCK_CONTEXT:
            block = &context.block;
            break;
    }
    if (block == nullptr) {
        return {
            .type = nullptr,
            .value = nullptr
        };
    }
    return {
        .type = block->data.get_type(ref.port),
        .value = block->data.get(ref.port)
    };
}