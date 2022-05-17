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
	/* uint64_t fbSize = basicrenderer->TargetFramebuffer->BufferSize; */

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
	basicrenderer->CursorPosition->Y += 16; // Height of font
}

void inline PutPix(BasicRenderer* basicrenderer, uint32_t x, uint32_t y, uint32_t color){
	*(uint32_t*)((uint64_t)basicrenderer->TargetFramebuffer->BaseAddress + (x * 4) + (y * basicrenderer->TargetFramebuffer->PixelsPerScanLine * 4)) = color;
}

uint32_t inline GetPix(BasicRenderer* basicrenderer, uint32_t x, uint32_t y){
	return *(uint32_t*)((uint64_t)basicrenderer->TargetFramebuffer->BaseAddress + (x * 4) + (y * basicrenderer->TargetFramebuffer->PixelsPerScanLine * 4));
}

void ClearMouseCursor(BasicRenderer* basicrenderer, uint8_t* mouseCursor, Point* position){
	if(!basicrenderer->MouseDrawn) return;

	int xMax = 16;
	int yMax = 16;
	int diffX = basicrenderer->TargetFramebuffer->Width - position->X;
	int diffY = basicrenderer->TargetFramebuffer->Height - position->Y;

	if(diffX < 16) xMax = diffX;
	if(diffY < 16) yMax = diffY;

	for(int y = 0; y < yMax; ++y){
		for(int x = 0; x < xMax; ++x){
			int bit = y * 16 + x;
			int byte = bit / 8;
			if(mouseCursor[byte] & (0b10000000 >> (x % 8))){
				if(GetPix(basicrenderer, position->X + x, position->Y + y) == basicrenderer->MouseCursorBufferAfter[x + y * 16])
					PutPix(basicrenderer, position->X + x, position->Y + y, basicrenderer->MouseCursorBuffer[x + y * 16]);
			}
		}
	}
}

void DrawOverlayMouseCursor(BasicRenderer* basicrenderer, uint8_t* mouseCursor, Point* position, uint32_t color){
	// NB: we know the cursor is 16 x 16
	int xMax = 16;
	int yMax = 16;
	int diffX = basicrenderer->TargetFramebuffer->Width - position->X;
	int diffY = basicrenderer->TargetFramebuffer->Height - position->Y;

	if(diffX < 16) xMax = diffX;
	if(diffY < 16) yMax = diffY;

	for(int y = 0; y < yMax; ++y){
		for(int x = 0; x < xMax; ++x){
			int bit = y * 16 + x;
			int byte = bit / 8;
			if(mouseCursor[byte] & (0b10000000 >> (x % 8))){
				basicrenderer->MouseCursorBuffer[x + y * 16] = GetPix(basicrenderer, position->X + x, position->Y + y);
				PutPix(basicrenderer, position->X + x, position->Y + y, color);
				basicrenderer->MouseCursorBufferAfter[x + y * 16] = GetPix(basicrenderer, position->X + x, position->Y + y);
			}
		}
	}

	basicrenderer->MouseDrawn = true;
}
