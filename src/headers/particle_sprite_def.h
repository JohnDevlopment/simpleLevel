#ifndef PARTICLE_SPRITE_DEF_H_INCLUDED
#define PARTICLE_SPRITE_DEF_H_INCLUDED

#include <iostream>
#include <image_class.hpp>
#include "particle.hpp"
#include <stdexcept>
#include "stdinc.h"
#include "sdl_incs.h"

class ParticleSprite {
	size_t m_count;
	Particle** m_particles;
	SDL_Renderer* m_ren;
	SDL_Texture* m_tex;
	SDL_Rect m_blit;
	
	void clip_frame(Particle* ptr);
	
	public:
	
	typedef void (*FPV_SetVals)(Particle*, void*);
	
	ParticleSprite(size_t num, SDL_Renderer* renderer, SDL_Texture* texture, int w, int h);
	~ParticleSprite();
	
	void set_locations(int x, int y, FPV_SetVals dist, void* udata);
	void render();
	void update(int flags);
};

#endif /* PARTICLE_SPRITE_DEF_H_INCLUDED */
