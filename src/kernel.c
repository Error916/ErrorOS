#include <stdint.h>
#include "BasicRenderer.h"
#include "cstr.h"

typedef struct {
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_font;
	void* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
} BootInfo;

void _start(BootInfo* bootinfo){

	Point CursorPosition = {0, 0};
	BasicRenderer newRenderer;
	BasicRendererConstructor(&newRenderer, &CursorPosition, bootinfo->framebuffer, bootinfo->psf1_font);

	Print(&newRenderer, "Hello From Kernel");

	return ;
}
