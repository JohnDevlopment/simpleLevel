#include "player_def.h"
#include "tile_collision.hpp"
#include "math.hpp"
#include "private/player_data_def.h"
#include "camera.hpp"
#include "memory.hpp"
#include "log.hpp"

int SpriteTileCollisionOneStep(SDL_Rect* loc, SDL_Rect& collbox, int steps);
int SpriteTileCollision_LeftRight(SDL_Rect* loc, SDL_Rect& collbox, int steps);

static const float _upSpeeds[] = {-5.5f, -6.5f};
static const float sc_HorzSpeeds[] = {2.5f, 3.5f};

Player::Player(SDL_Renderer* ren) : Sprite(255), m_MiscTimer(0), m_InvcTimer(0),
                                    m_DeathTimer(0), m_StunTimer(0), m_FallDistance(0),
                                    m_ExitDir(0), m_Dir(0)
{
	// give a renderer to the object
	m_obj.renderer = ren;
}

// private:
void Player::DefaultState(void* data) {
	using game::FrameCounter;
	
	uint16_t uiBits, uiFirst;
	uint8_t uiIndex = 0;
	
	uiBits  = * cast(data, _PlayerData*)->KeySymBits;
	uiFirst = * cast(data, _PlayerData*)->KeySymBitsFirstFrame;
	
	// if the player is stunned
	if (m_StunTimer > 0)
	  --m_StunTimer;
	
	// player is invincible...
	if (m_InvcTimer > 0)
	  --m_InvcTimer;
	// or if they`re dying
	else if (m_DeathTimer > 0) {
	  if (--m_DeathTimer == 0)
	  	cast(data, _PlayerData*)->what = 1;
	}
	
	// bar input detections
	if (m_StunTimer == 0) {
	  // holding the S key
	  if (uiBits & 1) {
	  	if (m_MiscTimer < 10)
	  	  ++m_MiscTimer;
	  	else
	  	  uiIndex = 1;
	  }
	  // decrement m_MiscTimer
	  else if (m_MiscTimer > 0)
	  	--m_MiscTimer;
	  
	  // holding the left arrow key
	  if (uiBits & 2) {
	  	m_obj.xspeed = -(sc_HorzSpeeds[uiIndex]);
	  	m_Dir = 0;
	  }
	  // holding the right arrow key
	  else if (uiBits & 4) {
	  	m_obj.xspeed = sc_HorzSpeeds[uiIndex];
	  	m_Dir = 1;
	  }
	  // no keys pressed
	  else {
	  	m_obj.xspeed = 0;
	  }
	  
	  // up arrow key pressed once
	  if (uiFirst & 8) {
	  	Jump();
	  }
	  // down arrow key held down
	  else if (uiBits & 16) {
	  	if ( GetColl(M_COLL_DOWN) )
	  	  set_state(1);
	  }
	}
	
	// update sprite position based on speed
	m_obj.move();
}

void Player::DefendState(void* data) {
	const uint16_t uiBits = * cast(data, _PlayerData*)->KeySymBits;
	
	// if the player stops holding down, revert back to state 0
	if (! (uiBits & 16)) {
	  set_state(0);
//	  m_obj.launch_xspeed(20.0f);
//	  m_obj.yspeed = -5.0f;
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
	
	const uint16_t uiBits = * cast(data, _PlayerData*)->KeySymBits;
	uint8_t uiFrameIdx = 0;
	
	switch (m_state) {
	  default: break;
	  
	  case 0:
	  	// moving left or right
	  	if (uiBits & 6) {
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
	  	DefaultState(data);
	  	break;
	  
	  case 1:
	  	DefendState(data);
	  	break;
	  
	  case 2:
	  	AttackState(data);
	  	break;
	}
	
	if ( cast(data, _PlayerData*)->what == 1 ) {
	  retval = 1;
	  cast(data, _PlayerData*)->what = 0;
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
	
	m_obj.collfunc_ud[0] = SpriteTileCollisionOneStep;
	m_obj.collfunc_lr = SpriteTileCollision_LeftRight;
	
	// set the player's HP
	m_hp = 5;
	
	// set INIT flag
	SetInit();
	
return 0;
}

int Player::Hurt() {
	using game::HeapStack;
	
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
	  	  SDL_Rect* intersection = memblk(HeapStack, SDL_Rect, HS_CollRect);
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
	  const int iSpeed = tabs( m_obj.curxspeed() ) > 2.5f;
	  
	  // attribute a negative Y speed
	  m_obj.yspeed = _upSpeeds[iSpeed];
	  
	  // take player off the ground
	  m_obj.incr_y(-2);
	  
	  // unset collision flag
	  UnsetColl(M_COLL_DOWN);
	}
}
