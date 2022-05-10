#include "panic.h"

void Panic(const char* panicMessage){
	Clear(GlobalRenderer, 0x00ff0000);
	GlobalRenderer->CursorPosition->X = 0;
	GlobalRenderer->CursorPosition->Y = 0;
	GlobalRenderer->Color = 0x00000000;
	Print(GlobalRenderer, "Kernel Panic:");
	Next(GlobalRenderer);
	Next(GlobalRenderer);
	Print(GlobalRenderer, panicMessage);
}
