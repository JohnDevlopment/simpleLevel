#include "game.hpp"
#include "sprite_helpers.hpp"
#include "levelcode.hpp"
#include "camera.hpp"
#include "sound.hpp"
#include "memory.hpp"

generic_class<SDL_Rect> InterRect;

using level::ThePlayer;

/*
    Bit-flags that affect the properties of the sprite. If the first bit is set, the collision
    between the sprite and the player is calculated *every other frame*.
*/
//constexpr int _data_sprite_flags[NUMBER_OF_UNIQUE_SPRITES] = {0, 1, 1, 1, 1, 0};

bool PlayerSpriteColl(const SDL_Rect& A, SDL_Rect* intersection) {
	using game::FrameCounter;
	using level::ThePlayer;
	
	bool result;
	SDL_Rect& B = ThePlayer->m_obj.add_hitbox();
	
	// return every even frame
	if ( ! (FrameCounter & 1) ) return false;
	
	// if the player is dying or invincible
	if (ThePlayer->m_InvcTimer | ThePlayer->m_DeathTimer)  return false;
	
	// test the intersection between both rects
	if (! intersection)
	  result = (bool) SDL_HasIntersection(&A, &B);
	else
	  result = (bool) SDL_IntersectRect(&A, &B, intersection);
	
return result;
}

int SpriteOffscreen(const SDL_Rect* A, const uint8_t entry) {
	static const int _distances[4] = {24, 40, 56, 72};
	
	assert(A != nullptr);
	
	SDL_Rect& camera = CAM_CAMERA;
	int iDist = _distances[(int) entry % 4];
	bool retval;
	
	//	  int sproffright = sprRect.x - (cam.x + cam.w);
	//	  if ( sproffleft < -distance || sproffright >= (distance - sprRect.w / 2) )
	retval = ( (A->x - camera.x) < -iDist ) || ( (A->x - (camera.x + camera.w)) >= (iDist - A->w / 2) );
	
return static_cast<int>(retval);
}

int FacePlayer(const SDL_Rect& A, int* dir, int* xdist, int* ydist) {
	using level::ThePlayer;
	int iDiffx;
	const SDL_Rect& B = ThePlayer->m_obj.get_dest_rect();
	
	iDiffx = B.x - A.x;
	if (xdist) *xdist = iDiffx;
	if (ydist) *ydist = A.y - B.y;
	
return iDiffx >= 0;
}
