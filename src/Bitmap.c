#include "Bitmap.h"

bool GetBitmap(Bitmap* bitmap, uint64_t index){
	if(index > bitmap->Size * 8) return false;
	uint64_t byteIndex = index / 8;
	uint8_t bitIndex = index % 8;
	uint8_t bitIndexer = 0b10000000 >> bitIndex;

	if((bitmap->Buffer[byteIndex] & bitIndexer) > 0)
		return true;

	return false;
}

bool SetBitmap(Bitmap* bitmap, uint64_t index, bool value){
	if(index > bitmap->Size * 8) return false;
	uint64_t byteIndex = index / 8;
	uint8_t bitIndex = index % 8;
	uint8_t bitIndexer = 0b10000000 >> bitIndex;
	bitmap->Buffer[byteIndex] &= ~bitIndexer;
	if (value){
		bitmap->Buffer[byteIndex] |= bitIndexer;
	}

	return true;
}
