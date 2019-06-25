#ifndef PLAYER_DEF_H_INCLUDED
#define PLAYER_DEF_H_INCLUDED

#include "sprite_def.h"
#include "sdl_incs.h"
#include "tmath.hpp"
#include "game.hpp"

class Player : public Sprite {
public:
	uint8_t m_MiscTimer;         // miscellaneous timer
	uint8_t m_InvcTimer;         // time to stay invincible
	uint8_t m_DeathTimer;        // length of death animation
	uint8_t m_StunTimer;         // how many frames to stay stunned
	uint8_t m_FallDistance;      // how many pixels the player has fallen
	uint8_t m_ExitDir;           // direction the player exits the level
	Object  m_obj;               // handles the speed and graphics
	
	// static data members //
	
	static float sm_UpSpeeds[2]; // (static) speed of the jump
	static uint8_t sm_FrmIdx; // (static) frame index
	
	// ctor
	Player(SDL_Renderer*);
	
	// dtor
	~Player() {
		m_obj.unload();
	}
	
	// player "tile" height
	int CollTileHeight() {return 2;}
	
	// player code
	int Main(void*);
	
	// initializes the player object
	int Init();
	
	// hurts the player
	void Hurt();
	
	// kills the player and resets their values
	void Kill();
	
	// causes the sprite to jump
	void Jump();
};

#endif /* PLAYER_DEF_H_INCLUDED */
