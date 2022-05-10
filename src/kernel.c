#include <stdint.h>
#include "BasicRenderer.h"
#include "cstr.h"
#include "efiMemory.h"
#include "memory.h"
#include "Bitmap.h"
#include "paging/PageFrameAllocator.h"
#include "paging/PageMapIndexer.h"
#include "paging/PageTableManager.h"
#include "paging/paging.h"
#include "gdt/gdt.h"
#include "interrupts/IDT.h"
#include "interrupts/interrupts.h"

typedef struct {
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
} BootInfo;

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void _start(BootInfo* bootinfo){

	/* START Graphycs */
	Point CursorPosition = {0, 0};
	BasicRenderer newRenderer;
	BasicRendererConstructor(&newRenderer, &CursorPosition, bootinfo->framebuffer, bootinfo->psf1_font);
	GlobalRenderer = &newRenderer;
	/* END Graphycs */

	/* START GDT transfer*/
	GDTDescriptor gdtDescriptor;
	gdtDescriptor.Size = sizeof(GDT) - 1;
	gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
	LoadGDT(&gdtDescriptor);
	/* END GDT transfer*/

	/* START prepare Memory */
	uint64_t mMapEntries = bootinfo->mMapSize / bootinfo->mMapDescSize;

	Bitmap bitmap = {0, NULL};
	GlobalAllocator.PageBitmap = &bitmap;
	ReadEFIMemoryMap(&GlobalAllocator, bootinfo->mMap, bootinfo->mMapSize, bootinfo->mMapDescSize);

	uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
	uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
	LockPages(&GlobalAllocator, &_KernelStart, kernelPages);

	PageTable* PML4 = (PageTable*)RequestPage(&GlobalAllocator);
	memset(PML4, 0, 0x1000);
	PageTableManager pageTableManager;
	PageTableManagerConst(&pageTableManager, PML4);

	for(uint64_t t = 0; t < GetMemorySize(bootinfo->mMap, mMapEntries, bootinfo->mMapDescSize); t+=0x1000){
		MapMemory(&pageTableManager, (void*)t, (void*)t);
	}

	uint64_t fbBase = (uint64_t)bootinfo->framebuffer->BaseAddress;
	uint64_t fbSize = (uint64_t)bootinfo->framebuffer->BufferSize + 0x1000; // overshoot for securrity
	LockPages(&GlobalAllocator, (void*)fbBase, fbSize / 0x1000 + 1);
	for(uint64_t t = fbBase; t < fbBase + fbSize; t+=4096){
		MapMemory(&pageTableManager, (void*)t, (void*)t);
	}

	asm ("mov %0, %%cr3" : : "r" (PML4));
	/* END prepare Memory */

	memset(bootinfo->framebuffer->BaseAddress, 0, bootinfo->framebuffer->BufferSize); // Clean the screen to black

	/* START interrupts*/
	IDTR idtr;
	idtr.Limit =  0x0fff;
	idtr.Offset = (uint64_t)RequestPage(&GlobalAllocator);

	IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + 0xE * sizeof(IDTDescEntry));
	SetOffsetIDT(int_PageFault, (uint64_t)PageFault_Handler);
	int_PageFault->type_attr = IDT_TA_InterruptGate;
	int_PageFault->selector = 0x08;

	asm("lidt %0" : : "m" (idtr));
	/* END interrupts*/

	/* START Testing */

	Print(GlobalRenderer, "Kernel Initialize Successfully");

	/* END Testing */

	while(true);
}
