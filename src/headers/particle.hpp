#ifndef PARTICLE_HPP_INCLUDED
#define PARTICLE_HPP_INCLUDED

#include <iostream>
#include "sdl_incs.h"

struct Particle {
	SDL_Rect blit, clip;
	int frame;
	size_t length;
};

#endif /* PARTICLE_HPP_INCLUDED */
