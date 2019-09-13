/*
 * tile_collision.hpp
 * This file is part of simpleLevel
 *
 * Copyright (C) 2019 - John
 *
 * simpleLevel is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * simpleLevel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with simpleLevel. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

/* tile_collision.hpp
   This file belongs under the project
   simpleLevel.
   Several functions here are for locating tiles within the level
   or pin-pointing their exact coordinates.
*/

#ifndef TILE_COLLISION_HPP
#define TILE_COLLISION_HPP

// includes <pointvector>
////#include "line_point.h"

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

// using the X and Y coordinates, along with the size of the level, get the number of the tile located at that position
uint16_t GetTile(int x, int y);

// get the X and Y position of a tile
Point<int> GetXY(uint16_t uiTile, int iRowTiles);

// get the location of a the origin of a tile located at (x,y)
Point<int> TileLocation(int x, int y);
void TileLocationInRect(SDL_Rect& rect, int x, int y);

// sprite-to-tile collision
int PlayerTileCollisionUpDown(SDL_Rect* loc, SDL_Rect& collbox, int steps);
int SpriteTileCollision_LeftRight(SDL_Rect* loc, SDL_Rect& collbox, int steps);

#endif /* TILE_COLLISION_HPP */
