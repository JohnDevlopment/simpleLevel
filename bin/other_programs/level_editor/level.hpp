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

// tile custom flags
#define TILEFLAG_LAYER      0x0001
#define WhichLayer(n)       (n & TILEFLAG_LAYER)

#define BackTilemap     0
#define FrontTilemap    1
#define Shader          2

// bit flags
#define FLAGS_CHK_LAYER 0x00000001
#define FLAGS_FILL_ST   0x00000002
#define FLAGS_FILL_END  0x00000004
#define FLAGS_FILL_NULL 0x00000008
#define FLAGS_FILL_BOTH (FLAGS_FILL_ST|FLAGS_FILL_END)
#define FLAGS_TILE_SEL  0x00000010
#define FLAGS_CAMERA    0x00000100

#define Flag_Layer(n)   (n & FLAGS_CHK_LAYER)

namespace level {
    extern generic_class<LevelHeader>   header;
    extern std::vector<Tile>            vtiles;
    extern SDL_Texture*                 layers[]; // 3 count
    extern SDL_Texture*                 tileset;
    extern generic_class<Image>         bg;
    extern Bitfield<uint32_t>           flags;
    extern std::vector<unsigned int>    tilenums; // gtile, ctile, fill_start, fill_end

    int build(const char* file, const Program& pg);

    // TODO (john#2#): add description; remove second argument
    void changeLine(SDL_Renderer* ren);

    // TODO (john#2#): add description
    void changeTile(SDL_Renderer* ren, uint16_t dtile);
}

/* Returns the numeric identifier of a cell in a table that's
   [roww] tiles wide, and is located at (x,y). */
uint16_t gettile(int roww, int x, int y);

/* Returns an XY coordinate of the a tile according to the width
   of a table, as specified by tbw (in pixels). */
Point<int> getXY(int tbw, uint16_t tile);

// prints the currently selected tile
void printTile();

#endif // LEVEL_HPP_INCLUDED
