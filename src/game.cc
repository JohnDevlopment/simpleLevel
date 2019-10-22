#include "game.hpp"
#include "log.hpp"
#include "memory.hpp"
#include "string.h"

// special defines
#define NumberOfSaveBytes	3

// namespace globals
//generic_class<BitmapFont> game::Bitmap;
//Bitfield<uint16_t>	  game::Flags;
//uint8_t			  game::FrameCounter = 0;
//TexFade_t*		  game::Black = nullptr;

static SDL_Renderer* _private_renderer = nullptr;

using namespace game;



/* Includes a header that defines game::makeHash. */
#define HASHFUNC_H_IMPLEMENTATION 1
#define HASHFUNC_H_NS game
#include "hashfunc.h"
