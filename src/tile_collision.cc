// global game data
#include "headers/game.hpp"

#include "headers/gamemode.hpp"
#include "headers/tile_collision.hpp"
#include "headers/levelcode.hpp"
#include "headers/camera.hpp"
#include "headers/math.hpp"
#include "headers/tmath.hpp"
#include <lvalue_rvalue_pointers.hpp>
#include "headers/memory.hpp"
#include "headers/log.hpp"
#include "headers/object/object.hpp"

// how many tiles are currently defined in the game
//#define NUMBER_OF_CODED_TILES	56

// static function declarations //
////static SDL_Rect newTileRect(int, int);

inline void incPosY(Point<int>& collpoint, int& y, int val) {
	collpoint[1] += val;
	y += val;
}

// external variables //

// sprite having its tile collision handled
Sprite* CurrentSprite = nullptr;

// static variables //
static Point<int> TilePoint;

#include "tilecode/_tile_collision_func.cxx"
#include "tilecode/_tables.cxx"

// aliases to tile functions
#define Tile000 EmptyTile
#define Tile001 Ledge
#define Tile024 SlopeJoint

// pointers to said functions
typedef int (*TileFunction)(Rect* loc, Rect* col, const ENTRY_POINT entry);

constexpr TileFunction _funcs[NUMBER_OF_CODED_TILES] = {
	Tile000, Tile001, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile024, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000,
	Tile000, Tile000, Tile000, Tile000
};

// inline functions //
INLINE uint16_t GetTile2(const Point<int>& xy) {
	return GetTile(xy.x(), xy.y());
}

INLINE uint16_t GetTile3(const SDL_Rect& rect) {
	return GetTile(rect.x, rect.y);
}

INLINE void TileLocationInPoint(Point<int>& point, const Point<int>& ref) {
	point[0] = ref.x() - (ref.x() % TILE_WIDTH);
	point[1] = ref.y() - (ref.y() % TILE_HEIGHT);
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

int SpriteTileCollision_LeftRight(Rect* loc, Rect& collbox, int steps) {
	using level::VTiles;
	
	uint8_t uiFlags = 0;
	
	/*if (steps)*/ {
	  Point<int> point;
	  int iDiff;
	  Tile tile;
	  
	  // left side
	  point[0] = collbox.x;
	  point[1] = collbox.y + static_cast<int>(0.75f * collbox.h);
	  
	  tile = VTiles.at( GetTile2(point) );
	  if ( TILEFLAG_IsSolid(tile) ) {
	  	iDiff = TILE_WIDTH - (point.x() % TILE_WIDTH);
	  	loc->x += iDiff;
	  	collbox.x += iDiff;
	  }
	  
	  
	}
	
return static_cast<int>(uiFlags);
}

int SpriteTileCollisionOneStep(Rect* loc, Rect& collbox, int steps) {
	using level::VTiles;
	
	#ifndef NDEBUG
	constexpr int _lvl_bot = TILE_HEIGHT * 30;
	static_assert(_lvl_bot > 0, "_lvl_bot is <= zero; assertion failed");
	#endif
	
	assert(collbox.y + collbox.h < _lvl_bot);
	assert(loc != nullptr);
	
	Point<int> point;
	int iSign;
	uint8_t uiFlags;
	
	iSign = tsign(steps);
	steps = tabs(steps);
	
	while (steps-- > 0) {
	  int iDiffY;
	  uint16_t uiTile;
	  
	  point[0] = collbox.x + collbox.w / 2;
	  point[1] = collbox.y + collbox.h;
	  uiTile = GetTile2(point);
	  iDiffY = point.y() % TILE_HEIGHT;
	  
	  // if a tile is a slope:
	  const Tile& tile = VTiles[uiTile];
	  if ( TILEFLAG_IsSlope(tile) ) {
	  	const short int* iOffsets = _slope_tables[TILEFLAG_SlopeID(tile)];
	  	int iDiffX = point.x() % TILE_WIDTH;
	  	
	  	// if the slope is inverted:
	  	if ( TILEFLAG_IsInvSlope(tile) ) {
	  	  iDiffX = (TILE_WIDTH - 1) - iDiffX;
	  	}
	  	
	  	if (iDiffY > iOffsets[iDiffX]) {
	  	  short int iOff = iDiffY - iOffsets[iDiffX];
	  	  loc->y -= iOff;
	  	  collbox.y -= iOff;
	  	  uiFlags = TILEFLAG_SLOPE;
	  	}
	  }
	  
	  // if a tile is solid:
	  else if ( TILEFLAG_IsSolid(tile) ) {
	  	uiFlags = TILEFLAG_SOLID;
	  	loc->y -= iDiffY;
	  	collbox.y -= iDiffY;
	  }
	  else {
	  	uiFlags = _funcs[tile.codeID](loc, &collbox, ENTRY_PLAYER_DOWN);
	  }
	  
	  // no collision
	  if (! uiFlags) {
	  	loc->y += iSign;
	  	collbox.y += iSign;
	  }
	  else {
	  	steps = 0;
	  }
	}
	
return (int) uiFlags;
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
//SDL_Rect newTileRect(int x, int y) {
//	SDL_Rect temp = {x, y, TILE_WIDTH, TILE_HEIGHT};
//	
//return temp;
//}
