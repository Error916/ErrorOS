#include "PageMapIndexer.h"

void PageMapIndexerConst(PageMapIndexer* pmi, uint64_t virtualAddress){
	virtualAddress >>= 12;
	pmi->P_i = virtualAddress & 0x1ff;
	virtualAddress >>= 9;
	pmi->PT_i = virtualAddress & 0x1ff;
	virtualAddress >>= 9;
	pmi->PD_i = virtualAddress & 0x1ff;
	virtualAddress >>= 9;
	pmi->PDP_i = virtualAddress & 0x1ff;

}
