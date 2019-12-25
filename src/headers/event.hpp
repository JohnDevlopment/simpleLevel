#ifndef EVENT_HPP
#define EVENT_HPP

#include "stdinc.h"
#include "sdl_incs.h"
#include "pdgamemode.h"

/* Process events from the event struct. This should be called in the middle of a while loop that's calling SDL_PollEvent of the event struct.
   Pass a gamemode struct in case you want events that could potentially affect the gamemode routine or actually change the gamemode.
*/
int Event_Main(SDL_Event& event, GameMode* const gamemode, const short int flag);

#endif /* EVENT_HPP */
