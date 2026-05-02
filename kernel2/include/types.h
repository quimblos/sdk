#pragma once
#include <cstring>
#include <map>
#include <vector>
#include "type.h"

#define N_B_TYPES 12
#define B_TYPE_VOID 0x00
#define B_TYPE_NULL 0x01
#define B_TYPE_BOOL 0x02
#define B_TYPE_U8 0x03
#define B_TYPE_I8 0x04
#define B_TYPE_U16 0x05
#define B_TYPE_I16 0x06
#define B_TYPE_U32 0x07
#define B_TYPE_I32 0x08
#define B_TYPE_F32 0x09
#define B_TYPE_STR 0x0A
#define B_TYPE_REF 0x0B

namespace qb {

    class TypeChecker {

        public:
            static const Type builtin_types[];
        
        private:
            std::vector<Type*> types;

        public:

            ~TypeChecker() {
                for (uint8_t i = 0; i < this->types.size(); i++) {
                    delete this->types[i];
                }
                // free(this->types);
            }

            const Type* get(type_t index) const {
                if (index < N_B_TYPES) {
                    return &TypeChecker::builtin_types[index];
                }
                if (index >= this->types.size()+N_B_TYPES) {
                    return nullptr;
                }
                return this->types[index-N_B_TYPES];
            }
            
            const type_t add_map(const type_t item_type);
            const type_t add_struct(const std::vector<type_t>& fields);
            const type_t add_event(const type_t item_type);
            const type_t add_vec(const type_t item_type);
            
    };


}