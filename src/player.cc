#include "player_def.h"
#include "tile_collision.hpp"
#include "math.hpp"
#include "camera.hpp"
#include "memory.hpp"
#include "log.hpp"
#include "sprite_helpers.hpp"

static const float _upSpeeds[] = {-7.5f, -8.0f};
static const float sc_HorzSpeeds[] = {2.5f, 3.0f};

Player::Player(SDL_Renderer* ren) : Sprite(255), m_MiscTimer(0), m_InvcTimer(0),
                                    m_DeathTimer(0), m_StunTimer(0), m_FallDistance(0),
                                    m_ExitDir(0), m_Dir(0)
{
	// give a renderer to the object
	m_obj.renderer = ren;
}

// private:
void Player::DefaultState(void* data) {
	using game::KeySymBits;
	using game::KeySymBitsFirstFrame;
	
	uint8_t uiIndex = 0;
	
	// if the player is stunned
	if (m_StunTimer > 0)
	  --m_StunTimer;
	
	// player is invincible...
	if (m_InvcTimer > 0)
	  --m_InvcTimer;
	// or if they`re dying
	else if (m_DeathTimer > 0) {
	  if (--m_DeathTimer == 0)
	  	* cast(data, int*) = 1;
	}
	
	// bar input detections if stunned
	if (m_StunTimer == 0) {
	  // holding the S key
	  if (KeySymBits & 1) {
	  	if (m_MiscTimer < 10)
	  	  ++m_MiscTimer;
	  	else
	  	  uiIndex = 1;
	  }
	  // decrement m_MiscTimer
	  else if (m_MiscTimer > 0)
	  	--m_MiscTimer;
	  
	  // holding the left arrow key
	  if (KeySymBits & 2) {
	  	m_obj.xspeed = -(sc_HorzSpeeds[uiIndex]);
	  	m_Dir = 0;
	  }
	  // holding the right arrow key
	  else if (KeySymBits & 4) {
	  	m_obj.xspeed = sc_HorzSpeeds[uiIndex];
	  	m_Dir = 1;
	  }
	  // no keys pressed
	  else {
	  	m_obj.xspeed = 0;
	  }
	  
	  // up arrow key pressed once
	  if (KeySymBitsFirstFrame & 8) {
	  	Jump();
	  }
	  // down arrow key held down
	  else if (KeySymBits & 16) {
	  	if (GetColl(M_COLL_DOWN))
	  	  set_state(1);
	  }
	}
	
	// update sprite position based on speed
	m_obj.move();
}

void Player::DefendState(void* data) {
	using game::KeySymBits;
	
	// if the player stops holding down, revert back to state 0
	if (! (KeySymBits & 16)) {
	  set_state(0);
	}
}

void Player::AttackState(void* data) {
	//
}
//////////////////////

void Player::set_state(int state) {
	switch (state) {
	  default:
	  	state = m_state;
	  	break;
	  
	  case 0:
	  	{
	  	  SDL_Rect thitbox = {34, 10, 17, 68};
	  	  m_obj.set_hitbox(thitbox);
	  	}
	  	break;
	  
	  case 1:
	  	{
	  	  SDL_Rect thitbox = {25, 10, 28, 68};
	  	  m_obj.set_hitbox(thitbox);
	  	  m_obj.xspeed = 0;
	  	}
	  	break;
	}
	
	m_state = state;
}

void Player::Graphics(void* data) {
	using game::FrameCounter;
	using game::KeySymBits;
	
	uint8_t uiFrameIdx = 0;
	
	switch (m_state) {
	  default: break;
	  
	  case 0:
	  	// moving left or right
	  	if (KeySymBits & 6) {
	  	  uiFrameIdx = ((FrameCounter / 4) & 1) + 3 + m_Dir * 8;
	  	}
	  	// standing still
	  	else {
	  	  uiFrameIdx = m_Dir * 8;
	  	}
	  	
	  	// if the player is jumping
	  	if (! GetColl(M_COLL_DOWN)) {
	  	  uiFrameIdx = m_Dir * 8 + 1;
	  	}
	  	break;
	  
	  case 1:
	  	uiFrameIdx = m_Dir * 8 + 5;
	  	break;
	}
	
	// blit graphics as long as the player isn`t blinking
	if (! m_InvcTimer || ! (m_InvcTimer % 3)) {
	  m_obj.frame = uiFrameIdx;
	  m_obj.blit(&CAM_CAMERA);
	}
}

