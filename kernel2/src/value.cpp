// #include "value.h"

// qb::value_ptr qb::value::_void(){
//     return (qb::Value<void*>*) new qb::Value<qb::void_t>({
//         .type = B_TYPE_VOID,
//         .data = 0
//     });
// };

// qb::value_ptr qb::value::null(){
//     return (qb::Value<void*>*) new qb::Value<qb::void_t>({
//         .type = B_TYPE_NULL,
//         .data = 0
//     });
// };

// qb::value_ptr qb::value::_bool(bool value){
//     return (qb::Value<void*>*) new qb::Value<bool>({
//         .type = B_TYPE_BOOL,
//         .data = value
//     });
// };

// qb::value_ptr qb::value::u8(uint8_t value){
//     return (qb::Value<void*>*) new qb::Value<uint8_t>({
//         .type = B_TYPE_U8,
//         .data = value
//     });
// };

// qb::value_ptr qb::value::i8(int8_t value){
//     return (qb::Value<void*>*) new qb::Value<int8_t>({
//         .type = B_TYPE_I8,
//         .data = value
//     });
// };

// qb::value_ptr qb::value::u16(uint16_t value){
//     return (qb::Value<void*>*) new qb::Value<uint16_t>({
//         .type = B_TYPE_U16,
//         .data = value
//     });
// };

// qb::value_ptr qb::value::i16(int16_t value){
//     return (qb::Value<void*>*) new qb::Value<int16_t>({
//         .type = B_TYPE_I16,
//         .data = value
//     });
// };

// qb::value_ptr qb::value::u32(uint32_t value){
//     return (qb::Value<void*>*) new qb::Value<uint32_t>({
//         .type = B_TYPE_U32,
//         .data = value
//     });
// };

// qb::value_ptr qb::value::i32(int32_t value){
//     return (qb::Value<void*>*) new qb::Value<int32_t>({
//         .type = B_TYPE_I32,
//         .data = value
//     });
// };

// qb::value_ptr qb::value::f32(float value){
//     return (qb::Value<void*>*) new qb::Value<float>({
//         .type = B_TYPE_F32,
//         .data = value
//     });
// };

// qb::value_ptr qb::value::str(std::string value){
//     return (qb::Value<void*>*) new qb::Value<std::string>({
//         .type = B_TYPE_STR,
//         .data = value
//     });
// };

// qb::value_ptr qb::value::ref(qb::value_ptr value){
//     return (qb::Value<void*>*) new qb::Value<qb::value_ptr>({
//         .type = B_TYPE_REF,
//         .data = value
//     });
// };


// //

// qb::value_ptr qb::value::_struct(qb::type_t type, std::vector<qb::value_ptr> data) {
//     return (qb::value_ptr) new qb::Value<std::vector<qb::value_ptr>>({
//         .type = type,
//         .data = data
//     });
// }