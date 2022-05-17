#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../efiMemory.h"
#include "../Bitmap.h"
#include "../memory.h"

typedef struct {
	Bitmap* PageBitmap;
} PageFrameAllocator;

void ReadEFIMemoryMap(PageFrameAllocator* pfa, EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize);
void* RequestPage(PageFrameAllocator* pfa);

uint64_t GetFreeRAM();
uint64_t GetUsedRAM();
uint64_t GetReservedRAM();

void FreePage(PageFrameAllocator* pfa, void* address);
void FreePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount);
void LockPage(PageFrameAllocator* pfa, void* address);
void LockPages(PageFrameAllocator* pfa, void* address, uint64_t pageCount);

void InitBitmap(PageFrameAllocator* pfa, size_t bitmapSize, void* bufferAddress);
void ReservePage(PageFrameAllocator* pfa, void* address);
void ReservePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount);
void UnreservePage(PageFrameAllocator* pfa, void* address);
void UnreservePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount);

extern PageFrameAllocator GlobalAllocator;
