#include "types.h"
#include <iostream>

/* Built-In Types */

const qb::Type qb::TypeChecker::builtin_types[] = {
    
    // 0x00: void
    qb::Type({
        .kind = qb::TypeKind::VOID,
        .flags = qb::TypeFlags({
            .of_void = {
                .is_void = true,
            }
        })
    }),
    
    // 0x01: null
    qb::Type({
        .kind = qb::TypeKind::VOID,
        .flags = qb::TypeFlags({
            .of_void = {
                .is_null = true,
            }
        })
    }),
    
    // 0x02: bool
    qb::Type({
        .kind = qb::TypeKind::BOOL,
        .flags = qb::TypeFlags({
            .value = 0
        })
    }),

    // 0x03: u8
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .is_unsigned = true,
                .res = 1
            }
        })
    }),

    // 0x04: i8
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .res = 1
            }
        })
    }),

    // 0x05: u16
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .is_unsigned = true,
                .res = 2
            }
        })
    }),

    // 0x06: i16
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .res = 2
            }
        })
    }),

    // 0x07: u32
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .is_unsigned = true,
                .res = 4
            }
        })
    }),

    // 0x08: i32
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .res = 4
            }
        })
    }),

    // 0x09: f32
    qb::Type({
        .kind = qb::TypeKind::FLOAT,
        .flags = 0
    }),

    // 0x0A: string
    qb::Type({
        .kind = qb::TypeKind::STRING,
        .flags = 0
    }),


    // 0x0B: ref
    qb::Type({
        .kind = qb::TypeKind::REF,
        .flags = 0
    })

};

/* Extendable Types */

// vec
const qb::type_t qb::TypeChecker::add_vec(const qb::type_t item_type) {
    this->types.push_back(new qb::Type({
        .kind = qb::TypeKind::VECTOR,
        .flags = 0,
        .schema = qb::TypeSchema({
            .of_map = {
                .type = this->get(item_type)
            }
        })
    }));
    
    uint8_t idx = this->types.size() - 1;
    return N_B_TYPES + idx;
}

// map
const qb::type_t qb::TypeChecker::add_map(const qb::type_t item_type) {
    this->types.push_back(new qb::Type({
        .kind = qb::TypeKind::OBJ,
        .flags = qb::TypeFlags({
            .of_obj = {
                .is_map = true
            }
        }),
        .schema = qb::TypeSchema({
            .of_map = {
                .type = this->get(item_type)
            }
        })
    }));
    uint8_t idx = this->types.size() - 1;
    return N_B_TYPES + idx;
}

// struct
const qb::type_t qb::TypeChecker::add_struct(const std::vector<type_t>& field_types) {    
    
    uint8_t n_fields = (uint8_t) field_types.size();
    auto fields = new const Type*[n_fields];

    for (size_t i = 0; i < n_fields; i++) {
        fields[i] = this->get(field_types[i]);
    }

    this->types.push_back(new qb::Type({
        .kind = qb::TypeKind::OBJ,
        .flags = qb::TypeFlags({
            .of_obj = {}
        }),
        .schema = qb::TypeSchema({
            .of_struct = {
                .n_fields = n_fields,
                .fields = fields
            }
        })
    }));
    uint8_t idx = this->types.size() - 1;
    return N_B_TYPES + idx;
}

// event
const qb::type_t qb::TypeChecker::add_event(const qb::type_t inner_type) {
    this->types.push_back(new qb::Type({
        .kind = qb::TypeKind::EVENT,
        .flags = 0,
        .schema = qb::TypeSchema({
            .of_map = {
                .type = this->get(inner_type)
            }
        })
    }));
    
    uint8_t idx = this->types.size() - 1;
    return N_B_TYPES + idx;
}
