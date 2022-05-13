#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "math.h"
#include "Framebuffer.h"
#include "simpleFont.h"

typedef struct {
	Point* CursorPosition;
	FrameBuffer* TargetFramebuffer;
	PSF1_FONT* PSF1_Font;
	uint32_t MouseCursorBuffer[16 * 16];
	uint32_t MouseCursorBufferAfter[16 * 16];
	unsigned int Color;
	unsigned int ClearColor;
	bool MouseDrawn;
} BasicRenderer;

void BasicRendererConstructor(BasicRenderer* basicrenderer, Point* cursor, FrameBuffer* framebuffer, PSF1_FONT* psf1_font);
void PutChar(BasicRenderer* basicrenderer, char chr, unsigned int xOff, unsigned int yOff);
void PutCharS(BasicRenderer* basicrenderer, char chr); // S = single char
void Print(BasicRenderer* basicrenderer, const char* str);
void Clear(BasicRenderer* basicrenderer);
void ClearChar(BasicRenderer* basicrenderer);
void Next(BasicRenderer* basicrenderer);
void PutPix(BasicRenderer* basicrenderer, uint32_t x, uint32_t y, uint32_t color);
uint32_t GetPix(BasicRenderer* basicrenderer, uint32_t x, uint32_t y);
void ClearMouseCursor(BasicRenderer* basicrenderer, uint8_t* mouseCursor, Point* position);
void DrawOverlayMouseCursor(BasicRenderer* basicrenderer, uint8_t* mouseCursor, Point* position, uint32_t color);

extern BasicRenderer* GlobalRenderer;
