#include "sprites/sprite000_def.h"
#include "texture_packs.hpp"
#include "camera.hpp"
#include "sprite_helpers.hpp"
#include "levelcode.hpp"
#include "sound.hpp"
#include "memory.hpp"

/* Withdraw = 0
   Default = 1
   Attack = 2
   Charge = 3
   Die = 4

*/

static const int _const_attk_tm[] = {55, 30, 20};
static const int _attk_anim_x_off[] = {0, 76, 152};

Sprite000::Sprite000(int x, int y, SDL_Renderer* ren) : Sprite(0), m_timer(0), m_counter(0) {
	// ctor
	std::memset(&m_stats, 0, sizeof(Stats));
	std::memset(&m_hitbox, 0, sizeof(SDL_Rect));
	m_gfx.set_xy(x, y + 16);
	m_gfx.renderer = ren;
}

Sprite000::~Sprite000() {
	// dtor
	m_gfx.unload();
}

int Sprite000::Init() {
	{
	  const SDL_Rect temp = m_gfx.get_dest_rect();
	  m_gfx.use_pack(SpriteTXPackList[0]);
	  m_gfx.use_flip(SDL_USEFLIPOBJ_YES);
	  m_gfx.set_xy(temp.x, temp.y);
	}
	
	m_stats.attack = 1;
	m_stats.defense = 0;
	m_stats.evasion = 10;
	m_stats.accuracy = 100;
	
	m_hp = 3;
	change_state(0);
	set_hitbox();
	
return 0;
}

// private:
void Sprite000::set_hitbox() {
	const SDL_Rect& dest = m_gfx.get_dest_rect();
	
	switch (m_state) {
	  default: break;
	  
	  case 0:
	  	{
	  	  const int iCenter = dest.x + dest.w / 2;
	  	  m_hitbox.x = iCenter - 64;
	  	  m_hitbox.y = dest.y - 16;
	  	  m_hitbox.w = 64 * 2;
	  	  m_hitbox.h = dest.h + 16;
	  	}
	  	break;
	  
	  case 1:
	  	{
	  	  m_hitbox.x = dest.x;
	  	  m_hitbox.y = dest.y;
	  	  m_hitbox.w = 26;
	  	  m_hitbox.h = 50;
	  	}
	  	break;
	  
	  case 2:
	  	{
	  	  m_hitbox.w = 25;
	  	  m_hitbox.h = 25;
	  	  
	  	  m_hitbox.y = dest.y + 8;
	  	  
	  	  if ( m_gfx.horz_direction() == 0 ) {
	  	  	m_hitbox.x = dest.x + 3;
	  	  }
	  	  else {
	  	  	m_hitbox.x = (dest.x + dest.w) - (m_hitbox.w + 3);
	  	  }
	  	}
	  	break;
	}
}

// private:
void Sprite000::change_state(int state) {
	static const int _xoff_def2attk = 38;
	
	switch (state) {
	  default:
	  	state = -1;
	  	break;
	  
	  case 0:
	  case 1:
	  	if (m_state == 2) {
	  	  int iDestx = m_gfx.get_dest_rect().x;
	  	  
	  	  if ( m_gfx.horz_direction() == 0 ) iDestx += _xoff_def2attk;
	  	  m_gfx.set_x(iDestx);
	  	}
	  	
	  	m_gfx.set_blit_size(38, 80);
	  	m_gfx.set_clip_size(38, 80);
	  	m_gfx.frame = 0;
	  	m_timer = -1;
	  	break;
	  
	  case 2:
	  	m_timer = 55;
	  	m_gfx.set_clip_size(76, 80);
	  	m_gfx.set_blit_size(76, 80);
	  	{
	  	  int iDestx = m_gfx.get_dest_rect().x;
	  	  
	  	  if ( m_gfx.horz_direction() == 0 ) iDestx -= _xoff_def2attk;
	  	  m_gfx.set_x(iDestx);
	  	}
	  	break;
	}
	
	if (state >= 0) m_state = state;
}

