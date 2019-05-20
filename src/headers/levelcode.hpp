#ifndef LEVELCODE_HPP
#define LEVELCODE_HPP

// standard includes
#include "stdinc.h"

// SDL2 includes
#include "sdl_incs.h"

// container for a single object
#include <custom/generic_class.hpp>

// levelLoad library header
#include <custom/levelLoad.hpp>

// definition of the Player class; also includes sprite_def.h
#include "player_def.h"

// std::array definition
#include <array>

// program window information
#include "program.hpp"

// compiler features: function attributes
#include "global_attributes.hpp"

// gamemode definition and functions
#include "gamemode.hpp"

// enumerations for level::levelEvents
#include "gm_level_defs.h"

// ...
#define NUM_SLOPES                 4

// custom bit flags for each tile
#define TILEFLAG_LAYER             0x0001
#define TILEFLAG_SLOPE             0x0002
#define TILEFLAG_SOLID             0x0004
#define TILEFLAG_SLOPEINV          0x0008
#define TILEFLAG_SLOPEID           0x00f0

// how many bits to shift for each flag
#define TFBITS_Slope               1
#define TFBITS_IsSolid             2
#define TFBITS_IsInvSlope          3
#define TFBITS_SlopeID             4

// macros to turn integers into flags
#define TILEFLAG_ToSlopeID(i)      (i << TFBITS_SlopeID)

// macros to isolate each tile flag
#define TILEFLAG_GetLayer(tile)    (tile.flags & TILEFLAG_LAYER)                           // layer number of tile, 0 or 1
#define TILEFLAG_IsSlope(tile)     ((tile.flags & TILEFLAG_SLOPE) >> TFBITS_Slope)         // 1 if the tile is a slope, 0 otherwise
#define TILEFLAG_IsSolid(tile)     ((tile.flags & TILEFLAG_SOLID) >> TFBITS_IsSolid)       // 1 if the slope is a solid, 0 otherwise
#define TILEFLAG_IsInvSlope(tile)  ((tile.flags & TILEFLAG_SLOPEINV) >> TFBITS_IsInvSlope) // 1 if the slope if facing right, 0 otherwise
#define TILEFLAG_SlopeID(tile)     ((tile.flags & TILEFLAG_SLOPEID) >> TFBITS_SlopeID)     // slope table lookup number

// misc defines
#define MAX_FALL_DISTANCE          28              // maxium number of pixels the player can fall down before taking damage
#define TILES_PER_SCREEN           16              // how many tiles per screen there are (screen = division of the level)

// player blocked status (0000 0000 0000 0000 0000 0000 0000 0000)
//                                                           udlr
#define BLOCKED_UP                 0x00000008
#define BLOCKED_DOWN               0x00000004
#define BLOCKED_LEFT               0x00000002
#define BLOCKED_RIGHT              0x00000001
#define BLOCKED_RESET              0xfffffff0

// tilemaps
#define TILEMAP_BACK               level::Tilemap[0]
#define TILEMAP_FRONT              level::Tilemap[1]
#define TILEMAP_BACKUP1            level::Tilemap[2]
#define TILEMAP_BACKUP2            level::Tilemap[3]

namespace level {
	// level header: width, height, background ID, tileset ID, & music ID
	extern generic_class<LevelHeader> Header;
	
	// every tile in the level is defined in this array
	extern std::vector<Tile> VTiles;
	
	// four pointers to SDL_Texure: they all represent different layers of the tilemap and their respective backups
	// 0 = back tilemap, 1 = front tilemap, 2 = backup of (0), 3 = backup of (1)
	extern SDL_Texture* Tilemap[];
	
	// the player object
	extern Player* ThePlayer;
	
	// the current absolute position of the background within the level
	extern int bgx;
	
	// screen exits
	extern std::vector<std::vector<char>> screenExits;
	
	// TODO add description
	extern uint8_t levelEvents;
	
	// TODO add description
	void correctBackground(const SDL_Rect* srcCam, const SDL_Rect* dstCam);
	
	/* Loads a .dat file indicatd by the first argument and returns zero if
	   it successfully builds a level based off what the file says. If it returns
	   zero, the function has successfully read the file and allocated memory
	   for two textures located in the 'level' namespace. Moreover, a vector
	   container is filled with the tilemap data read from the file.
	*/
	int load(std::string file, const PROGRAM& program);


	void update(GameMode* gm, char entry = 0);
	void unload();
	void exit(int x, int y);
}

#endif /* LEVELCODE_HPP */
