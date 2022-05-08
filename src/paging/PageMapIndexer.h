#pragma once
#include <stdint.h>

typedef struct {
	uint64_t PDP_i;
	uint64_t PD_i;
	uint64_t PT_i;
	uint64_t P_i;
} PageMapIndexer;

void PageMapIndexerConst(PageMapIndexer* pmi, uint64_t virtualAddress);
