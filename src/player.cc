#include "player_def.h"
#include "tile_collision.hpp"
#include "math.hpp"
#include "private/player_data_def.h"

int SpriteTileCollisionOneStep(Rect* loc, Rect& collbox, int steps);
int SpriteTileCollision_LeftRight(Rect* loc, Rect& collbox, int steps);

float Player::sm_UpSpeeds[2] = {-5.5f, -6.5f};
uint8_t Player::sm_FrmIdx = 1;

Player::Player(SDL_Renderer* ren) : Sprite(255), m_MiscTimer(0), m_InvcTimer(0),
                                    m_DeathTimer(0), m_StunTimer(0), m_FallDistance(0),
                                    m_ExitDir(0)
{
	// give a renderer to the object
	m_obj.renderer = ren;
}

int Player::Main(void* data) {
	using game::FrameCounter;
	
	int retval = 0;
	_PlayerData* playerData = (_PlayerData*) data;
	uint8_t uiBitmask = *(playerData->KeySymBits);
	
	// called from level::update
	if (playerData->what == 1) {
	  if (m_StunTimer > 0) --m_StunTimer;
	  
	  if (m_InvcTimer > 0) {
	  	--m_InvcTimer;
	  }
	  else if (m_DeathTimer > 0) {
	  	--m_DeathTimer;
	  	retval = 1;
	  }
	}
	
	else if (playerData->what == 2) {
	  // every three frames, change the animation frame when the player is moving left or right
	  if (uiBitmask & (_key_sym_bitmasks[GMLevel_Key_Left] | _key_sym_bitmasks[GMLevel_Key_Right])) {
	  	if ( ! (FrameCounter % 3) ) {
	  	  sm_FrmIdx = (FrameCounter % 2) + 1;
	  	}
	  	m_obj.frame = sm_FrmIdx;
	  }
	  else {
	  	m_obj.frame = 0;
	  }
	  
	  if (m_InvcTimer) {
	  	int iFrame = m_obj.frame;
	  	const int iFrames[] = {iFrame, 3};
	  	m_obj.frame = iFrames[FrameCounter % 3];
	  }
	}
	
return retval;
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
	m_obj.set_clip_size(16, 32);
	m_obj.set_blit_size(32, 64);
	
	// set the player's hitbox: original hitbox = 2, 2, 14, 30
	{
	  Rect temp = {4, 4, 28, 60};
	  m_obj.set_hitbox(temp);
	}
	
	// enable the player to flip its graphics
	m_obj.use_flip(SDL_USEFLIPOBJ_YES);
	
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

void Player::Hurt() {
	if (m_InvcTimer) return; // player's invincible
	
	// die with no hp
	if (--m_hp == 0)
	  Kill();
	
	// lose hp, temporary invincibility
	else {
	  m_StunTimer = Ms2Frames(2000); // amount of time to stay still
	  m_InvcTimer = Ms2Frames(5000); // how long to stay invincible
	  m_obj.xspeed = 0;              // freeze player
	}
}

void Player::Kill() {
	m_DeathTimer = 255; // death animation timer
	m_obj.xspeed = 0; // no speed
}

void Player::Jump() {
	// if the player is on the ground
	if (GetColl(M_COLL_DOWN)) {
	  // index to a speed table
	  const int iSpeed = tabs( m_obj.curxspeed() ) > 2.5f;
	  
	  // attribute a negative Y speed
	  m_obj.yspeed = sm_UpSpeeds[iSpeed];
	  
	  // take player off the ground
	  m_obj.incr_y(-2);
	  
	  // unset collision flag
	  UnsetColl(M_COLL_DOWN);
	}
}
