#pragma once
#include <stdint.h>

const char* uto_string(uint64_t value);
const char* ito_string(int64_t value);
const char* u64to_hstring(uint64_t value);
const char* u32to_hstring(uint32_t value);
const char* u16to_hstring(uint16_t value);
const char* u8to_hstring(uint8_t value);
const char* dpto_string(double value, uint8_t decimalPlaces);
const char* dto_string(double value);
