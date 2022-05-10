#include "IDT.h"

void SetOffsetIDT(IDTDescEntry* idtde, uint64_t offset){
	idtde->offset0 = (uint16_t)(offset & 0x000000000000ffff);
	idtde->offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
	idtde->offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
}

uint64_t GetOffsetIDT(IDTDescEntry* idtde){
	uint64_t offset = 0;
	offset |= (uint64_t)idtde->offset0;
	offset |= (uint64_t)idtde->offset1 << 16;
	offset |= (uint64_t)idtde->offset2 << 32;

	return offset;
}
