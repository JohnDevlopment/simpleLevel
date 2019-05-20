// global game data
#include "headers/game.hpp"

// gamemode data and functions
#include "headers/gamemode.hpp"

// tile collision code
#include "headers/tile_collision.hpp"

// level data
#include "headers/levelcode.hpp"

// camera data
#include "headers/camera.hpp"

// math functions
#include "headers/math.hpp"
#include "headers/tmath.hpp"

// references and pointers
#include <custom/lvalue_rvalue_pointers.hpp>

// functions that reference memory directly
#include "headers/memory.hpp"

// logging functions
#include "headers/log.hpp"

// how many tiles are currently defined in the game
#define NUMBER_OF_CODED_TILES	56

// static function declarations //
static SDL_Rect newTileRect(int, int);
static int getTileHeight(int);

inline void incPosY(Point<int>& collpoint, int& y, int val) {
	collpoint[1] += val;
	y += val;
}

// external variables //

// sprite having its tile collision handled
Sprite* CurrentSprite = nullptr;

// static variables //
StaticDArray<SDL_Rect,2> SpriteAndTileRects = {
	{0, 0, 0, 0},
	{0, 0, TILE_WIDTH, TILE_HEIGHT}
};

// for each number (sprite id), there is a number denoting the tile height for each

// the location of a tile
static Point<int> atile;

// rectangle for a tile
static generic_class<SDL_Rect> tileRect(newTileRect(0, 0));

// type declaration
typedef void (* FPV_TileFunction)(int*, float*, const SDL_Rect&, const Point<int>&, const ENTRY_POINT);

// declarations to a series of functions only visible in this source file
#include "tilecode/_tile_func_decl.cxx"

// slope tables
#include "tilecode/_tables.cxx"

// aliases to tile functions
#define Tile001 Ledge
#define Tile025	SolidTile
#define Tile008 Slope
#define Tile009 Slope
#define Tile010 Slope
#define Tile011 Slope

// pointers to said functions
constexpr FPV_TileFunction TileFunctions[NUMBER_OF_CODED_TILES] = {
	Tile000, Tile001, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile008, Tile009, Tile010, Tile011,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile025, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000
};

// inline functions //
inline uint16_t GetTile2(const Point<int>& xy) {
	return GetTile(xy.x(), xy.y());
}

inline uint16_t GetTile3(const SDL_Rect& rect) {
	return GetTile(rect.x, rect.y);
}

// public functions //
SDL_Rect TileLocationInRect(int x, int y) {
	SDL_Rect retval = {
	  x - (x % TILE_WIDTH),
	  y - (y % TILE_HEIGHT),
	  TILE_WIDTH,
	  TILE_HEIGHT
	};
	
return retval;
}

bool IntersectPointAndTile(const Point<int>& point, int pw, int ph, int x, int y, SDL_Rect& result) {
	SDL_Rect A = {point.x(), point.y(), pw, ph};
	SDL_Rect B = {x, y, TILE_WIDTH, TILE_HEIGHT};
	
return (bool) SDL_IntersectRect( &A, &B, &result );
}

//bool IntersectPointAndTile(const Point<int>& point, int pw, int ph, int x, int y, SDL_Rect& result) {
//	int Amin, Amax, Bmin, Bmax;
//	
//	// point fields are zero
//	if (point == 0) {
//	  ERROR(IntersectPointAndTile, point);
//	  return false;
//	}
//	
//	// width or height = 0
//	if (pw == 0)  {
//	  ERROR(IntersectPointAndTile, pw);
//	  return false;
//	}
//	else if (ph == 0) {
//	  ERROR(IntersectPointAndTile, ph);
//	  return false;
//	}
//	
//	// if x or y are invalid
//	if (x < 0 || y < 0) {
//	  result[0] = 0;
//	  result[1] = 0;
//	  return false;
//	}
//	
//	// horizontal overlap
//	Amin = point.x();
//	Amax = point.x() + pw;
//	Bmin = x;
//	Bmax = x + TILE_WIDTH;
//	
//	if (Bmin > Amin)
//	  Amin = Bmin;
//	result.x = Amin;
//	if (Bmax < Amax)
//	  Amax = Bmax;
//	result.w = Amax - Amin;
//	
//	// vertical overlap
//	Amin = point.y();
//	Amax = point.y() + ph;
//	Bmin = y;
//	Bmax = y + TILE_HEIGHT;
//	
//	
//}

