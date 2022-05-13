#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
	Present = 0,
	ReadWrite = 1,
	UserSuper = 2,
	WriteThrough = 3,
	CacheDisabled = 4,
	Accessed = 5,
	LargerPages = 7,
	Custom0 = 9,
	Custom1 = 10,
	Custom2 = 11,
	NX = 63 //only if supported
} PT_Flag;

typedef struct {
	uint64_t Value;
} PageDirectoryEntry;

typedef struct {
	PageDirectoryEntry entries [512];
} PageTable __attribute__((aligned(0x1000)));

void SetFlagPDE(PageDirectoryEntry* pde, PT_Flag flag, bool enabled);
bool GetFlagPDE(PageDirectoryEntry* pde, PT_Flag flag);
void SetAddressPDE(PageDirectoryEntry* pde, uint64_t address);
uint64_t GetAddressPDE(PageDirectoryEntry* pde);
