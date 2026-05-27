#pragma once
#include "engine.h"

namespace qb {

    namespace runtime {

        struct Reference {
            const Type* type;
            const data_t value;
        };

        runtime::Reference resolve_ref(const qb::Context& context, const qb::mem::Reference& ref);
        
    };
    
}