#pragma once
#include <string>
#include "data.h"

#define UNRESOLVED { .type = qb::DataType::VOID, .value = nullptr, .heap = true }

#define ERROR(MSG) { .data = nullptr, .error = new std::string(MSG) }

namespace qb {

    namespace _operator {

        struct res_t {
            data_t* data = nullptr;
            std::string* error = nullptr;
        };

        void delete_data(qb::data_t* data);
        void clean_heap(qb::_operator::res_t* res);

        res_t cast(type_t type, data_t* source);
        res_t assign(data_t* target, data_t* source);
        res_t compare(data_t* target, data_t* source);
        
    }

}