#pragma once
#include <bit>
#include <stdint.h>

#ifndef __STDC_IEC_559__
#error The target (platform + compiler) MUST use the IEEE 754 Standard for floating points.
#endif

inline uint32_t float_to_u32(float value) {
    return std::bit_cast<uint32_t>(value);
}

inline float u32_to_float(uint32_t bits){
    return std::bit_cast<float>(bits);
}

// Parsers

inline uint16_t parse_u16(uint8_t* bytes) {
    return (uint16_t) ((bytes[0] << 8) | bytes[1]);
}

inline uint32_t parse_u32(uint8_t* bytes) {
    return (uint32_t) ((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3]);
}

inline float parse_f32(uint8_t* bytes) {
    return u32_to_float((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3]);
}