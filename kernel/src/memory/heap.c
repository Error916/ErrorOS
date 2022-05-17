#include "heap.h"

void* heapStart;
void* heapEnd;
HeapSegHdr* LastHdr;
void InitializeHeap(void* heapAddress, size_t pageCount){
	void* pos = heapAddress;

	for(size_t i = 0; i < pageCount; ++i){
		MapMemory(&GlobalPTM, pos, RequestPage(&GlobalAllocator));
		pos = (void*)((size_t)pos + 0x1000);
	}

	size_t heapLength = pageCount * 0x1000;

	heapStart = heapAddress;
	heapEnd = (void*)((size_t)heapStart + heapLength);
	HeapSegHdr* startSeg = (HeapSegHdr*)heapAddress;
	startSeg->length = heapLength - sizeof(HeapSegHdr);
	startSeg->next = NULL;
	startSeg->last = NULL;
	startSeg->free = true;
	LastHdr = startSeg;
}

void free(void* address){
	HeapSegHdr* segment = (HeapSegHdr*)address - 1;
	segment->free = true;
	CombineForward(segment);
	CombineBackward(segment);
}

// TODO: improve speed of malloc
void* malloc(size_t size){
	if(size % 0x10 > 0){ // not a multiple of 0x10
		size -= (size % 0x10);
		size += 0x10;
	}

	if(size == 0) return NULL;

	HeapSegHdr* currentSeg = (HeapSegHdr*) heapStart;
	while(true){
		if(currentSeg->free){
			if(currentSeg->length > size){
				Split(currentSeg, size);
				currentSeg->free = false;
				return (void*)((uint64_t)currentSeg + sizeof(HeapSegHdr));
			}
			if(currentSeg->length == size){
				currentSeg->free = false;
				return (void*)((uint64_t)currentSeg + sizeof(HeapSegHdr));
			}
		}
		if(currentSeg->next == NULL) break;
		currentSeg = currentSeg->next;
	}

	ExpandHeap(size);
	return malloc(size);
}

HeapSegHdr* Split(HeapSegHdr* heapSegHdr, size_t splitLength){
	if(splitLength  < 0x10) return NULL;
	int64_t splitSegLength = heapSegHdr->length - splitLength - sizeof(HeapSegHdr);
	if(splitSegLength < 0x10) return NULL;

	HeapSegHdr* newSplitHdr = (HeapSegHdr*)((size_t)heapSegHdr + splitLength + sizeof(HeapSegHdr));
	heapSegHdr->next->last = newSplitHdr;
	newSplitHdr->next = heapSegHdr->next;
	heapSegHdr->next = newSplitHdr;
	newSplitHdr->last = heapSegHdr;
	newSplitHdr->length = splitSegLength;
	newSplitHdr->free = heapSegHdr->free;
	heapSegHdr->length = splitLength;

	if(LastHdr == heapSegHdr) LastHdr = newSplitHdr;
	return newSplitHdr;
}

void ExpandHeap(size_t length){
	if(length % 0x1000){
		length -= length % 0x1000;
		length += 0x1000;
	}

	size_t pageCount = length / 0x1000;
	HeapSegHdr* newSegHdr = (HeapSegHdr*)heapEnd;

	for(size_t i = 0; i < pageCount; ++i){
		MapMemory(&GlobalPTM, heapEnd, RequestPage(&GlobalAllocator));
		heapEnd = (void*)((size_t)heapEnd + 0x1000);
	}

	newSegHdr->free = true;
	newSegHdr->last = LastHdr;
	LastHdr->next = newSegHdr;
	LastHdr = newSegHdr;
	newSegHdr->next = NULL;
	newSegHdr->length = length - sizeof(HeapSegHdr);
	CombineBackward(newSegHdr);
}

void CombineForward(HeapSegHdr* heapSegHdr){
	if(heapSegHdr->next == NULL) return;
	if(!heapSegHdr->next->free) return;

	if(heapSegHdr->next == LastHdr) LastHdr = heapSegHdr;

	if(heapSegHdr->next->next != NULL){
		heapSegHdr->next->next->last = heapSegHdr;
	}

	heapSegHdr->next = heapSegHdr->next->next;
	heapSegHdr->length = heapSegHdr->length + heapSegHdr->next->length + sizeof(HeapSegHdr);
}

void CombineBackward(HeapSegHdr* heapSegHdr){
	if(heapSegHdr->last != NULL && heapSegHdr->last->free) CombineForward(heapSegHdr->last);
}
