#pragma once
#include <string>
#include <cmath>
#include "instruction.h"

#define UNRESOLVED_DATA { .type = DataType::VOID, .value = nullptr, .heap = true }
#define UNRESOLVED_DEVICE_DATA { .device = nullptr, .data = UNRESOLVED_DATA }

#define ERROR(MSG) { .data = nullptr, .error = new std::string(MSG) }

namespace qb {

    // Resolved data, used to read/write from memory
    // allocated inside Data objects.
    //
    // - `value`:
    //   - (null) nullptr
    //   - (numeric, string) a pointer to the memory allocated by the Data object
    //   - (error, vector or ref) a pointer to the Data object
    //   - (sliced vector) a pointer to a iterator_t
    // - `heap`:
    //   - (true) this data should be deleted after the operation
    //   - (false) this data lifecycle is not manageable by this operation
    struct data_t {
        type_t type;
        void* value;
        bool heap = false;
    };

    struct data_slice_t {
        type_t type;
        void* value = nullptr;
        bool heap = false;
        data::Slice* slice = nullptr;

        ~data_slice_t() {
            delete this->slice;
        }
    };

    extern data_t BOOL_TARGET_TYPE;
    extern data_t U32_TARGET_TYPE;
   
    namespace _operator {

        struct res_t {
            data_t* data = nullptr;
            std::string* error = nullptr;
        };

        void clean_data(data_t* res);
        // data_t* copy_data(data_t* res);
        void clean_heap(_operator::res_t* res);

        res_t cast(data_t* target, data_t* source);
        res_t assign(data_t* target, data_t* source);
        res_t compare(data_t* target, data_t* source);
        res_t arithmetic_bool(InstructionType type, data_t* target, data_t* source);
        res_t arithmetic(InstructionType type, data_t* target, data_t* source);
        
    }

}