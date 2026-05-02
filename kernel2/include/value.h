// #pragma once
// #include <cstring>
// #include <map>
// #include <vector>
// #include "types.h"

// namespace qb {

//     template <typename T>
//     struct Value {
//         const type_t type;
//         T data;

//         template <typename X>
//         Value<X>* as() {
//             return (Value<X>*) this;
//         }

//         template <typename X>
//         X* data_as() {
//             return(X*) &this->data;
//         }
//     };
//     typedef Value<void*>* value_ptr;
//     typedef uint8_t void_t;

//     // Data Implementation (by Type Kind)

//     // typedef Data<bool> BoolData;
    
//     // struct IntData: public Data<uint8_t*> {        
//     //     IntData(uint8_t res, void* value) {
//     //         this->value = new uint8_t[res];
//     //         memcpy(this->value, value, res);
//     //     }
//     //     ~IntData() {
//     //         delete[] this->value;
//     //     }
//     // };

//     // typedef Data<float> FloatData;
//     // typedef Data<std::string> StringData;
    
//     // struct MapData: public Data<std::map<std::string, Data*>> {
//     //     MapData(std::map<std::string, Data*> value): value(value) {}
//     //     ~MapData() {
//     //         for (auto& it : this->value) {
//     //             delete it.second;
//     //         }
//     //     }
//     // };

//     // typedef Data<std::vector<Data*>> StructData;

//     // struct EventData: public Data<Value*> {
//     //     enum LevelKind {
//     //         TRACE = 0x00,
//     //         DEBUG = 0x01,
//     //         INFO = 0x02,
//     //         WARN = 0x03,
//     //         ERROR = 0x04,
//     //     } level;
//     //     std::string name;
//     //     EventData(LevelKind level, std::string name, Value* value): level(level), name(name), value(value) {}
//     //     ~EventData() {
//     //         delete this->value;
//     //     }
//     // };

//     // typedef Data<std::vector<Data*>> VectorData;
//     // typedef Data<Value*> RefData;

//     namespace value {
//         qb::value_ptr _void();
//         qb::value_ptr null();
//         qb::value_ptr _bool(bool value = false);
//         qb::value_ptr u8(uint8_t value = 0);
//         qb::value_ptr i8(int8_t value = 0);
//         qb::value_ptr u16(uint16_t value = 0);
//         qb::value_ptr i16(int16_t value = 0);
//         qb::value_ptr u32(uint32_t value = 0);
//         qb::value_ptr i32(int32_t value = 0);
//         qb::value_ptr f32(float value = 0);
//         qb::value_ptr str(std::string value = "");
//         qb::value_ptr ref(value_ptr value = nullptr);
    
//         // qb::value_ptr map(TypeChecker& checker, type_t type, std::pair<std::string, bool> data[]);
//         // qb::value_ptr vec(type_t type, std::map<std::string, value_ptr> value);
//         // qb::value_ptr _struct(type_t type, std::vector<value_ptr> value);

//         template <typename T>
//         qb::value_ptr map(qb::type_t type, std::map<std::string, T> data) {
//             return (qb::value_ptr) new qb::Value<std::map<std::string, T>*>({
//                 .type = type,
//                 .data = new std::map<std::string, T>(data)
//             });
//         }

//         qb::value_ptr _struct(qb::type_t type, std::vector<qb::value_ptr> data);
//     }

// }