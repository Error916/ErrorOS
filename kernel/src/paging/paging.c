#include "paging.h"

void SetFlagPDE(PageDirectoryEntry* pde, PT_Flag flag, bool enabled){
	uint64_t  bitSelector = (uint64_t)1 << flag;
	pde->Value &= ~bitSelector;
	if(enabled){
		pde->Value |= bitSelector;
	}
}

bool GetFlagPDE(PageDirectoryEntry* pde, PT_Flag flag){
	uint64_t  bitSelector = (uint64_t)1 << flag;
	return ((pde->Value & bitSelector) > 0) ? true : false;
}

void SetAddressPDE(PageDirectoryEntry* pde, uint64_t address){
	address &= 0x000000ffffffffff;
	pde->Value &= 0xfff0000000000fff;
	pde->Value |= (address << 12);
}

uint64_t GetAddressPDE(PageDirectoryEntry* pde){
	return (pde->Value & 0x000ffffffffff000) >> 12;
}
