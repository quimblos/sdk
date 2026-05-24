#include "typeblock.h"

/* Built-In Types */

const qb::Type qb::TypeBlock::builtin_types[] = {
    
    // 0xFF: void
    qb::Type({
        .kind = qb::TypeKind::VOID,
        .flags = qb::TypeFlags({
            .of_void = {
                .is_void = true,
            }
        })
    }),
    
    // 0xFE: null
    qb::Type({
        .kind = qb::TypeKind::VOID,
        .flags = qb::TypeFlags({
            .of_void = {
                .is_null = true,
            }
        })
    }),
    
    // 0xFD: bool
    qb::Type({
        .kind = qb::TypeKind::BOOL,
        .flags = qb::TypeFlags({
            .value = 0
        })
    }),

    // 0xFC: u8
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .is_unsigned = true,
                .res = 1
            }
        })
    }),

    // 0xFB: i8
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .res = 1
            }
        })
    }),

    // 0xFA: u16
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .is_unsigned = true,
                .res = 2
            }
        })
    }),

    // 0xF9: i16
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .res = 2
            }
        })
    }),

    // 0xF8: u32
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .is_unsigned = true,
                .res = 4
            }
        })
    }),

    // 0xF7: i32
    qb::Type({
        .kind = qb::TypeKind::INT,
        .flags = qb::TypeFlags({
            .of_int = {
                .res = 4
            }
        })
    }),

    // 0xF6: f32
    qb::Type({
        .kind = qb::TypeKind::FLOAT,
        .flags = 0
    }),

    // 0xF5: string
    qb::Type({
        .kind = qb::TypeKind::STRING,
        .flags = 0
    }),

    // 0xF4: ref
    qb::Type({
        .kind = qb::TypeKind::REF,
        .flags = 0
    }),

    // 0xF3: ref_slice
    qb::Type({
        .kind = qb::TypeKind::REF_SLICE,
        .flags = 0
    })

};

/* Extendable Types */

// from schema
const qb::type_t qb::TypeBlock::add_from_def(const qb::TypeDef& type_def) {
    // Is use
    if (type_def.add.kind == qb::TypeKind::VOID) {
        return type_def.use;
    }
    // Is add
    switch (type_def.add.kind) {
        case qb::TypeKind::VOID:
        case qb::TypeKind::BOOL:
        case qb::TypeKind::INT:
        case qb::TypeKind::FLOAT:
        case qb::TypeKind::STRING:
        case qb::TypeKind::REF:
        case qb::TypeKind::REF_SLICE:
            break;
        case qb::TypeKind::VECTOR: {
            auto item_type = this->add_from_def(type_def.add.children[0]);
            return this->add_vec(item_type);
        }
        case qb::TypeKind::MAP: {
            auto item_type = this->add_from_def(type_def.add.children[0]);
            return this->add_map(item_type);
        }
        case qb::TypeKind::STRUCT: {
            auto item_types = std::vector<type_t>(type_def.add.children.size());
            for (type_t i = 0; i < item_types.size(); i++) {
                item_types[i] = this->add_from_def(type_def.add.children[i]);
            }
            return this->add_struct(item_types);
        }
        case qb::TypeKind::EVENT: {
            auto item_type = this->add_from_def(type_def.add.children[0]);
            return this->add_event(item_type);
        }
        case qb::TypeKind::FN:
            return B_TYPE_VOID;
    }

    return type_def.use;
}

// vec
const qb::type_t qb::TypeBlock::add_vec(const qb::type_t item_type) {
    this->types.push_back(new qb::Type({
        .kind = qb::TypeKind::VECTOR,
        .flags = 0,
        .schema = qb::TypeSchema({
            .of_map = {
                .type = this->get(item_type)
            }
        })
    }));
    
    port_t idx = this->types.size() - 1;
    return idx;
}

// map
const qb::type_t qb::TypeBlock::add_map(const qb::type_t item_type) {
    this->types.push_back(new qb::Type({
        .kind = qb::TypeKind::MAP,
        .flags = 0,
        .schema = qb::TypeSchema({
            .of_map = {
                .type = this->get(item_type)
            }
        })
    }));
    port_t idx = this->types.size() - 1;
    return idx;
}

// struct
const qb::type_t qb::TypeBlock::add_struct(const std::vector<type_t>& field_types) {    
    
    port_t n_fields = (port_t) field_types.size();
    auto fields = new const Type*[n_fields];

    for (size_t i = 0; i < n_fields; i++) {
        fields[i] = this->get(field_types[i]);
    }

    auto schema = qb::TypeSchema({
        .of_struct = {
            .n_fields = n_fields,
            .fields = fields
        }
    });

    this->types.push_back(new qb::Type({
        .kind = qb::TypeKind::STRUCT,
        .flags = 0,
        .schema = schema
    }));
    port_t idx = this->types.size() - 1;
    return idx;
}

// event
const qb::type_t qb::TypeBlock::add_event(const qb::type_t inner_type) {
    this->types.push_back(new qb::Type({
        .kind = qb::TypeKind::EVENT,
        .flags = 0,
        .schema = qb::TypeSchema({
            .of_map = {
                .type = this->get(inner_type)
            }
        })
    }));
    
    port_t idx = this->types.size() - 1;
    return idx;
}
