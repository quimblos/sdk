#pragma once
#include <cstring>
#include <map>
#include <vector>
#include "type.h"

#define B_TYPES_0 0xF3

#define B_TYPE_VOID 0xFF
#define B_TYPE_NULL 0xFE
#define B_TYPE_BOOL 0xFD
#define B_TYPE_U8 0xFC
#define B_TYPE_I8 0xFB
#define B_TYPE_U16 0xFA
#define B_TYPE_I16 0xF9
#define B_TYPE_U32 0xF8
#define B_TYPE_I32 0xF7
#define B_TYPE_F32 0xF6
#define B_TYPE_STR 0xF5
#define B_TYPE_REF 0xF4
#define B_TYPE_REF_SLICE 0xF3

namespace qb {

    struct TypeDef {
        struct {
            qb::TypeKind kind = qb::TypeKind::VOID;
            std::vector<TypeDef> children = {};
        } add = {};
        type_t use = B_TYPE_VOID;

        ~TypeDef() {
            
        }

        static TypeDef block(const std::vector<TypeDef>& children) {
            return TypeDef({
                .add = {
                    .kind = qb::TypeKind::STRUCT,
                    .children = children
                }
            });
        }

        static TypeDef _add(qb::TypeKind kind, const std::vector<TypeDef>& children) {
            return TypeDef({
                .add = {
                    .kind = kind,
                    .children = children
                }
            });
        }

        static TypeDef _use(type_t type) {
            return TypeDef({
                .use = type
            });
        }
    };

    class TypeBlock {

        public:
            static const Type builtin_types[];
            
        
        private:
            std::vector<Type*> types;

        public:

            ~TypeBlock() {
                for (port_t i = 0; i < this->types.size(); i++) {
                    delete this->types[i];
                }
                // free(this->types);
            }

            const Type* get(type_t index) const {
                if (index >= B_TYPES_0) {
                    return &TypeBlock::builtin_types[0xFF-index];
                }
                if (index >= this->types.size()) {
                    return nullptr;
                }
                return this->types[index];
            }
            
            const type_t add_from_def(const TypeDef& type_def);

            const type_t add_vec(const type_t item_type);
            const type_t add_map(const type_t item_type);
            const type_t add_struct(const std::vector<type_t>& fields);
            const type_t add_event(const type_t item_type);
            
    };


}