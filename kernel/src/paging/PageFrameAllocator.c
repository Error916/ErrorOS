#include "PageFrameAllocator.h"

uint64_t freeMemory;
uint64_t reservedMemory;
uint64_t usedMemory;
bool Initialized = false;
PageFrameAllocator GlobalAllocator;

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

	ReservePages(pfa, 0, memorySize / 4096 + 1); // look all memory

	// unreserve the efiConventionalMemory pages
	for(int i = 0; i < mMapEntries; ++i){
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
		if(desc->type == 7){
			UnreservePages(pfa, desc->physAddr, desc->numPages);
		}
	}

	// reserve 0 -> 0x100000 to be sure to not touch bios stuff
	ReservePages(pfa, 0, 0x100);
	// lock bitmap pages
	LockPages(pfa, pfa->PageBitmap->Buffer, pfa->PageBitmap->Size / 4096 + 1);
}

// TODO: write a better function that use a little bit of logic
uint64_t pageBitmapIndex = 0;
void* RequestPage(PageFrameAllocator* pfa){
	for(; pageBitmapIndex < pfa->PageBitmap->Size * 8; ++pageBitmapIndex){
		if(GetBitmap(pfa->PageBitmap, pageBitmapIndex) == true) continue;
		LockPage(pfa, (void*)(pageBitmapIndex * 4096));
		return (void*)(pageBitmapIndex * 4096);
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
	if(SetBitmap(pfa->PageBitmap, index, false)){
		freeMemory += 4096;
		usedMemory -= 4096;
		if(pageBitmapIndex > index) pageBitmapIndex = index;
	}
}

void FreePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount){
	for(int i = 0; i < pageCount; ++i){
		FreePage(pfa, (void*)((uint64_t)address + (i * 4096)));
	}
}

void LockPage(PageFrameAllocator* pfa, void* address){
	uint64_t index = (uint64_t)address / 4096;
	if(GetBitmap(pfa->PageBitmap, index) == true) return;
	if(SetBitmap(pfa->PageBitmap, index, true)){
		freeMemory -= 4096;
		usedMemory += 4096;
	}
}

void LockPages(PageFrameAllocator* pfa, void* address, uint64_t pageCount){
	for(int i = 0; i < pageCount; ++i){
		LockPage(pfa, (void*)((uint64_t)address + (i * 4096)));
	}
}

void InitBitmap(PageFrameAllocator* pfa, size_t bitmapSize, void* bufferAddress){
	pfa->PageBitmap->Size = bitmapSize;
	pfa->PageBitmap->Buffer = (uint8_t*)bufferAddress;
	for(int i = 0; i < bitmapSize; ++i){
		*(uint8_t*)(pfa->PageBitmap->Buffer + i) = 0;
	}
}

void UnreservePage(PageFrameAllocator* pfa, void* address){
	uint64_t index = (uint64_t)address / 4096;
	if(GetBitmap(pfa->PageBitmap, index) == false) return;
	if(SetBitmap(pfa->PageBitmap, index, false)){
		freeMemory += 4096;
		reservedMemory -= 4096;
		if(pageBitmapIndex > index) pageBitmapIndex = index;
	}
}

void UnreservePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount){
	for(int i = 0; i < pageCount; ++i){
		UnreservePage(pfa, (void*)((uint64_t)address + (i * 4096)));
	}
}

void ReservePage(PageFrameAllocator* pfa, void* address){
	uint64_t index = (uint64_t)address / 4096;
	if(GetBitmap(pfa->PageBitmap, index) == true) return;
	if(SetBitmap(pfa->PageBitmap, index, true)){
		freeMemory -= 4096;
		reservedMemory += 4096;
	}
}

void ReservePages(PageFrameAllocator* pfa, void* address, uint64_t pageCount){
	for(int i = 0; i < pageCount; ++i){
		ReservePage(pfa, (void*)((uint64_t)address + (i * 4096)));
	}
}