void Sprite000::Graphics() {
	using game::FrameCounter;
	SDL_Rect* const camera = &CAM_CAMERA;
	
	switch (m_state) {
	  default: break;
	  
	  case 0:
	  	m_gfx.frame = 6;
	  	m_gfx.blit(camera);
	  	break;
	  
	  case 1:
	  	{
	  	  int iFrame = m_gfx.frame;
	  	  
	  	  if (m_coll) {
	  	  	--m_coll;
	  	  	iFrame = 6;
	  	  }
	  	  else if (! (FrameCounter & 7)) {
	  	  	if (iFrame == 2 || iFrame == 6) {
	  	  	  iFrame = 0;
	  	  	}
	  	  	else {
	  	  	  ++iFrame;
	  	  	}
	  	  }
	  	  m_gfx.frame = iFrame;
	  	}
	  	m_gfx.blit(camera);
	  	break;
	  
	  case 2:
	  	{
	  	  //_const_attk_tm
	  	  char iFrame;
	  	  SDL_Rect dest = m_gfx.get_dest_rect();
	  	  SDL_Rect& clip = const_cast<SDL_Rect&>( m_gfx.get_clip_rect() );
	  	  
	  	  if (m_timer >= _const_attk_tm[1]) {
	  	  	iFrame = 2;
	  	  }
	  	  else if (m_timer >= _const_attk_tm[2]) {
	  	  	iFrame = 0;
	  	  }
	  	  else {
	  	  	iFrame = 1;
	  	  }
	  	  
	  	  clip.x = _attk_anim_x_off[(int) iFrame];
	  	  clip.y = 160;
	  	  m_gfx.Image::blit(dest.x, dest.y, camera);
	  	  m_gfx.set_xy(dest.x, dest.y);
	  	}
	  	break;
	}
}

////////////// MAIN routine, separated into state functions /////////////////
int Sprite000::WithdrawState() {
	// every other frame, if the sprite is not offscreen, enter state 1 if the player gets close
	if (game::FrameCounter & 1) {
	  if (! SpriteOffscreen(&m_hitbox, 0)) {
	  	const SDL_Rect& collrect = level::ThePlayer->m_obj.add_hitbox();
	  	if (SDL_HasIntersection(&collrect, &m_hitbox)) {
	  	  change_state(1);
	  	  set_hitbox();
	  	}
	  }
	}
	
return 0;
}

int Sprite000::DefaultState() {
	int iTimer;
	
	// revert to Withdraw state if off-camera
	if (game::FrameCounter & 1) {
	  if ( SpriteOffscreen(&m_hitbox, 0) ) {
	  	change_state(0);
	  	set_hitbox();
	  	return 0;
	  }
	}
	
	// deplete timer
	iTimer = m_timer--;
	if (iTimer < 0) {
	  if (m_coll > 0) {
	  	--m_coll;
	  }
	  else {
	  	m_timer = iTimer = 90 + 15 * m_counter;
	  	iTimer = m_timer--;
	  }
	}
	
	// every four frames, flip the direction of the sprite if it isn`t facing the player
	if (! (game::FrameCounter & 3)) {
	  if ( FacePlayer(m_hitbox, nullptr, nullptr, nullptr) != m_gfx.horz_direction() ) {
	  	FlipDirection(&m_gfx);
	  }
	}
	
	// when the timer depletes, attack
	if (iTimer == 0) {
	  int iDir = FacePlayer(m_hitbox, nullptr, nullptr, nullptr);
	  if ( iDir != m_gfx.horz_direction() ) {
	  	FlipDirection(&m_gfx);
	  }
	  change_state(2);
	}
	
return 0;
}

int Sprite000::AttackState() {
	using level::ThePlayer;
	int iTimer = m_timer--;
	
	// revert back to Default state
	if (iTimer == 0) {
	  change_state(1);
	  set_hitbox();
	  return 0;
	}
	
	// set hitbox prior to attacking
	if (iTimer == _const_attk_tm[2]) {
	  set_hitbox();
	}
	// collision active during this timeframe
	else if (iTimer < _const_attk_tm[2]) {
	  SDL_Rect* intersection = memblk(game::HeapStack, SDL_Rect, (int) HS_CollRect);
	  
	  if ( PlayerSpriteColl(m_hitbox, intersection) ) {
	  	bool ishurt = (bool) ThePlayer->Hurt();
	  	
	  	// player isn`t hurt
	  	if (ishurt) {
	  	  const float iLaunchSpeeds[] = {-20.0f, 20.0f};
	  	  ThePlayer->m_obj.launch_xspeed(iLaunchSpeeds[(int) m_gfx.horz_direction()]);
	  	}
	  	
	  	m_timer = 0;
//	  	change_state(1);
//	  	set_hitbox();
	  }
	}
	
return 0;
}

int Sprite000::Main(void* udata) {
	int iRetval;
	
	switch (m_state) {
	  default:
	  	iRetval = -1;
	  	break;
	  
	  case 0:
	  	iRetval = WithdrawState();
	  	break;
	  
	  case 1:
	  	iRetval = DefaultState();
	  	break;
	  
	  case 2:
	  	iRetval = AttackState();
	  	break;
	}
	
	Graphics();
	
return iRetval;
}
