#ifndef _ROTDATA_H_INCLUDED
#define _ROTDATA_H_INCLUDED

#include "sdl_incs.h"

struct _RotData {
	double angle;
	SDL_Point pivot;
	bool use;
	
	_RotData() : angle(0.0), pivot(), use(false) {}
};

#endif /* _ROTDATA_H_INCLUDED */
