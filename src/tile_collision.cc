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
#include <lvalue_rvalue_pointers.hpp>

// functions that reference memory directly
#include "headers/memory.hpp"

// logging functions
#include "headers/log.hpp"

// how many tiles are currently defined in the game
#define NUMBER_OF_CODED_TILES	56

// static function declarations //
static SDL_Rect newTileRect(int, int);

inline void incPosY(Point<int>& collpoint, int& y, int val) {
	collpoint[1] += val;
	y += val;
}

// external variables //

// sprite having its tile collision handled
Sprite* CurrentSprite = nullptr;

// static variables //
// rectangular hitbox of a portion of a sprite
static generic_class<SDL_Rect> spriteRect;

// rectangular hitbox for a tile
static generic_class<SDL_Rect> tileRect(newTileRect(0, 0));

// the location of a tile as a point
static Point<int> atile;

// type declaration
typedef void (* FPV_TileFunction)(int*, float*, const SDL_Rect&, const Point<int>&, const ENTRY_POINT);

// declarations to a series of functions only visible in this source file
#include "tilecode/_tile_func_decl.cxx"

// slope tables
#include "tilecode/_tables.cxx"

// aliases to tile functions
#define Tile001 Ledge
#define Tile025 SolidTile
#define Tile008 Slope
#define Tile009 Slope
#define Tile010 Slope
#define Tile011 Slope
#define Tile026 OneWayDown
#define Tile027 OneWayDown

// pointers to said functions
constexpr FPV_TileFunction TileFunctions[NUMBER_OF_CODED_TILES] = {
	Tile000, Tile001, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile008, Tile009, Tile010, Tile011,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile025, Tile026, Tile027,
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
void TileLocationInRect(SDL_Rect& rect, int x, int y) {
	rect.x = x - (x % TILE_WIDTH);
	rect.y = y - (y % TILE_HEIGHT);
}

bool IntersectPointAndTile(const Point<int>& point, int pw, int ph, int x, int y, SDL_Rect& result) {
	SDL_Rect A = {point.x(), point.y(), pw, ph};
	SDL_Rect B = {x, y, TILE_WIDTH, TILE_HEIGHT};
	
return (bool) SDL_IntersectRect( &A, &B, &result );
}

void SpriteTileCollision(int* iXY, float* fDeltasCGSpeeds, const SDL_Rect& hitbox) {
	using level::VTiles;
	static uint16_t uiBotTile = 0;
	int entry;
	uint16_t uiCurTile;
	
	// entry point into every function
	entry = CurrentSprite->GetID() == 255 ? ENTRY_PLAYER_DOWN : ENTRY_SPRITE_DOWN;
	
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
	  // hide the slope id flag in the sprite's X position
	  int iSlpID = TILEFLAG_SlopeID(VTiles[uiCurTile]);
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
	
	// process sprite collision with a tile to the left or right of the sprite
	for (char leftright = 0; leftright < 2; ++leftright) {
	  // entry point switching between left and right
	  ++entry;
	  
	  // skip if ceiling collision is true
	  if (CurrentSprite->GetColl(M_COLL_UP)) break;
	  
	  // approximate tile height of the hitbox
	  const int iSprH = CurrentSprite->CollTileHeight();
	  
	  // counter: reaches zero or less when whole sprite has been checked
	  int iCnt = hitbox.h;
	  // height of a division of the sprite hitbox
	  int iSubDivHeight;
	  
	  // set sprite rectangle
	  spriteRect->x = hitbox.x;
	  spriteRect->y = hitbox.y;
	  spriteRect->w = hitbox.w;
	  spriteRect->h = hitbox.h / iSprH;
	  
	  // height of a subdivision of the sprite rectangle
	  iSubDivHeight = spriteRect->h;
	  // location of a tile in rect
	  TileLocationInRect(tileRect.get(), spriteRect->x + (leftright ? (spriteRect->w - 1) : 0), spriteRect->y);
	  
	  // process each subdivision of the sprite hitbox
	  for (int x = 0; x < iSprH; ++x) {
	  	SDL_Rect result;
	  	
	  	// measure intersection of the two rects
	  	if ( SDL_IntersectRect(tileRect.getp(), spriteRect.getp(), &result) ) {
	  	  // tile point
	  	  atile[0] = tileRect->x;
	  	  atile[1] = tileRect->y;
	  	  // collision point
	  	  collpoint[0] = result.x +  (leftright ? result.w - 1 : 0);
	  	  collpoint[1] = result.y;
	  	  
	  	  // tile number
	  	  uiCurTile = GetTile2(collpoint);
	  	  // function
	  	  TILEFUNC(entry);
	  	}
	  	
	  	// decrease counter
	  	iCnt -= iSubDivHeight;
	  	// lower tile rect Y
	  	tileRect->y += iSubDivHeight;
	  	// lower sprite rect Y
	  	spriteRect->y += iSubDivHeight;
	  	
	  	// negative counter
	  	if (iCnt < 0) {
	  	  tileRect->y   += iCnt;
	  	  spriteRect->y += iCnt;
	  	}
	  } // end for (int x = 0; x < iSprH; ++x)
	} // end for (char leftright = 0; leftright < 2; ++leftright)
	
	
	
	
	
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

#include "tilecode/_tile_collision_func.cxx"
