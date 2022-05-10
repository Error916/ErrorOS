#include "interrupts.h"

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
	Print(GlobalRenderer, "Page Fault Detected");
	while(1);
}
