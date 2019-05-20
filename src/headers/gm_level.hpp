#ifndef GM_LEVEL_HPP
#define GM_LEVEL_HPP

#include <iostream>

// gamemode information
#include "gamemode.hpp"

// game screen and renderer information
#include "program.hpp"

// event triggers
#include "triggers.hpp"

/* TODO ARCHIVE -- Bug found in tile collision code. Set sprite's position to 234,139 and shift its Y position up toward
   the top. When it reaches terminal velocity (as seen in the MAXYSPEED define in object_class.hpp), it will
   simply phase through the tile. The same problem has been found when the sprite is located at 38, 336.
   And the problem can also be replicated at 0,320. From what I can tell, this problem has to do with the collision
   with tiles underneath the sprite when it's falling at max velocity. This would indicate that in certain instances
   the sprite manages to go over the overlap range that's determined at the start of TileCollision_Main.
   
   UPDATE It would seem that I fixed the problem simply by capping the maximum positive Y speed at 8. The old value was
   10, which is more than the overlap distance of my sprite, which is 8. Could there be a connection?
*/
int gm_level(void* const gamemode_void, const PROGRAM& program);

int gm_level_keyboard(SDL_Event& event, GameMode* const gm, uint8_t entry_point);

#endif /* GM_LEVEL_HPP */
