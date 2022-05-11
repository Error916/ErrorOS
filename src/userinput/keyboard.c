#include "keyboard.h"

bool isLeftShiftPressed;
bool isRighttShiftPressed;

void HandleKeyboard(uint8_t scancode){

	// TODO: Check the keyboard layout

	switch (scancode){
		case LeftShift:
			isLeftShiftPressed = true;
			return;
		case LeftShift + 0x80:
			isLeftShiftPressed = false;
			return;
		case RightShift:
			isRighttShiftPressed = true;
			return;
		case RightShift + 0x80:
			isRighttShiftPressed = false;
			return;
		case Enter:
			Next(GlobalRenderer);
			return;
		case Spacebar:
			PutCharS(GlobalRenderer, ' ');
			return;
		case BackSpace:
			ClearChar(GlobalRenderer);
			return;
	}

	char ascii = Translate(scancode, isLeftShiftPressed | isRighttShiftPressed);

	if(ascii != 0){
		PutCharS(GlobalRenderer, ascii);
	}
}
