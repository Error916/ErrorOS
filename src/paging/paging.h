#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	bool Present : 1;
	bool ReadWrite : 1;
	bool UserSuper : 1;
	bool WriteThrough : 1;
	bool CacheDisabled : 1;
	bool Accessed : 1;
	bool ignore0 : 1;
	bool LargerPages : 1;
	bool ingore1 : 1;
	uint8_t Available : 3;
	uint64_t Address : 52;
} PageDirectoryEntry;

typedef struct {
	PageDirectoryEntry entries [512];
} PageTable __attribute__((aligned(0x1000)));
