#ifndef SPRITE_ROUTER_HPP
#define SPRITE_ROUTER_HPP

#include <custom/object_class.hpp>
#include <cstring>

#include "sprite_def.h"
#include "program.hpp"
#include "game.hpp"
#include "global_attributes.hpp"

// bitmasks of Sprite::actv's value
typedef enum {
	Sprite_IsActive		= 0x01,
	Sprite_ExtraBit1	= 0x10,
	Sprite_ExtraBit2	= 0x20
} SpriteBits;

// If defined, means the sprite is touching a solid tile below itself
#define GROUND_DETECTION_BOTTOM		0x01

// Currently unused. Dictates how many sprites are actually coded in the game
#define NUM_CODED_SPRITES		5

/* Referenced in sprite_router.cc with an array of function pointers; in sprite_helpers.cc with an array of
integers; and in texture_packs.cc, with one array of TexturePack*s, and one array of integers. */
#define NUMBER_OF_UNIQUE_SPRITES	6

#endif /* SPRITE_ROUTER_HPP */
