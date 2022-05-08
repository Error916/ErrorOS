#pragma once
#include <stdint.h>
#include <stddef.h>
#include "efiMemory.h"
#include "Bitmap.h"
#include "memory.h"

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

static void InitBitmap(PageFrameAllocator* pfa, size_t bitmapSize, void* bufferAddress);
static void ReservePage(PageFrameAllocator* pfa, void* address);
static void ReservePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount);
static void UnreservePage(PageFrameAllocator* pfa, void* address);
static void UnreservePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount);
