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
typedef int (*TileFunction)(SDL_Rect* loc, SDL_Rect* col, const ENTRY_POINT entry);

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

int SpriteTileCollision_LeftRight(SDL_Rect* loc, SDL_Rect& collbox, int steps) {
	using level::Tiles;
	
	uint8_t uiFlags = 0;
	Point<int> myPoint;
	int iDiff;
	Tile tile;
	
	// left side
	myPoint[0] = collbox.x;
	myPoint[1] = collbox.y + static_cast<int>(0.75f * collbox.h);
	
	tile = Tiles[GetTile2(myPoint)];
	if ( TILEFLAG_IsSolid(tile) ) {
	  iDiff = TILE_WIDTH - (myPoint.x() % TILE_WIDTH);
	  loc->x += iDiff;
	  collbox.x += iDiff;
	}
	
return static_cast<int>(uiFlags);
}

int SpriteTileCollisionOneStep(SDL_Rect* loc, SDL_Rect& collbox, int steps) {
	using level::Tiles;
//	using level::TileLocations;
	
	uint8_t uiFlags = 0;
	Tile* curTile;
	
	// tile underneath the sprite
{
	const int iHw = collbox.w / 2;
	Point<int> collbotm_left(collbox.x + iHw / 2, collbox.y + collbox.h);
	uint16_t uiTile1, uiTile2;
	
	uiTile1 = GetTile2(collbotm_left);
	uiTile2 = GetTile(collbox.x + iHw * 1.5f, collbotm_left.y());
	
	// select tile underneath the left point
	curTile = &Tiles[uiTile1];
	if (TILEFLAG_IsSolid(*curTile)) {
	  uiFlags = _funcs[curTile->codeID](loc, &collbox, ENTRY_PLAYER_DOWN);
	}
	
	// select tile underneath the right point
	curTile = &Tiles[uiTile2];
	if (TILEFLAG_IsSolid(*curTile) && uiTile1 != uiTile2) {
	  uiFlags |= _funcs[curTile->codeID](loc, &collbox, ENTRY_PLAYER_DOWN);
	}
}
	
	// tile above the sprite
{
	Point<int> colltop(collbox.x + collbox.w / 2, collbox.y);
	
	curTile = &Tiles[GetTile2(colltop)];
	if (TILEFLAG_IsSolid(*curTile)) {
	  uiFlags = _funcs[curTile->codeID](loc, &collbox, ENTRY_PLAYER_UP);
	}
}
	
	// move player
	if (! (uiFlags & TILEFLAG_SOLID)) {
	  loc->y += steps;
	  collbox.y += steps;
	}
	
return static_cast<int>(uiFlags);
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
