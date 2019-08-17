// headers with macros defined
#include "headers/event.hpp"
#include "headers/event_defines.hpp"

// access to gm_menu
#include "headers/gm_menu.hpp"

// access to gm_level
#include "headers/gm_level.hpp"

// access to global program data
#include "headers/game.hpp"

int Event_Main(SDL_Event& event, GameMode* const gamemode, const short int flag) {
	using game::Flags;
	
	uint8_t index = 0;
	int retval = 0;
	
	/* Pointer to a function that returns int and accepts an l-value reference
	   to SDL_Event and a pointer to GameMode. */ 
//	typedef int (* FPI_EventFunc)(SDL_Event&, GameMode* const);
	
	/* Pointer to a function that returns int and accepts SDL_Event&, a pointer
	   to GameMode, and an unsigned 8-bit integer. Said integer expresses
	   whether the event being processed is a button-down or button-release
	   input. */
	typedef int (* FPI_MouseButtonFunc)(SDL_Event&, GameMode* const, uint8_t);
	
	/* Pointer to a function that returns int and accepts SDL_Event&, a GameMode
	   pointer, and an unsigned 8-bit integer. */
	typedef int (* FPI_KeyboardFunc)(SDL_Event&, GameMode* const, uint8_t);
	
	// bit masks that control which types of input are processed in each game mode
	static const uint8_t event_masks[NUMBER_OF_GAMEMODES] = {
	  EVENT_MASK_NOEVENTS,
	  EVENT_MASK_MOUSEDOWN,
	  EVENT_MASK_NOEVENTS,
	  EVENT_MASK_NOEVENTS
	};
	
	// key press events
	static const FPI_KeyboardFunc keyboard_func[NUMBER_OF_GAMEMODES] = {
	  nullptr,
	  nullptr,
	  nullptr,
	  gm_level_keyboard
	};
	
	// mouse motion events (currently disabled)
//	static const FPI_EventFunc mousemove_func[NUMBER_OF_GAMEMODES] = {
//	  nullptr,
//	  nullptr,
//	  nullptr,
//	  nullptr
//	};
	
	// mouse button functions
	static const FPI_MouseButtonFunc mousebutton_func[NUMBER_OF_GAMEMODES] = {
	  nullptr,
	  gm_menu_event_mousebutton,
	  nullptr,
	  nullptr
	};
	
	// if this flag is set, ALL input is ignored
	if (Flags.mask(DISABLE_INPUT))
	  return 0;
	
	/* If the integer argument is non-zero, the gamemode's corresponding keyboard function is called
	   with an argument that explicitly calls for the use of keyboard states; all this provided
	   the pointer is not NULL. */
	if (flag && keyboard_func[gamemode->gm_cur] != nullptr) {
	  return keyboard_func[gamemode->gm_cur](event, gamemode, EVENT_USE_KEYSTATES);
	}
	
	// branch to different event functions
	switch (event.type) {
	  default: break;
	  
	  case SDL_KEYDOWN:
	  	index = 1;
	  case SDL_KEYUP:
	  	if (! index) {
	  	  index = 2;
	  	}
	  	if ( MASKKEYS(event_masks[gamemode->gm_cur]) ) {
	  	  const uint8_t masks[] = { 0, EVENT_MASK_KEYDOWN, EVENT_MASK_KEYUP };
	  	  retval = keyboard_func[gamemode->gm_cur](event, gamemode, masks[index]);
	  	}
	  	break;
	  
	  case SDL_MOUSEBUTTONUP:
	  	index = 1;
	  case SDL_MOUSEBUTTONDOWN:
	  	if (! index) {
	  	  index = 2;
	  	}
	  	if ( MASKBUTTONS(event_masks[gamemode->gm_cur]) ) {
	  	  const uint8_t masks[] = { 0, EVENT_MASK_MOUSEUP, EVENT_MASK_MOUSEDOWN };
	  	  retval = mousebutton_func[gamemode->gm_cur](event, gamemode, masks[index]);
	  	}
	  	break;
	  
//	  case SDL_MOUSEMOTION:
//	  	if (event_masks[gamemode->gm_cur] & EVENT_MASK_MOUSEMOVE) {
//	  	  retval = mousemove_func[gamemode->gm_cur](event, gamemode);
//	  	}
//	  	break;
	}
	
return retval;
}
