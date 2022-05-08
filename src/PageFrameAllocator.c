#include "PageFrameAllocator.h"

uint64_t freeMemory;
uint64_t reservedMemory;
uint64_t usedMemory;
bool Initialized = false;

void ReadEFIMemoryMap(PageFrameAllocator* pfa, EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize){
	if(Initialized) return;

	Initialized = true;

	uint64_t mMapEntries = mMapSize / mMapDescSize;

	void* largestFreeMemSeg = NULL;
	size_t largestFreeMemSize = 0;

	for(int i = 0; i < mMapEntries; ++i){
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
		if(desc->type == 7){ // type == EfiConventionalMemory
			if(desc->numPages * 4096 > largestFreeMemSize){
				largestFreeMemSeg = desc->physAddr;
				largestFreeMemSize = desc->numPages * 4096;
			}
		}
	}

	uint64_t memorySize = GetMemorySize(mMap, mMapEntries, mMapDescSize);
	freeMemory = memorySize;
	uint64_t bitmapSize = memorySize / 4096 / 8 + 1;

	InitBitmap(pfa, bitmapSize, largestFreeMemSeg);

	// lock bitmap pages
	LockPages(pfa, pfa->PageBitmap, pfa->PageBitmap->Size / 4096 + 1);

	// reserve the reserved/unusable pages
	for(int i = 0; i < mMapEntries; ++i){
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
		if(desc->type != 7){
			ReservePages(pfa, desc->physAddr, desc->numPages);
		}
	}

}

// TODO: write a better function that use a little bit of logic
void* RequestPage(PageFrameAllocator* pfa){
	for(uint64_t index = 0; index < pfa->PageBitmap->Size * 8; ++index){
		if(GetBitmap(pfa->PageBitmap, index) == true) continue;
		LockPage(pfa, (void*)(index * 4096));
		return (void*)(index * 4096);
	}

	// TODO: Implement Page Frame Swap to file
	return NULL;
}

uint64_t GetFreeRAM(){ return freeMemory; }
uint64_t GetUsedRAM(){ return usedMemory; }
uint64_t GetReservedRAM(){ return reservedMemory; }

void FreePage(PageFrameAllocator* pfa, void* address){
	uint64_t index = (uint64_t)address / 4096;
	if(GetBitmap(pfa->PageBitmap, index) == false) return;
	SetBitmap(pfa->PageBitmap, index, false);
	freeMemory += 4096;
	usedMemory -= 4096;
}

void FreePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount){
	for(int i = 0; i < pageCount; ++i){
		FreePage(pfa, (void*)((uint64_t)address + (i * 4096)));
	}
}

void LockPage(PageFrameAllocator* pfa, void* address){
	uint64_t index = (uint64_t)address / 4096;
	if(GetBitmap(pfa->PageBitmap, index) == true) return;
	SetBitmap(pfa->PageBitmap, index, true);
	freeMemory -= 4096;
	usedMemory += 4096;
}

void LockPages(PageFrameAllocator* pfa, void* address, uint64_t pageCount){
	for(int i = 0; i < pageCount; ++i){
		LockPage(pfa, (void*)((uint64_t)address + (i * 4096)));
	}
}

static void InitBitmap(PageFrameAllocator* pfa, size_t bitmapSize, void* bufferAddress){
	pfa->PageBitmap->Size = bitmapSize;
	pfa->PageBitmap->Buffer = (uint8_t*)bufferAddress;
	for(int i = 0; i < bitmapSize; ++i){
		*(uint8_t*)(pfa->PageBitmap->Buffer + i) = 0;
	}
}

static void UnreservePage(PageFrameAllocator* pfa, void* address){
	uint64_t index = (uint64_t)address / 4096;
	if(GetBitmap(pfa->PageBitmap, index) == false) return;
	SetBitmap(pfa->PageBitmap, index, false);
	freeMemory += 4096;
	reservedMemory -= 4096;
}

static void UnreservePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount){
	for(int i = 0; i < pageCount; ++i){
		UnreservePage(pfa, (void*)((uint64_t)address + (i * 4096)));
	}
}

static void ReservePage(PageFrameAllocator* pfa, void* address){
	uint64_t index = (uint64_t)address / 4096;
	if(GetBitmap(pfa->PageBitmap, index) == true) return;
	SetBitmap(pfa->PageBitmap, index, true);
	freeMemory -= 4096;
	reservedMemory += 4096;
}

static void ReservePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount){
	for(int i = 0; i < pageCount; ++i){
		ReservePage(pfa, (void*)((uint64_t)address + (i * 4096)));
	}
}
