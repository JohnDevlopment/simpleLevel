#ifndef SPRITE_HELPERS
#define SPRITE_HELPERS

#include "sprite_router.hpp"
#include "global_attributes.hpp"
#include <generic_class.hpp>

extern generic_class<SDL_Rect> InterRect;

/* Returns 1 if the sprite is completely off-camera, or 0 otherwise. An error is returned if A is NULL.

*/
int SpriteOffscreen(const SDL_Rect* A, const uint8_t entry) HOT;

/* Returns true if the given sprite hitbox collides with the sprite, false otherwise.

*/
bool PlayerSpriteColl(const SDL_Rect& A, SDL_Rect* intersection) HOT;

/* Returns 1 if the player is to the right of the given sprite hitbox, or 0 otherwise.
   Optionally three pointers can be given to fill them with the return value, the
   horizontal distance, and the vertical distance respectively.

*/
int FacePlayer(const SDL_Rect& A, int* dir, int* xdist, int* ydist) HOT;

INLINE void FlipDirection(Image* obj) {
	if (! obj) return;
	
	if ( obj->horz_direction() ) {
	  obj->flip(SDL_FLIP_NONE);
	}
	else {
	  obj->flip(SDL_FLIP_HORIZONTAL);
	}
}

#endif /* SPRITE_HELPERS */
