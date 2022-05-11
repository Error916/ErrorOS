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

void PutCharS(BasicRenderer* basicrenderer, char chr){
	PutChar(basicrenderer, chr, basicrenderer->CursorPosition->X, basicrenderer->CursorPosition->Y);
	basicrenderer->CursorPosition->X += 8;
	if(basicrenderer->CursorPosition->X + 8 > basicrenderer->TargetFramebuffer->Width){
		basicrenderer->CursorPosition->X = 0;
		basicrenderer->CursorPosition->Y += 16;
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

void Clear(BasicRenderer* basicrenderer){
	uint64_t fbBase = (uint64_t)basicrenderer->TargetFramebuffer->BaseAddress;
	uint64_t bytesPerScanline = basicrenderer->TargetFramebuffer->PixelsPerScanLine * 4;
	uint64_t fbHeight = basicrenderer->TargetFramebuffer->Height;
	uint64_t fbSize = basicrenderer->TargetFramebuffer->BufferSize;

	for(int verticalScanLine = 0; verticalScanLine < fbHeight; ++verticalScanLine){
		uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanLine);
		for(uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); ++pixPtr){
			*pixPtr = basicrenderer->ClearColor;
		}
	}
}

void ClearChar(BasicRenderer* basicrenderer){
	if(basicrenderer->CursorPosition->X == 0){
		basicrenderer->CursorPosition->X = basicrenderer->TargetFramebuffer->Width;
		basicrenderer->CursorPosition->Y -= 16;
		if(basicrenderer->CursorPosition->Y < 0) basicrenderer->CursorPosition->Y = 0;
	}

	unsigned int xOff = basicrenderer->CursorPosition->X;
	unsigned int yOff = basicrenderer->CursorPosition->Y;

	unsigned int* pixPtr = (unsigned int*) basicrenderer->TargetFramebuffer->BaseAddress;
	for(unsigned long y = yOff; y < yOff + 16; ++y){
		for(unsigned long x = xOff - 8; x < xOff; ++x){
			*(unsigned int*)(pixPtr + x + (y * basicrenderer->TargetFramebuffer->PixelsPerScanLine)) = basicrenderer->ClearColor;
		}
	}

	basicrenderer->CursorPosition->X -= 8;
	if(basicrenderer->CursorPosition->X < 0){
		basicrenderer->CursorPosition->X = basicrenderer->TargetFramebuffer->Width;
		basicrenderer->CursorPosition->Y -= 16;
		if(basicrenderer->CursorPosition->Y < 0) basicrenderer->CursorPosition->Y = 0;
	}

}

void Next(BasicRenderer* basicrenderer){
	basicrenderer->CursorPosition->X = 0;
	basicrenderer->CursorPosition->Y += 16; // Heigth of font
}
