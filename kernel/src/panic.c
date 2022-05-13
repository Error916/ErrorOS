#include "panic.h"

void Panic(const char* panicMessage){
	GlobalRenderer->ClearColor = 0x00ff0000;
	Clear(GlobalRenderer);
	GlobalRenderer->CursorPosition->X = 0;
	GlobalRenderer->CursorPosition->Y = 0;
	GlobalRenderer->Color = 0x00000000;
	Print(GlobalRenderer, "Kernel Panic:");
	Next(GlobalRenderer);
	Next(GlobalRenderer);
	Print(GlobalRenderer, panicMessage);
}
