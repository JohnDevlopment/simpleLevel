// class Player
#include "headers/player_def.h"

// tile collision code
#include "headers/tile_collision.hpp"

#include "headers/math.hpp"

float Player::sm_UpSpeeds[2] = {-5.5f, -6.5f};

Player::Player(SDL_Renderer* ren) : Sprite(255), m_MiscTimer(0), m_InvcTimer(0),
                                    m_DeathTimer(0), m_StunTimer(0), m_FallDistance(0),
                                    m_ExitDir(0)
{
	// give a renderer to the object
	m_obj.renderer = ren;
}

int Player::Init() {
	// open the player's graphics
	if (! m_obj.loaded()) {
	  // save position
	  int temp[] = {m_obj.x, m_obj.y};
	  
	  // open graphics
	  m_obj.open("images/sprites/kevin.png", 0xffffffff);
	  
	  if (m_obj.error()) {
	  	std::cerr << m_obj.get_error() << '\n';
	  	return 1;
	  }
	  
	  // restore position
	  m_obj.x = temp[0];
	  m_obj.y = temp[1];
	}
	
	// clip the sprite graphics and blit it at a certain size
	m_obj.set_clip_size(16, 32);
	m_obj.set_blit_size(32, 64);
	
	// set the player's hitbox: original hitbox = 2, 2, 14, 30
	m_obj.set_hitbox(4, 4, 28, 60);
	
	// set the player's collision function
	m_obj.set_collision_function(SpriteTileCollision);
	
	// enable the player to flip its graphics
	m_obj.use_flip(SDL_USEFLIPOBJ_YES);
	
	// set the player's X delta
	m_obj.set_x_delta(FPS, 8);
	
	// set the gravity delta and the gravity speed
	m_obj.set_gravity_delta(FPS, 15);
	m_obj.set_gravity_speed(GRAVITY_SPEED);
	
	// set the player's HP
	m_hp = 5;
	
	// set INIT flag
	SetInit();
	
return 0;
}

void Player::Hurt() {
	if (m_InvcTimer) return; // player's invincible
	
	// die with no hp
	if (--m_hp == 0)
	  Kill();
	
	// lose hp, temporary invincibility
	else {
	  m_StunTimer = Ms2Frames(2000);   // amount of time to stay still
	  m_InvcTimer = Ms2Frames(5000);   // how long to stay invincible
	  m_obj.set_x_speed_and_vel(0, 0); // freeze player
	}
}

void Player::Kill() {
	m_obj.set_x_speed_and_vel(0, 0); // freeze player
	m_DeathTimer = 255;              // amount of time to show death animation
}

void Player::Jump() {
	// if the player is on the ground
	if (GetColl(M_COLL_DOWN)) {
	  // index to a speed table
	  const int iSpeed = tabs(m_obj.get_x_speed()) > 2.5f ? 1 : 0;
	  
	  // attribute a negative Y speed
	  m_obj.set_y_speed_and_vel(sm_UpSpeeds[iSpeed], sm_UpSpeeds[iSpeed]);
	  
	  // take player off the ground
	  m_obj.y -= 2;
	  
	  // unset collision flag
	  UnsetColl(M_COLL_DOWN);
	}
}
