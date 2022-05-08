#pragma once
#include "paging.h"

typedef struct {
	PageTable* PML4;
} PageTableManager;

void PageTableManagerConst(PageTableManager* ptm, PageTable* PML4Address);
void MapMemory(PageTableManager* ptm, void* virtualMemory, void* physicalMemory);
