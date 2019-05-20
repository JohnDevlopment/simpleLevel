#ifndef SPRITE_HELPERS
#define SPRITE_HELPERS

#include "sprite_router.hpp"
#include "global_attributes.hpp"

// affect player status
//void PlayerBounce(int yinter); // causes the player to "bounce" off the sprite it just hit
//void HurtPlayer(size_t sfx = 4); // decreases the player hp and kills it if it's reduced to zero
//void KillPlayer(char animation, size_t* sfx = nullptr); // kill the player: pass 0 for no animation, 1 or animation

/*
The following functions set various flags and check various things for the sprite, sometimes in relation to
the player. PlayerSpriteColl checks for the collision between the player and the sprite selected. If it returns
true there is a collision, else it is false. If given the address to a SDL_Rect, PlayerSpriteColl will fill it
with information on the rectangular intersection that has occurred.

WhatToFacePlayer returns 0 if the player is to the right of the sprite and 1 if to the left. The CHAR pointer
is used to return the direction the sprite is already facing; what meaning it has is specific to the sprite
itself. The last two pointers are used to dereference integers and give them the difference between the X offsets
of the sprite and player, and the difference between the Y offsets, respectively.

SpriteOffscreen checks if the sprite is outside the camera and it also checks if it is outside the level's
boundaries. The sprite's offscreen and deletion flags are automatically set by this function. Additionally,
SpriteOffscreen returns a non-zero value if the sprite is confirmed to be either offscreen or out of level
boundaries. The second argument is used to determine how far off the screen (or level) the sprite may be considered
out of bounds. Sprites that leave out of the level's boundaries are deleted automatically.
*/
//bool PlayerSpriteColl(Sprite* spr, SDL_Rect* rect) HOT;
//int WhatToFacePlayer(const Sprite* spr, char* dir, int* xdist = nullptr, int* ydist = nullptr);
//int SpriteOffscreen(Sprite* spr, const uint8_t entry) HOT;

// flips the direction of the sprite
//void FlipDirection(Object& obj);

#endif /* SPRITE_HELPERS */
