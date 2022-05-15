#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"

typedef struct heapseghdr {
	size_t length;
	struct heapseghdr* next;
	struct heapseghdr* last;
	bool free;
} HeapSegHdr;

void CombineForward(HeapSegHdr* heapSegHdr);
void CombineBackward(HeapSegHdr* heapSegHdr);
HeapSegHdr* Split(HeapSegHdr* heapSegHdr, size_t splitLength);

void InitializeHeap(void* heapAddress, size_t pageCount);

void* malloc(size_t size);
void free(void* address);

void ExpandHeap(size_t length);
