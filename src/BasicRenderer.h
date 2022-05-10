#pragma once
#include <stdint.h>
#include "math.h"
#include "Framebuffer.h"
#include "simpleFont.h"

typedef struct {
	Point* CursorPosition;
	FrameBuffer* TargetFramebuffer;
	PSF1_FONT* PSF1_Font;
	unsigned int Color;
} BasicRenderer;

void BasicRendererConstructor(BasicRenderer* basicrenderer, Point* cursor, FrameBuffer* framebuffer, PSF1_FONT* psf1_font);
void PutChar(BasicRenderer* basicrenderer, char chr, unsigned int xOff, unsigned int yOff);
void Print(BasicRenderer* basicrenderer, const char* str);
void Clear(BasicRenderer* basicrenderer, uint32_t color);
void Next(BasicRenderer* basicrenderer);

extern BasicRenderer* GlobalRenderer;
