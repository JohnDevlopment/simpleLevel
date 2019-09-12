#ifndef SPRITESLOTS_H_INCLUDED
#define SPRITESLOTS_H_INCLUDED

#include <iostream>
#include "pdobject.h"

class SpriteSlot_t {
	Sprite_t sprite;
	bool used;
	
	public:
	
	SpriteSlot_t() : used(false)
	{
		// ctor
		std::memset(&sprite, 0, sizeof(Sprite_t));
	}
	
	~SpriteSlot_t()
	{
		// dtor
	}
};

#endif /* SPRITESLOTS_H_INCLUDED */