int Player::Main(void* data) {
	uint8_t retval = 0;
	
	switch (m_state) {
	  default: break;
	  
	  case 0:
	  	DefaultState(&retval);
	  	break;
	  
	  case 1:
	  	DefendState(&retval);
	  	break;
	  
	  case 2:
	  	AttackState(&retval);
	  	break;
	}
	
	Graphics(data);
	
return (int) retval;
}

int Player::Init() {
	// open the player's graphics
	if (! m_obj.loaded()) {
	  // save position
	  int temp[] = {m_obj.get_x(), m_obj.get_y()};
	  
	  // open graphics
	  m_obj.open("images/sprites/kevin.png", 0xffffffff);
	  
	  if ( m_obj.error() ) {
	  	std::cerr << m_obj.get_error() << '\n';
	  	return 1;
	  }
	  
	  // restore position
	  m_obj.set_xy(temp[0], temp[1]);
	}
	
	// clip the sprite graphics and blit it at a certain size
	m_obj.set_clip_size(101, 78);
	m_obj.set_blit_size(101, 78);
	
	set_state(0);
	
	// set the player's X delta
	m_obj.xfactor = CalcDelta(FPS, 7);
	
	m_obj.collfunc_ud[0] = PlayerTileCollisionUpDown;
	m_obj.collfunc_lr = SpriteTileCollision_LeftRight;
	
	// set the player's HP
	m_hp = 5;
	
	// set INIT flag
	SetInit();
	
return 0;
}

int Player::Hurt() {
	constexpr float _static_xspeeds[2] = {
	  20.0f, -20.0f
	};
	
	if (m_InvcTimer) return 0; // player's invincible
	
	switch (m_state) {
	  default: break;
	  
	  case 0:
	  	// default state
	  	--m_hp;
	  	m_StunTimer = Ms2Frames(2000);
	  	m_InvcTimer = Ms2Frames(5000);
	  	m_obj.xspeed = 0;
	  	m_obj.launch_xspeed( _static_xspeeds[(int) m_Dir] );
	  	break;
	  
	  case 1:
	  	{
	  	  // defend state TODO work on
	  	  SDL_Rect* intersection = InterRect.getp();
	  	  SDL_Rect& playerhitbox = m_obj.add_hitbox();
	  	  
	  	  // player is facing left
	  	  if (m_Dir == 0) {
	  	  	if (intersection->x != playerhitbox.x) {
	  	  	  // hurt the player
	  	  	  --m_hp;
	  	  	  m_StunTimer = Ms2Frames(500);
	  	  	  m_InvcTimer = Ms2Frames(500);
	  	  	  m_obj.xspeed = 0;
	  	  	}
	  	  	else {
	  	  	  return 1;
	  	  	}
	  	  }
	  	  // player is facing right
	  	  else {
	  	  	if (intersection->x == playerhitbox.x) {
	  	  	  // hurt the player
	  	  	  --m_hp;
	  	  	  m_StunTimer = Ms2Frames(500);
	  	  	  m_InvcTimer = Ms2Frames(500);
	  	  	  m_obj.xspeed = 0;
	  	  	}
	  	  	else {
	  	  	  return 1;
	  	  	}
	  	  }
	  	  
	  	  // launch the player backward
	  	  m_obj.launch_xspeed(_static_xspeeds[(int) m_Dir]);
	  	  m_obj.yspeed = -5.0f;
	  	}
	  	break;
	}
	
	// die with no hp
	if (m_hp == 0)
	  Kill();
	
return 0;
}

int Player::Kill() {
	m_DeathTimer = 255; // death animation timer
	m_obj.xspeed = 0; // no speed
	
return 0;
}

void Player::Jump() {
	// if the player is on the ground
	if (GetColl(M_COLL_DOWN)) {
	  // index to a speed table
	  const int iSpeed = tabs(m_obj.curxspeed()) > sc_HorzSpeeds[0];
	  
	  // attribute a negative Y speed
	  m_obj.yspeed = _upSpeeds[iSpeed];
	  
	  // take player off the ground
	  m_obj.incr_y(-2);
	  
	  // unset collision flag
	  UnsetColl(M_COLL_DOWN);
	}
}
