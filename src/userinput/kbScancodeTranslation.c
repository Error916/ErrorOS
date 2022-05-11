#include "kbScancodeTranslation.h"

/* Set 1 of US QWERTY not compleated */
// TODO: finish the table https://wiki.osdev.org/PS/2_Keyboard
const char ASCIITable[] = {
	 0 ,  0 , '1', '2',
	'3', '4', '5', '6',
	'7', '8', '9', '0',
	'-', '=',  0 ,  0 ,
	'q', 'w', 'e', 'r',
	't', 'y', 'u', 'i',
	'o', 'p', '[', ']',
	 0 ,  0 , 'a', 's',
	'd', 'f', 'g', 'h',
	'j', 'k', 'l', ';',
	'\'','`',  0 , '\\',
	'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',',
	'.', '/',  0 , '*',
	0 , ' '
};

char Translate(uint8_t scancode, bool uppercase){
	//scan code of values over 58 are not implemented yet
	if(scancode > 58) return 0;

	if(uppercase)
		return ASCIITable[scancode] - 32;
	else
		return ASCIITable[scancode];
}
