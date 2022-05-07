typedef unsigned long long size_t;

typedef struct {
	void* BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
} FrameBuffer;

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
} PSF1_FONT;

typedef struct {
	unsigned int X;
	unsigned int Y;
} Point;

void putChar(FrameBuffer* framebuffer, PSF1_FONT* psf1_font, unsigned int color, char chr, unsigned int xOff, unsigned int yOff){
	unsigned int* pixPtr = (unsigned int*) framebuffer->BaseAddress;
	char* fontPtr = psf1_font->glyphBuffer + (chr * psf1_font->psf1_Header->charsize);
	for(unsigned long y = yOff; y < yOff + 16; ++y){
		for(unsigned long x = xOff; x < xOff + 8; ++x){
			if((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
				*(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = color;
			}
		}
		fontPtr++;
	}
}

Point CursorPosition;
void Print(FrameBuffer* framebuffer, PSF1_FONT* psf1_font, unsigned int color, char* str){
	unsigned int x = 0;
	char* chr = str;
	while(*chr != 0){
		putChar(framebuffer, psf1_font, color, *chr, CursorPosition.X, CursorPosition.Y);
		CursorPosition.X += 8;
		if(CursorPosition.X + 8 > framebuffer->Width){
			CursorPosition.X = 0;
			CursorPosition.Y += 16;
		}
		chr++;
	}
}

void _start(FrameBuffer* framebuffer, PSF1_FONT* psf1_font){
	CursorPosition.X = 0;
	CursorPosition.Y = 0;

	Print(framebuffer, psf1_font, 0xffffffff, "Hello From Kernel");

	return ;
}
