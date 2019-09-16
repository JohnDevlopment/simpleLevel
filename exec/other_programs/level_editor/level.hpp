#ifndef LEVEL_HPP_INCLUDED
#define LEVEL_HPP_INCLUDED

#include <SDL.h>
#include <levelLoad.hpp>
#include <generic_class.hpp>
#include <image_class.hpp>
#include <cstdio>
#include <pointvector>
#include <bitfield_class.hpp>
#include <arrays.hpp>

#include "program.h"
#include "camera.h"
#include "mathfunc.hpp"

// forces the compiler to inline a function, except for when the function is called indirectly
#if defined(__GNUG__) || defined(__clang__)
  #define INLINE __attribute__ ((always_inline)) static __inline__
#elif defined(_MSC_VER)
  #define INLINE __forceinline
#endif

// tile custom flags
#define TILEFLAG_LAYER      0x0001
#define WhichLayer(n)       (n & TILEFLAG_LAYER)

enum {
    BackTilemap = 0,
    FrontTilemap,
    Shader,
    SpriteTilemap
};

// bit flags
#define FLAGS_CHK_LAYER     0x00000001
#define FLAGS_FILL_ST       0x00000002
#define FLAGS_FILL_END      0x00000004
#define FLAGS_FILL_NULL     0x00000008
#define FLAGS_FILL_BOTH     (FLAGS_FILL_ST|FLAGS_FILL_END)
#define FLAGS_TILE_SEL      0x00000010
#define FLAGS_SPRITE_SEL    0x00000020
#define FLAGS_TEXT_INPUT    0x00000040
#define FLAGS_CAMERA        0x00000100

#define Flag_Layer(n)   (n & FLAGS_CHK_LAYER)

namespace level {
    // bit flags
    extern Bitfield<uint32_t> Flags;
    
    // background of the level
    extern generic_class<Image> Background;
    
    // tiles pasted onto the level
    extern SDL_Texture* Tileset;
    
    // in order: back-layer tilemap (0), shade layer (1), front-layer tilemap (2)
    extern SDL_Texture* Layers[];
    
    // keeps track of tile selections and info (gtile, ctile, fill-start, fill-end)
    extern StaticDArray<unsigned int, 4> TileNumbers;
    
    // header
    extern generic_class<LevelHeader> Header;
    
    // tiles in the level
    extern Tile* Tiles;
    
    // list of sprites
    extern std::vector<SprObj> Sprites;
    
    // sprite id number
    extern unsigned int SpriteID;
    
    /* Builds the level tilemap. The tileset, front and back tilemaps are
    created as a result of this function. */
    bool build(const char* file, const Program& pg);
}

/* Returns the numeric identifier of a cell in a table that's
   [roww] tiles wide, and is located at (x,y). */
uint16_t gettile(int roww, int x, int y);

/* Returns the index of a cell in a table array that`s
   [roww] tiles width, located at (tilex,tiley). */
INLINE uint16_t gettile_inline(int roww, int tilex, int tiley) {
    return static_cast<uint16_t>(roww * tiley + tilex);
}

/* Returns an XY coordinate of the a tile according to the width
   of a table, as specified by tbw (in pixels). */
Point<int> getXY(int tbw, uint16_t tile);

// prints the currently selected tile
void printTile();

/* changeLine: changes a line of tiles, setting their graphicID to TileNumbers[0]
   and their codeID into TileNumbers[1], then draws on a line between TileNumbers[2]
   and TileNumbers[3].
   
   changeSpriteTile: either displays or deletes a "tile" off from the sprite object
   layer. Whether it copies or deletes said tile is decided by <del>.
   
   changeTile: changes a tile, setting its graphicID into TileNumbers[0] and its
   codeID into TileNumbers[1]. NOTE: changeLine() actually just calls this function
   on each tile in a line. */
void changeLine(SDL_Renderer* ren);
void changeSpriteTile(SDL_Renderer* ren, uint16_t dtile, const bool del);
void changeTile(SDL_Renderer* ren, uint16_t dtile);

#endif // LEVEL_HPP_INCLUDED
