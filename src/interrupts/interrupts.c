#include "interrupts.h"

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
	Panic("Page Fault Detected");
	while(1);
}

__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame){
	Panic("Double Fault Detected");
	while(1);
}

__attribute__((interrupt)) void GPFault_Handler(struct interrupt_frame* frame){
	Panic("General Protection Fault Detected");
	while(1);
}
