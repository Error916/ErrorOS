#include "cstr.h"

char uintTo_StringOutput[128];
const char* uto_string(uint64_t value){
	uint8_t size = 0;
	uint64_t sizeTest = value;
	while(sizeTest / 10 > 0){
		sizeTest /= 10;
		size++;
	}

	uint8_t index = 0;
	while(value / 10 > 0){
		uint8_t remainder = value % 10;
		value /= 10;
		uintTo_StringOutput[size - index] = remainder + '0';
		index++;
	}

	uint8_t remainder = value % 10;
	uintTo_StringOutput[size - index] = remainder + '0';
	uintTo_StringOutput[size + 1] = 0;

	return uintTo_StringOutput;
}

char intTo_StringOutput[128];
const char* ito_string(int64_t value){
	uint8_t isNegative = 0;

	if(value < 0){
		isNegative = 1;
		value *= -1;
		intTo_StringOutput[0] = '-';
	}

	uint8_t size = 0;
	uint64_t sizeTest = value;
	while(sizeTest / 10 > 0){
		sizeTest /= 10;
		size++;
	}

	uint8_t index = 0;
	while(value / 10 > 0){
		uint8_t remainder = value % 10;
		value /= 10;
		intTo_StringOutput[isNegative + size - index] = remainder + '0';
		index++;
	}

	uint8_t remainder = value % 10;
	intTo_StringOutput[isNegative + size - index] = remainder + '0';
	intTo_StringOutput[isNegative + size + 1] = 0;

	return intTo_StringOutput;
}

char hexTo_StrintgOutput64[128];
const char* u64to_hstring(uint64_t value){
	uint64_t* valPtr = &value;
	uint8_t* ptr;
	uint8_t tmp;
	uint8_t size = 8 * 2 - 1;
	for(uint8_t i = 0; i < size; i++){
		ptr = ((uint8_t*)valPtr + i);
		tmp = ((*ptr & 0xf0) >> 4);
		hexTo_StrintgOutput64[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
		tmp = ((*ptr & 0x0f));
		hexTo_StrintgOutput64[size - (i * 2 + 0)] = tmp + (tmp > 9 ? 55 : '0');
	}

	hexTo_StrintgOutput64[size + 1] = 0;

	return hexTo_StrintgOutput64;
}

char hexTo_StrintgOutput32[128];
const char* u32to_hstring(uint32_t value){
	uint32_t* valPtr = &value;
	uint8_t* ptr;
	uint8_t tmp;
	uint8_t size = 4 * 2 - 1;
	for(uint8_t i = 0; i < size; i++){
		ptr = ((uint8_t*)valPtr + i);
		tmp = ((*ptr & 0xf0) >> 4);
		hexTo_StrintgOutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
		tmp = ((*ptr & 0x0f));
		hexTo_StrintgOutput32[size - (i * 2 + 0)] = tmp + (tmp > 9 ? 55 : '0');
	}

	hexTo_StrintgOutput32[size + 1] = 0;

	return hexTo_StrintgOutput32;
}

char hexTo_StrintgOutput16[128];
const char* u16to_hstring(uint16_t value){
	uint16_t* valPtr = &value;
	uint8_t* ptr;
	uint8_t tmp;
	uint8_t size = 2 * 2 - 1;
	for(uint8_t i = 0; i < size; i++){
		ptr = ((uint8_t*)valPtr + i);
		tmp = ((*ptr & 0xf0) >> 4);
		hexTo_StrintgOutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
		tmp = ((*ptr & 0x0f));
		hexTo_StrintgOutput16[size - (i * 2 + 0)] = tmp + (tmp > 9 ? 55 : '0');
	}

	hexTo_StrintgOutput16[size + 1] = 0;

	return hexTo_StrintgOutput16;
}

char hexTo_StrintgOutput8[128];
const char* u8to_hstring(uint8_t value){
	uint8_t* valPtr = &value;
	uint8_t* ptr;
	uint8_t tmp;
	uint8_t size = 1 * 2 - 1;
	for(uint8_t i = 0; i < size; i++){
		ptr = ((uint8_t*)valPtr + i);
		tmp = ((*ptr & 0xf0) >> 4);
		hexTo_StrintgOutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
		tmp = ((*ptr & 0x0f));
		hexTo_StrintgOutput8[size - (i * 2 + 0)] = tmp + (tmp > 9 ? 55 : '0');
	}

	hexTo_StrintgOutput8[size + 1] = 0;

	return hexTo_StrintgOutput8;
}

char doubleTo_StringOutput[128];
const char* dpto_string(double value, uint8_t decimalPlaces){
	if(decimalPlaces > 20) decimalPlaces = 20;

	char* intPtr = (char*)ito_string((int64_t)value);
	char* doublePtr = doubleTo_StringOutput;

	if(value < 0 )
		value *= -1;

	while(*intPtr != 0){
		*doublePtr = *intPtr;
		intPtr++;
		doublePtr++;
	}

	*doublePtr = '.';
	doublePtr++;

	double newValue = value - (int)value;

	for(uint8_t i = 0; i < decimalPlaces; i++){
		newValue *= 10;
		*doublePtr = (int)newValue + '0';
		newValue -= (int)newValue;
		doublePtr++;
	}

	*doublePtr = 0;

	return doubleTo_StringOutput;
}

const char* dto_string(double value){
	return dpto_string(value , 2);
}
