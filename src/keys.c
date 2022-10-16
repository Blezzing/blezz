#include"keys.h"

char getCharfromKeysym(int keysym){
	if (keysym >= XKB_KEY_a && keysym <= XKB_KEY_z)
		return 'a' + (keysym - XKB_KEY_a);
	if (keysym >= XKB_KEY_A && keysym <= XKB_KEY_Z)
		return 'A' + (keysym - XKB_KEY_A);
	if (keysym == XKB_KEY_comma)
		return ',';
	if (keysym == XKB_KEY_period)
		return '.';
	if (keysym == XKB_KEY_minus)
		return '-';
	if (keysym >= XKB_KEY_0 && keysym <= XKB_KEY_9)
		return '0' + (keysym - XKB_KEY_0);
	return '\0';
}
