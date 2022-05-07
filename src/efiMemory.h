#pragma once
#include <stdint.h>

typedef struct {
	uint32_t type;
	void* phyAddr;
	void* virtAddr;
	uint64_t numPages;
	uint64_t attribs;
} EFI_MEMORY_DESCRIPTOR;

extern const char* EFI_MEMORY_TYPE_STRINGS[];