void SpriteTileCollision(int* iXY, float* fDeltasCGSpeeds, const SDL_Rect& hitbox) {
	using level::VTiles;
	static uint16_t uiBotTile = 0;
	
	// entry point into every function
	int entry = CurrentSprite->GetID() == 255 ? ENTRY_PLAYER_DOWN : ENTRY_SPRITE_DOWN;
	
	// number of the current tile being looked at
	uint16_t uiCurTile;
	
	// camera edge detection //
	
	// if the sprite is left of the camera...
	if (hitbox.x < CAM_CAMERA.x) {
	  // push the sprite forward by subtracting its X value by the difference between it and the camera edge
	  iXY[0] -= hitbox.x - CAM_CAMERA.x;
	}
	
	// if the sprite is right of the camera
	else if ((hitbox.x + hitbox.w) > (CAM_CAMERA.x + CAM_CAMERA.w)) {
	  // pull the sprite back by subtracting its X value by the positive difference between it and the camera edge
	  iXY[0] -= (hitbox.x + hitbox.w) - (CAM_CAMERA.x + CAM_CAMERA.w);
	}
	
	// start tile collision detection //
	
	// shorthand for the tile function call
	#define TILEFUNC(entry) TileFunctions[VTiles[uiCurTile].codeID](\
	                                    iXY,\
	                                    fDeltasCGSpeeds,\
	                                    hitbox,\
	                                    collpoint,\
	                                    (ENTRY_POINT) entry\
	                                                               )
	
	// shorthand for getting the location of a tile underneath the point
	#define WHERETILE(point) TileLocation(point.x(), point.y())
	
	// the bottom edge of the hitbox
	Point<int> collpoint(hitbox.x + hitbox.w / 2, hitbox.y + hitbox.h);
	
	// location of the tile under the bottom edge of the sprite
	atile = WHERETILE(collpoint);
	
	// tile number
	uiCurTile = uiBotTile = GetTile2(collpoint);
	
	// positive Y speed
	if (fDeltasCGSpeeds[3] > 0) {
	  const Tile& tile = VTiles[uiCurTile];
	  
	  // hide the slope id flag in the sprite's X position
	  int iSlpID = TILEFLAG_SlopeID(tile);
	  iXY[0] |= iSlpID << 24; // 0x000000ff -> 0xff000000
	  
	  // collision
	  TILEFUNC(entry);
	}
	
	// the top edge of the hitbox
	collpoint[1] -= hitbox.h;
	
	// location of that tile
	atile = WHERETILE(collpoint);
	
	// tile number
	uiCurTile = GetTile2(collpoint);
	
	// change entry point to ENTRY_*_UP (* = PLAYER/SPRITE)
	++entry;
	TILEFUNC(entry);
	
	
	
	
	
	// collision with left and right walls; done with boxes instead of points //
	++entry; // change entry point to ENTRY_*_LEFT (PLAYER/SPRITE)
	const int iSprH = getTileHeight(CurrentSprite->GetID()); // apprx tile height of sprite
	
	
	
	
	
	
	
	return;
	
	
	// calculate player fall distance if its down-blocked state is not set
	if (CurrentSprite->GetID() == 255) {
	  Player* player = (Player*) CurrentSprite;
	  
	  // if the player is touching the ground, then check if the fall distance has exceeded MAX_FALL_DISTANCE
	  if (player->GetColl(M_COLL_DOWN)) {
	  	// if so, hurt the player
	  	if (player->m_FallDistance == MAX_FALL_DISTANCE) {
	  	  player->Hurt();
	  	}
	  	
	  	// reset counter
	  	player->m_FallDistance = 0;
	  }
	  
	  // if not touching the ground, and Y speed is beyond a certain speed, increment the counter
	  else if (fDeltasCGSpeeds[3] > 3.0f) {
	  	player->m_FallDistance = Inc(player->m_FallDistance, 1, MAX_FALL_DISTANCE);
	  }
	}
}

Point<int> GetXY(uint16_t uiTile, int iRowTiles) {
	Point<int> retval;
	
	// size of a single row in pixels
	iRowTiles *= TILE_WIDTH;
	
	// X offset of the tile
	int x = TILE_WIDTH * uiTile;
	retval[0] = x - iRowTiles * (x / iRowTiles);
	
	// Y offset of the tile
	retval[1] = x / iRowTiles * TILE_HEIGHT;
	
return retval;
}

uint16_t GetTile(int x, int y) {
	// convert pixel coordinates to tiles
	if (x)
	  x /= TILE_WIDTH;

	if (y)
	  y /= TILE_HEIGHT;

	// for each row, add its worth of tiles to the X coordinate
	x += level::Header->width * y;

return static_cast<uint16_t>(x);
}

Point<int> TileLocation(int x, int y) {
	Point<int> retval;
	retval[0] = x - (x % TILE_WIDTH);
	retval[1] = y - (y % TILE_HEIGHT);

return retval;
}

// private functions //
SDL_Rect newTileRect(int x, int y) {
	SDL_Rect temp = {x, y, TILE_WIDTH, TILE_HEIGHT};
	
return temp;
}

int getTileHeight(int id) {
	int retval;
	
	switch (id) {
	  default:
	  	retval = 1;
	  	break;
	  
	  case 1:
	  case 2:
	  case 255:
	  	retval = 2;
	}
	
return retval;
}

#include "tilecode/_tile_collision_func.cxx"
