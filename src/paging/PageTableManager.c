#include "PageTableManager.h"
#include "PageMapIndexer.h"
#include "PageFrameAllocator.h"
#include "../memory.h"
#include <stdint.h>

void PageTableManagerConst(PageTableManager* ptm, PageTable* PML4Address){
	ptm->PML4 = PML4Address;
}

void MapMemory(PageTableManager* ptm, void* virtualMemory, void* physicalMemory){
	PageMapIndexer indexer;
	PageMapIndexerConst(&indexer, (uint64_t)virtualMemory);
	PageDirectoryEntry PDE;

	PDE = ptm->PML4->entries[indexer.PDP_i];
	PageTable* PDP;
	if(!PDE.Present){
		PDP = (PageTable*)RequestPage(&GlobalAllocator);
		memset(PDP, 0, 0x1000);
		PDE.Address = (uint64_t)PDP >> 12;
		PDE.Present = true;
		PDE.ReadWrite = true;
		ptm->PML4->entries[indexer.PDP_i] = PDE;
	} else {
		PDP = (PageTable*)((uint64_t)PDE.Address << 12);
	}

	PDE = PDP->entries[indexer.PD_i];
	PageTable* PD;
	if(!PDE.Present){
		PD = (PageTable*)RequestPage(&GlobalAllocator);
		memset(PD, 0, 0x1000);
		PDE.Address = (uint64_t)PD >> 12;
		PDE.Present = true;
		PDE.ReadWrite = true;
		PDP->entries[indexer.PD_i] = PDE;
	} else {
		PD = (PageTable*)((uint64_t)PDE.Address << 12);
	}

	PDE = PD->entries[indexer.PT_i];
	PageTable* PT;
	if(!PDE.Present){
		PT = (PageTable*)RequestPage(&GlobalAllocator);
		memset(PT, 0, 0x1000);
		PDE.Address = (uint64_t)PT >> 12;
		PDE.Present = true;
		PDE.ReadWrite = true;
		PD->entries[indexer.PT_i] = PDE;
	} else {
		PT = (PageTable*)((uint64_t)PDE.Address << 12);
	}

	PDE = PT->entries[indexer.P_i];
	PDE.Address = (uint64_t)physicalMemory >> 12;
	PDE.Present = true;
	PDE.ReadWrite = true;
	PT->entries[indexer.P_i] = PDE;
}
