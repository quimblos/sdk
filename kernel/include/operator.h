#pragma once
#include <string>
#include <cmath>
#include "instruction.h"

#define UNRESOLVED_DATA { .type = DataType::VOID, .value = nullptr, .heap = true }
#define UNRESOLVED_DEVICE_DATA { .device = nullptr, .data = UNRESOLVED_DATA }

#define ERROR(MSG) { .data = nullptr, .error = new std::string(MSG) }

namespace qb {

    extern qb::data_t BOOL_TARGET_TYPE;
    extern qb::data_t U32_TARGET_TYPE;

    namespace _operator {

        struct res_t {
            data_t* data = nullptr;
            std::string* error = nullptr;
        };

        void delete_data(data_t* data);
        void clean_heap(_operator::res_t* res);

        res_t cast(data_t* target, data_t* source);
        res_t assign(data_t* target, data_t* source);
        res_t compare(data_t* target, data_t* source);
        res_t arithmetic_bool(InstructionType type, data_t* target, data_t* source);
        res_t arithmetic(InstructionType type, data_t* target, data_t* source);
        
    }

}