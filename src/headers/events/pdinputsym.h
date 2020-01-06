#ifndef PDINPUTSYM_H
#define PDINPUTSYM_H

#include "sdl_incs.h"

enum PDInputSymEnum {
	PDSym_Unknown      = 0,
	PDSym_A            = 1,
	PDSym_B            = 2,
	PDSym_X            = 3,
	PDSym_Y            = 4,
	PDSym_DPAD_LEFT    = 5,
	PDSym_DPAD_RIGHT   = 6,
	PDSym_DPAD_UP      = 7,
	PDSym_DPAD_DOWN    = 8,
	PDSym_LBUTTON      = 9,
	PDSym_RBUTTON      = 10,
	PDSym_START        = 11,
	PDSym_SELECT       = 12,
	
	PDSym_Last
};

/* Abstraction of an input symbol. Functions are implemented in input.cc. */
struct PDInputSym {
	int sym;
	int type;
	bool pressed;
	bool triggered;
};

#endif /* PDINPUTSYM_H */
