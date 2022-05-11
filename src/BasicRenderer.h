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
	unsigned int ClearColor;
} BasicRenderer;

void BasicRendererConstructor(BasicRenderer* basicrenderer, Point* cursor, FrameBuffer* framebuffer, PSF1_FONT* psf1_font);
void PutChar(BasicRenderer* basicrenderer, char chr, unsigned int xOff, unsigned int yOff);
void PutCharS(BasicRenderer* basicrenderer, char chr); // S = single char
void Print(BasicRenderer* basicrenderer, const char* str);
void Clear(BasicRenderer* basicrenderer);
void ClearChar(BasicRenderer* basicrenderer);
void Next(BasicRenderer* basicrenderer);

extern BasicRenderer* GlobalRenderer;
