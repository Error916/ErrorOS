#include <stdint.h>
#include "BasicRenderer.h"
#include "cstr.h"
#include "efiMemory.h"
#include "memory.h"
#include "Bitmap.h"
#include "PageFrameAllocator.h"

typedef struct {
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
} BootInfo;

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void _start(BootInfo* bootinfo){

	Point CursorPosition = {0, 0};
	BasicRenderer newRenderer;
	BasicRendererConstructor(&newRenderer, &CursorPosition, bootinfo->framebuffer, bootinfo->psf1_font);

	uint64_t mMapEntries = bootinfo->mMapSize / bootinfo->mMapDescSize;

	Bitmap bitmap = {0, NULL};
	PageFrameAllocator newAllocator;
	newAllocator.PageBitmap = &bitmap;
	ReadEFIMemoryMap(&newAllocator, bootinfo->mMap, bootinfo->mMapSize, bootinfo->mMapDescSize);

	uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
	uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
	LockPages(&newAllocator, &_KernelStart, kernelPages);

	Print(&newRenderer, "Free RAM: ");
	Print(&newRenderer, uto_string(GetFreeRAM() / 1024));
	Print(&newRenderer, " KB");
	newRenderer.CursorPosition->X = 0;
	newRenderer.CursorPosition->Y += 16;

	Print(&newRenderer, "Lock RAM: ");
	Print(&newRenderer, uto_string(GetUsedRAM() / 1024));
	Print(&newRenderer, " KB");
	newRenderer.CursorPosition->X = 0;
	newRenderer.CursorPosition->Y += 16;

	Print(&newRenderer, "Reserved RAM: ");
	Print(&newRenderer, uto_string(GetReservedRAM() / 1024));
	Print(&newRenderer, " KB");
	newRenderer.CursorPosition->X = 0;
	newRenderer.CursorPosition->Y += 16;

	for(int t = 0; t < 20; ++t){
		void* address = RequestPage(&newAllocator);
		Print(&newRenderer, u64to_hstring((uint64_t)address));
		newRenderer.CursorPosition->X = 0;
		newRenderer.CursorPosition->Y += 16;
	}

	/* Print(&newRenderer, uto_string(GetMemorySize(bootinfo->mMap, mMapEntries, bootinfo->mMapDescSize))); */

	/* for(int i = 0; i < mMapEntries; ++i){ */
	/* 	EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)bootinfo->mMap + (i * bootinfo->mMapDescSize)); */
	/* 	newRenderer.CursorPosition->X = 0; */
	/* 	newRenderer.CursorPosition->Y += 16; */
	/* 	Print(&newRenderer, EFI_MEMORY_TYPE_STRINGS[desc->type]); */
	/* 	newRenderer.Color = 0xffff00ff; */
	/* 	Print(&newRenderer, " "); */
	/* 	Print(&newRenderer, uto_string(desc->numPages * 4096 / 1024)); */
	/* 	Print(&newRenderer, " KB"); */
	/* 	newRenderer.Color = 0xffffffff; */
	/* } */



	return ;
}
