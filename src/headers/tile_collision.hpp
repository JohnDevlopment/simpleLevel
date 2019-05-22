#ifndef TILE_COLLISION_HPP
#define TILE_COLLISION_HPP

// includes <pointvector>
#include "line_point.h"

// level loading library
#include <levelLoad.hpp>

// SDL headers
#include "sdl_incs.h"

// function attributes
#include "global_attributes.hpp"

// Player class
#include "player_def.h"

enum ENTRY_POINT {
	ENTRY_PLAYER_DOWN,
	ENTRY_PLAYER_UP,
	ENTRY_PLAYER_LEFT,
	ENTRY_PLAYER_RIGHT,
	ENTRY_PLAYER_INSIDE,
	ENTRY_SPRITE_DOWN,
	ENTRY_SPRITE_UP,
	ENTRY_SPRITE_LEFT,
	ENTRY_SPRITE_RIGHT,
	ENTRY_SPRITE_INSIDE
};

// sprite being processed by the tile collision code
extern Sprite* CurrentSprite;

// calculate collision for a sprite
void SpriteTileCollision(int* iXY, float* fSpeedsDeltas, const SDL_Rect& hitbox);

// using the X and Y coordinates, along with the size of the level, get the number of the tile located at that position
uint16_t GetTile(int x, int y) PURE;

// get the X and Y position of a tile
Point<int> GetXY(uint16_t uiTile, int iRowTiles);

// get the location of a the origin of a tile located at (x,y)
Point<int> TileLocation(int x, int y);
void TileLocationInRect(SDL_Rect& rect, int x, int y);

#endif /* TILE_COLLISION_HPP */