#include "BasicRenderer.h"

BasicRenderer* GlobalRenderer;

//TODO: use malloc to remove the references to BasicRenderer and Cursor
void BasicRendererConstructor(BasicRenderer* basicrenderer, Point* cursor, FrameBuffer* framebuffer, PSF1_FONT* psf1_font){
	basicrenderer->CursorPosition = cursor;
	basicrenderer->TargetFramebuffer = framebuffer;
	basicrenderer->PSF1_Font = psf1_font;
	basicrenderer->Color = 0xffffffff;
}

void PutChar(BasicRenderer* basicrenderer, char chr, unsigned int xOff, unsigned int yOff){
	unsigned int* pixPtr = (unsigned int*) basicrenderer->TargetFramebuffer->BaseAddress;
	char* fontPtr = (char*)basicrenderer->PSF1_Font->glyphBuffer + (chr * basicrenderer->PSF1_Font->psf1_Header->charsize);
	for(unsigned long y = yOff; y < yOff + 16; ++y){
		for(unsigned long x = xOff; x < xOff + 8; ++x){
			if((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
				*(unsigned int*)(pixPtr + x + (y * basicrenderer->TargetFramebuffer->PixelsPerScanLine)) = basicrenderer->Color;
			}
		}
		fontPtr++;
	}
}

void Print(BasicRenderer* basicrenderer, const char* str){
	char* chr = (char*)str;
	while(*chr != 0){
		PutChar(basicrenderer, *chr, basicrenderer->CursorPosition->X, basicrenderer->CursorPosition->Y);
		basicrenderer->CursorPosition->X += 8;
		if(basicrenderer->CursorPosition->X + 8 > basicrenderer->TargetFramebuffer->Width){
			basicrenderer->CursorPosition->X = 0;
			basicrenderer->CursorPosition->Y += 16;
		}
		chr++;
	}
}

void Clear(BasicRenderer* basicrenderer, uint32_t color){
	uint64_t fbBase = (uint64_t)basicrenderer->TargetFramebuffer->BaseAddress;
	uint64_t bytesPerScanline = basicrenderer->TargetFramebuffer->PixelsPerScanLine * 4;
	uint64_t fbHeight = basicrenderer->TargetFramebuffer->Height;
	uint64_t fbSize = basicrenderer->TargetFramebuffer->BufferSize;

	for(int verticalScanLine = 0; verticalScanLine < fbHeight; ++verticalScanLine){
		uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanLine);
		for(uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); ++pixPtr){
			*pixPtr = color;
		}
	}
}

void Next(BasicRenderer* basicrenderer){
	basicrenderer->CursorPosition->X = 0;
	basicrenderer->CursorPosition->Y += 16; // Heigth of font
}
