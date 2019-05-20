// global game data
#include "headers/game.hpp"

// sprite helper functions
#include "headers/sprite_helpers.hpp"

// level data and functions
#include "headers/levelcode.hpp"

// camera system
#include "headers/camera.hpp"

// sound system
#include "headers/sound.hpp"

using level::ThePlayer;

/*
    Bit-flags that affect the properties of the sprite. If the first bit is set, the collision
    between the sprite and the player is calculated *every other frame*.
*/
//constexpr int _data_sprite_flags[NUMBER_OF_UNIQUE_SPRITES] = {0, 1, 1, 1, 1, 0};

//bool PlayerSpriteColl(Sprite* spr, SDL_Rect* rect) {
//	using namespace global;
//	
//	// return every other frame
//	if ( ! (_data_sprite_flags[spr->id] & 1) ) {
//	  if ( ! (FrameCounter & 1) ) return false;
//	}
//	
//	// if the player is stunned or in its dying animation
//	if (PlayerHurtTimer || PlayerDeathTimer) return false;
//	
//	// player hitbox
//	SDL_Rect& playerhit = player->get_temp_rect();
//	
//	{
//	  const SDL_Rect& temp = player->get_hitbox();
//	  
//	  playerhit.x = temp.x + player->x;
//	  playerhit.y = temp.y + player->y;
//	  playerhit.w = temp.w;
//	  playerhit.h = temp.h;
//	}
//	
//	// rectangle for the sprite's hitbox
//	SDL_Rect& sprhit = spr->obj.get_temp_rect();
//	
//	{
//	  const SDL_Rect& temp = spr->obj.get_hitbox();
//	  
//	  sprhit.x = temp.x + spr->obj.x;
//	  sprhit.y = temp.y + spr->obj.y;
//	  sprhit.w = temp.w;
//	  sprhit.h = temp.h;
//	}
//	
//	// test for an intersection of the two rectangles
//	SDL_bool result = SDL_IntersectRect(&playerhit, &sprhit, rect);
//	
//return result == SDL_TRUE ? true : false;
//}

/*
Returns 0 if the sprite is safely on the screen and can sensibly can rendered
and not waste time. But if it returns 1 that means the sprite is either off-camera
or outside the level altogether.
*/
//int SpriteOffscreen(Sprite* spr, const uint8_t entry) {
//	using camera::CameraLevel;
//	
//	int distance;
//	const SDL_Rect& sprRect = spr->obj.get_dest_rect();
//	
//	switch (entry) {
//	  default:
//	  	distance = 24;
//	  	break;
//	  
//	  case 1:
//	  	distance = 24;
//	  	break;
//	  
//	  case 2:
//	  	distance = 40;
//	  	break;
//	  
//	  case 3:
//	  	distance = 56;
//	  	break;
//	  
//	  case 4:
//	  	distance = 72;
//	  	break;
//	}
//	
//	spr->del = 0; // if this flag is non-zero, the sprite gets deleted
//	spr->off = 0; // if this flag is non-zero, the sprite is considered offscreen
//	
//	// exits the bounds of the level
//	{
//	  SDL_Rect& level = CameraLevel[1];
//	  uint8_t x = 0;
//	  const int difference[] = { sprRect.y - level.h, sprRect.x - level.w };
//	  const int coords[] = { sprRect.y, sprRect.x };
//	  const uint8_t del[] = { 0x01, 0x10 };
//	  
//	  while (x < 2) {
//	  	if (coords[x] < -distance || difference[x] >= distance)
//	  	  spr->del |= del[x];
//	  	
//	  	++x;
//	  }
//	}
//	
//	// check if the sprite goes off camera a certain amount
//	{
//	  SDL_Rect& cam = CameraLevel[0];
//	  int sproffleft = sprRect.x - cam.x;
//	  int sproffright = sprRect.x - (cam.x + cam.w);
//	  
//	  if ( sproffleft < -distance || sproffright >= (distance - sprRect.w / 2) )
//	  	spr->off |= 0x10;
//	}
//	
//return (spr->del || spr->off) ? 1 : 0;
//}

//void HurtPlayer(size_t sfx) {
//	using level::player;
//	using namespace global;
//	
//	if (PlayerInvcTimer || PlayerDeathTimer) return; // invincibility frames
//	
//	// run out of hp
//	if (--PlayerHp == 0) {
//	  KillPlayer(1);
//	  return;
//	}
//	
//	// knockback speed
//	float speed = -player->get_x_speed() * 2.0f;
//	
//	// to large
//	if (speed < -3 || speed > 3)
//	  speed = speed * 3 / 4;
//	
//	// no speed at all
//	if (speed == 0) {
//	  const char _speeds[] = { -3, 3 };
//	  char index = player->horz_direction();
//	  speed = static_cast<float>(_speeds[index]);
//	}
//	
//	// speed + timers + sound effect
//	player->set_x_speed_and_vel(speed, 0);
//	PlayerHurtTimer = 30;
//	PlayerInvcTimer = 90;
//}

//void PlayerBounce(int yinter) {
//	using level::player;
//	
//	player->y = yinter - 16;
//	player->set_y_vel(-5.0f);
//}

//void KillPlayer(char animation, size_t* sfx) {
//	using camera::CameraLevel;
//	
//	// death animation + death music
//	PlayerDeathTimer = 100;
//	player->set_collision_function(nullptr);
//	player->set_x_speed_and_vel(0, 0);
//	
//	// if no animation is warrented, stop the player
//	if (! animation) {
//	  player->set_gravity_delta(0);
//	  return;
//	}
//	
//	player->set_y_vel(-6);
//}

//void FlipDirection(Object& obj) {
//	if ( obj.horz_direction() )
//	  obj.flip(SDL_FLIP_NONE);
//	
//	else
//	  obj.flip(SDL_FLIP_HORIZONTAL);
//}

//int WhatToFacePlayer(const Sprite* spr, char* dir, int* xdist, int* ydist) {
//	const int diff_x = (int) spr->obj.x - player->x;
//	
//	if (dir)
//	  *dir = (char) spr->obj.horz_direction();
//	
//	if (xdist)
//	  *xdist = diff_x;
//	
//	if (ydist)
//	  *ydist = (int) spr->obj.y - player->y;
//	
//return diff_x >= 0 ? 0 : 1;
//}
