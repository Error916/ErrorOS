#include "BasicRenderer.h"
#include "cstr.h"

void _start(FrameBuffer* framebuffer, PSF1_FONT* psf1_font){

	Point CursorPosition = {0, 0};
	BasicRenderer newRenderer;
	BasicRendererConstructor(&newRenderer, &CursorPosition, framebuffer, psf1_font);

	Print(&newRenderer, "Hello From Kernel");
	newRenderer.CursorPosition->X = 0;
	newRenderer.CursorPosition->Y = 16;
	Print(&newRenderer, ito_string(-1234));
	newRenderer.CursorPosition->X = 0;
	newRenderer.CursorPosition->Y = 32;
	Print(&newRenderer, uto_string(1234));
	newRenderer.CursorPosition->X = 0;
	newRenderer.CursorPosition->Y = 48;
	Print(&newRenderer, dto_string(-3.14));
	newRenderer.CursorPosition->X = 0;
	newRenderer.CursorPosition->Y = 64;

	return ;
}
