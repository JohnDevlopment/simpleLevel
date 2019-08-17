#include "object/object.hpp"
#include "memory.hpp"
#include "tmath.hpp"
#include "sprite_def.h"
#include "levelcode.hpp"
#include "log.hpp"

#define BITMASK_YSTEP       0xFF000000
#define BITMASK_YCUR        0x00FF0000
#define BITMASK_YFACTOR     0x0000FF00
#define BITMASK_YSPEED      0x000000FF

#define BITSHIFT_YSTEP      24
#define BITSHIFT_YCUR       16
#define BITSHIFT_YFACTOR    8

#define BITMASK_FULL_INT    0xFFFFFFFF

#define YSpeed(spd, fct)    ((spd & 0xFF) \
                             | ((fct & 0xFF) << BITSHIFT_YFACTOR))

#define LE 1234
#define BE 4321

#define DEACC 0.3

extern Sprite* CurrentSprite;

unsigned int Object::count = 0;

Object::Object() : xfrac(0), xcspeed(0), yfrac(0), ycspeed(0), collfunc_lr(nullptr), yspeed(0), yfactor(0), xspeed(0), xfactor(0), frame(0)
{
	// ctor
	++count;
	std::memset(&hitbox, 0, sizeof(SDL_Rect));
	std::memset(collfunc_ud, 0, sizeof(Object_CollisionFunction) * 2);
}

Object::~Object()
{
	// dtor
	--count;
}

void Object::clip_frame() {
	if (frame == 0) {
	  ClipRect.x = 0;
	  ClipRect.y = 0;
	  return;
	}
	
	ClipRect.x = frame * ClipRect.w;
	if (ClipRect.x >= ImageRect.w) {
	  ClipRect.y = (ClipRect.x / ImageRect.w) * ClipRect.h;
	  ClipRect.x -= ImageRect.w * (ClipRect.y / ClipRect.h);
	}
	else {
	  ClipRect.y = 0;
	}
	
	if (ClipRect.y >= ImageRect.h) {
	  ClipRect.y -= ClipRect.y - ImageRect.h;
	}
}

void Object::move() {
	if (xfactor) {
	  if (xspeed) {
	  	xcspeed = LinearInterp(xcspeed, xspeed, xfactor);
	  }
	}
	else {
	  xcspeed = xspeed;
	}
	
	// if it's close enough to zero, zero out the x speed
	if (! xspeed && xcspeed) {
	  xcspeed *= csub(1.0, DEACC);
	  if ( tabs(xcspeed) < 0.6f ) {
	  	xcspeed = 0;
	  	xfrac = 0;
	  }
	}
	
	/* return (1.0f / fps) * (acc + 1);
	*/
	constexpr float _grvdt = (1.0f / FPS) * 17;
	
	if (yspeed < 0) {
	  ycspeed = yspeed;
	}
	else {
	  ycspeed = Approach(ycspeed, yspeed, _grvdt);
	}
	
	if (yspeed < 8.0f) {
	  yspeed = Approach(yspeed, 8.0f, _grvdt);
	}
	
	// update x position based on speed
	if (xcspeed) {
	  float temp;
	  xfrac += xcspeed;
	  xfrac = std::modf(xfrac, &temp);
	  DestRect.x += (int) temp;
	}
	
	if (collfunc_lr) {
  	  TempRect.x = DestRect.x + hitbox.x;
  	  TempRect.y = DestRect.y + hitbox.y;
  	  TempRect.w = hitbox.w;
  	  TempRect.h = hitbox.h;
	  collfunc_lr(&DestRect, TempRect, xcspeed);
	}
	
	// update y position based on speed
	int iSteps = 0;
	uint8_t uiWhichFunc = count > 8;
	uint8_t uiFlags;
	
	if (ycspeed) {
	  float temp;
	  yfrac += ycspeed;
	  yfrac = std::modf(yfrac, &temp);
	  iSteps = (int) temp;
	}
	
	if (collfunc_ud[uiWhichFunc]) {
	  TempRect.x = DestRect.x + hitbox.x;
	  TempRect.y = DestRect.y + hitbox.y;
	  TempRect.w = hitbox.w;
	  TempRect.h = hitbox.h;
	  uiFlags = collfunc_ud[uiWhichFunc](&DestRect, TempRect, iSteps);
	  if (uiFlags == TILEFLAG_SOLID || uiFlags == TILEFLAG_SLOPE) {
	  	CurrentSprite->SetColl(M_COLL_DOWN);
	  }
	}
	else {
	  DestRect.y += iSteps;
	}
}

void Object::blit(const SDL_Rect* vpt) {
	if (renderer && Loaded) {
	  TempRect = DestRect;
	  if (vpt) {
	  	TempRect.x -= vpt->x;
	  	TempRect.y -= vpt->y;
	  }
	  clip_frame();
	  
	  if (FlipFlags.use == SDL_USEFLIPOBJ_YES) {
	  	SDL_RenderCopyEx(renderer, Texture, &ClipRect, &TempRect, FlipFlags.angle, FlipFlags.center, FlipFlags.flip);
	  }
	  else {
	  	SDL_RenderCopy(renderer, Texture, &ClipRect, &TempRect);
	  }
	}
}

SDL_Rect& Object::add_hitbox() {
	TempRect.x = DestRect.x + hitbox.x;
	TempRect.y = DestRect.y + hitbox.y;
	TempRect.w = hitbox.w;
	TempRect.h = hitbox.h;
	
return TempRect;
}
