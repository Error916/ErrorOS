#include "mouse.h"

uint8_t MousePointer[] = {
	0b10000000, 0b00000000,
	0b11000000, 0b00000000,
	0b11100000, 0b00000000,
	0b11110000, 0b00000000,
	0b11111000, 0b00000000,
	0b11111100, 0b00000000,
	0b11111110, 0b00000000,
	0b11111111, 0b00000000,
	0b11111111, 0b10000000,
	0b11111111, 0b11000000,
	0b11111100, 0b00000000,
	0b11101100, 0b00000000,
	0b11000110, 0b00000000,
	0b10000110, 0b00000000,
	0b00000011, 0b00000000,
	0b00000011, 0b00000000,
};

void MouseWait(){
	uint64_t timeout = 100000;
	while(--timeout){
		if((inb(0x64) & 0b10) == 0) return;
	}
}

void MouseWaitInput(){
	uint64_t timeout = 100000;
	while(--timeout){
		if(inb(0x64) & 0b1) return;
	}
}

void MouseWrite(uint8_t value){
	MouseWait();
	outb(0x64, 0xd4);
	MouseWait();
	outb(0x60, value);
}

uint8_t MouseRead(){
	MouseWaitInput();
	return inb(0x60);
}

uint8_t MouseCycle = 0;
uint8_t MousePacket[4]; // TODO: support for scroll and 4,5 button whit 4th byte
bool MousePacketReady = false;
Point MousePosition;
Point MousePositionOLD;
void HandlePS2Mouse(uint8_t data){
	// TODO: implement an interrupt priority system so that mouse and keybord dont collide and make the mouse outof sink
	// FIXME: process packets here get them always outof sink i need to look why
	/* ProcessMousePacket(); */
	/* static bool skip = true; */
	/* if(skip){ skip = false; return; } */

	switch(MouseCycle){
		case 0:
			if(MousePacketReady) break;
			if((data & 0b00001000) == 0) break; // outof sink
			MousePacket[0] = data;
			MouseCycle++;
			break;
		case 1:
			if(MousePacketReady) break;
			MousePacket[1] = data;
			MouseCycle++;
			break;
		case 2:
			if(MousePacketReady) break;
			MousePacket[2] = data;
			MouseCycle++;
			break;
		case 3:
			if(MousePacketReady) break; // data for the scroll and more buttons https://wiki.osdev.org/Mouse_Input to unmask
			MousePacket[3] = data;
			MousePacketReady = true;
			MouseCycle = 0;
			break;
	}
}

void ProcessMousePacket(){
	if(!MousePacketReady) return;
	MousePacketReady = false;

	bool xNegative, yNegative, xOverflow, yOverflow;
	if(MousePacket[0] & PS2XSign){
		xNegative = true;
	} else xNegative = false;

	if(MousePacket[0] & PS2YSign){
		yNegative = true;
	} else yNegative = false;

	if(MousePacket[0] & PS2XOverflow){
		xOverflow = true;
	} else xOverflow = false;

	if(MousePacket[0] & PS2YOverflow){
		yOverflow = true;
	} else yOverflow = false;

	if(!xNegative){
		MousePosition.X += MousePacket[1];
		if(xOverflow){
			MousePosition.X += 255;
		}
	} else {
		MousePacket[1] = 256 - MousePacket[1];
		MousePosition.X -= MousePacket[1];
		if(xOverflow){
			MousePosition.X -= 255;
		}
	}

	//NB invert y ax
	if(!yNegative){
		MousePosition.Y -= MousePacket[2];
		if(yOverflow){
			MousePosition.Y -= 255;
		}
	} else {
		MousePacket[2] = 256 - MousePacket[2];
		MousePosition.Y += MousePacket[2];
		if(yOverflow){
			MousePosition.Y += 255;
		}
	}

	if(MousePosition.X < 0) MousePosition.X = 0;
	if(MousePosition.X > GlobalRenderer->TargetFramebuffer->Width - 1) MousePosition.X = GlobalRenderer->TargetFramebuffer->Width - 1;
	if(MousePosition.Y < 0) MousePosition.Y = 0;
	if(MousePosition.Y > GlobalRenderer->TargetFramebuffer->Height - 1) MousePosition.Y = GlobalRenderer->TargetFramebuffer->Height - 1;

	ClearMouseCursor(GlobalRenderer, MousePointer, &MousePositionOLD);
	DrawOverlayMouseCursor(GlobalRenderer, MousePointer, &MousePosition, 0xffffffff);

	// Do something if press
	if(MousePacket[0] & PS2Leftbutton){}

	if(MousePacket[0] & PS2Middlebutton){}

	if(MousePacket[0] & PS2Rightbutton){}

	MousePacketReady = false;
	MousePositionOLD = MousePosition;
}

void InitPS2Mouse(){
	outb(0x64, 0xa8); //enable auxiliary device - mouse
	MouseWait();
	outb(0x64, 0x20); //tell keyboard controller that er are sending  a command to the mouse
	MouseWaitInput();

	uint8_t status = inb(0x60);
	status |= 0b10;
	MouseWait();
	outb(0x64, 0x60);
	MouseWait();
	outb(0x60, status); //setting the correct bit in the compaq byte

	MouseWrite(0xf6);
	MouseRead();

	MouseWrite(0xf4);
	MouseRead();
}
