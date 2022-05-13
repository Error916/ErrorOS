#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	size_t Size;
	uint8_t* Buffer;
} Bitmap;

bool GetBitmap(Bitmap* bitmap, uint64_t index);
bool SetBitmap(Bitmap* bitmap, uint64_t index, bool value);
