#ifndef PLAYER_DEF_H_INCLUDED
#define PLAYER_DEF_H_INCLUDED

#include "sprite_def.h"
#include "sdl_incs.h"
#include "tmath.hpp"
#include "game.hpp"

class Player : public Sprite {
	void DefaultState(void*);
	void DefendState(void*);
	void AttackState(void*);
	
	void Graphics(void*);
	
	public:
	
	uint8_t m_MiscTimer;     // miscellaneous timer
	uint8_t m_InvcTimer;     // time to stay invincible
	uint8_t m_DeathTimer;    // length of death animation
	uint8_t m_StunTimer;     // how many frames to stay stunned
	uint8_t m_FallDistance;  // how many pixels the player has fallen
	uint8_t m_ExitDir;       // direction the player exits the level
	uint8_t m_Dir;           // direction the player faces
	Object  m_obj;           // handles the speed and graphics
	
	// static data members //
	
	// ctor
	Player(SDL_Renderer*);
	
	// dtor
	~Player() {
		m_obj.unload();
	}
	
	// player code
	virtual int Main(void*);
	
	// initializes the player object
	virtual int Init();
	
	// hurts the player
	virtual void Hurt();
	
	// kills the player and resets their values
	virtual void Kill();
	
	// causes the sprite to jump
	void Jump();
};

#endif /* PLAYER_DEF_H_INCLUDED */
