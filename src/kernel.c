#include <stdint.h>
#include "BasicRenderer.h"
#include "cstr.h"
#include "efiMemory.h"

typedef struct {
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
} BootInfo;

void _start(BootInfo* bootinfo){

	Point CursorPosition = {0, 0};
	BasicRenderer newRenderer;
	BasicRendererConstructor(&newRenderer, &CursorPosition, bootinfo->framebuffer, bootinfo->psf1_font);

	uint64_t mMapEntries = bootinfo->mMapSize / bootinfo->mMapDescSize;
	for(int i = 0; i < mMapEntries; ++i){
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)bootinfo->mMap + (i * bootinfo->mMapDescSize));
		newRenderer.CursorPosition->X = 0;
		newRenderer.CursorPosition->Y += 16;
		Print(&newRenderer, EFI_MEMORY_TYPE_STRINGS[desc->type]);
		newRenderer.Color = 0xffff00ff;
		Print(&newRenderer, " ");
		Print(&newRenderer, uto_string(desc->numPages * 4096 / 1024));
		Print(&newRenderer, " KB");
		newRenderer.Color = 0xffffffff;
	}

	return ;
}
