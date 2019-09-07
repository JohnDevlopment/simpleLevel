/*
 * gm_level.cc
 * This file is part of simpleLevel
 *
 * Copyright (C) 2019 - John Russell
 *
 * simpleLevel is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * simpleLevel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with simpleLevel. If not, see <http://www.gnu.org/licenses/>.
 * 
 * This file provides the functions for gamemode 3, AKA. gm_level.
 * 
 * 
 * To find specific sections of the file, use your editor`s search
 * feature and input the following:
 *     INTFUNC_LEVEL - internal (private) functions for gm_level
 *     LEVEL_ROUTE   - gm_level router function
 *     LEVEL_INIT    - gm_level init function
 *     LEVEL_MAIN    - gm_level main function
 *     LEVEL_CLEAN   - gm_level cleanup function
 *     LEVEL_EVENT   - gm_level event function
 *     INTFUNC_EVENT - gm_level event router helper functions
 *     INTFUNC_KEY   - gm_level keyboard event function (normal config)
 */

#include "game.hpp"
#include "levelcode.hpp"
#include "levelinfo.hpp"
#include "event_defines.hpp"
#include "sound.hpp"
#include "tile_collision.hpp"
#include "camera.hpp"
#include "sprite_router.hpp"
#include "triggers.hpp"
#include "tcl.hpp"
#include "sprites/sprite000_def.h"
#include "private/player_data_def.h"


#define NDEBUG
#include "log.hpp"
#include "math.hpp"
#include "gm_level.hpp"

using namespace std;
using game::Flags;
using level::ThePlayer;
using level::update;

// priority list of events to process
static const uint8_t _key_sym[] = {
	GMLevel_Key_s, GMLevel_Key_Left, GMLevel_Key_Right,
	GMLevel_Key_Up, GMLevel_Key_Down, GMLevel_Key_l, GMLevel_Key_Escape,
	GMLevel_Key_Buffer
};

// list of scancodes
static StaticDArray<SDL_Scancode,10> _scancodes = {
	SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN
};

// level music
static MusData* LevelMusic = nullptr;

void _gm_level_saveMusData(MusData* mus) {
	LevelMusic = mus;
}

MusData* _gm_level_getMusData() {return LevelMusic;}

//////////////// functions to help: gm_level MAIN routine /////////////////////
// search code: INTFUNC_LEVEL
static uint32_t timerCallback_playLevelMusic(uint32_t interval, void* data) {
	
	if (Sound_PlayingMusic()) {
	  if (Sound_PausedMusic()) {
	  	Sound_ResumeMusic();
	  }
	}
	else {
	  if (LevelMusic) {
	  	Sound_PlayMusic(LevelMusic, -1);
	  }
	}

return 0;
}

/* Notes to self: firstly, uipKeyBits is a field of bits that each correspond to a key being pressed. Mask the bits
   using _key_sym_bitmasks indexed by a value of GMLevel_KeyListSym (header: gm_level_defs.h). That is how you
   can tell what inputs have been inputted. */
//static int _main_normal(GameMode* const gm, const PROGRAM& pg) {
//	
//	
//return 0;
//}

////////////////////////// gm_level MAIN routine /////////////////////////
// search code: LEVEL_MAIN
int _gm_level_main(GameMode* const gm, const PROGRAM& program) {
	// update the level
	level::update(gm, program.renderer);
	
	// render the black screen if the right flags are set
	if (Flags.mask(FADING))
	  BG_BLACK.blit();
	
	// if the user is quitting the game...
	if (Flags.mask(QUITGAME)) {
	  // activate the level's cleanup routine
	  if (! Flags.mask(LEVEL_CLEANUP)) {
	  	Flags.set(LEVEL_CLEANUP);
	  	GM_ChangeGamemode(gm, 0, 75);
	  }
	}
	
return 0;
}

//////////////////////// gm_level CLEANUP routine ///////////////////////
// search code: LEVEL_CLEAN
int _gm_level_cleanup(GameMode* const gm, const PROGRAM& program) {
	using level::unload;
	
	// unload the level
	level::unload();
	
	// reset the background color
	JColor black = {0, 0, 0, 255};
	game::bgcolor(black);
	
	// clean up the level data
{
	int iFlags = 0;
	
	if (Flags.mask(LEVEL_CLEANUP)) {
	  Sound_FreeMUS(LevelMusic);
	  LevelMusic = nullptr;
	  iFlags |= LEVEL_CLEANUP;
	}
	iFlags |= QUITGAME;
	Flags.unset(iFlags);
}
	
return 0;
}

/////////////////////// gm_level INIT routine //////////////////////////
// search code: LEVEL_INIT
int _gm_level_init(GameMode* const gm, const PROGRAM& program) {
	using game::KeySymBits;
	using game::KeySymBitsFirstFrame;
	
	// retrieve the scancode of each key defined here
	if (! _scancodes[SCAN_END]) {
	  _scancodes[SCAN_S]      = SDL_GetScancodeFromKey(SDLK_s);
	  _scancodes[SCAN_LEFT]   = SDL_GetScancodeFromKey(SDLK_LEFT);
	  _scancodes[SCAN_RIGHT]  = SDL_GetScancodeFromKey(SDLK_RIGHT);
	  _scancodes[SCAN_UP]     = SDL_GetScancodeFromKey(SDLK_UP);
	  _scancodes[SCAN_DOWN]   = SDL_GetScancodeFromKey(SDLK_DOWN);
	  _scancodes[SCAN_L]      = SDL_GetScancodeFromKey(SDLK_l);
	  _scancodes[SCAN_ESCAPE] = SDL_GetScancodeFromKey(SDLK_ESCAPE);
	  _scancodes[SCAN_Q]      = SDL_GetScancodeFromKey(SDLK_q);
	  _scancodes[SCAN_W]      = SDL_GetScancodeFromKey(SDLK_w);
	  
	  // just to make sure this statement isn't read twice
	  _scancodes[SCAN_END]    = SDL_GetScancodeFromKey(SDLK_UP);
	}

	// reset key bits
	KeySymBits = 0;
	KeySymBitsFirstFrame = 0;
	
	// after a certain amount of time, play the level's music
	SDL_TimerID timer = SDL_AddTimer(2000, timerCallback_playLevelMusic, nullptr);
	
	if (! timer) {
	  Log_Cerr("Failed to initialize timer in gm_level: %s\n", SDL_GetError());
	  Sound_HaltMusic();
	  Sound_FreeMUS(LevelMusic);
	  LevelMusic = nullptr;
	}
	
	// fade in from black
	game::Flags.set(FADEIN);
	BG_BLACK.blit();
	
	// initialize player
	level::ThePlayer->Init();
	
	// correct the camera if it`s not over the player
	camera::correctCamera();

	// evaluate level scripts
	Tcl_Eval(gInterp, "source scripts/levels.tcl");

return 2;
}

// public gamemode function //
// search code: LEVEL_ROUTE
int gm_level(void* const gamemode_void, const PROGRAM& program) {
	typedef int (* FPI_OO)(GameMode* const, const PROGRAM&);
	
	static const FPI_OO functions[] = {
	  _gm_level_main,
	  _gm_level_init,
	  _gm_level_cleanup
	};
	
	GameMode* gm = (GameMode*) gamemode_void;
	const uint16_t index = (gm->tm < 3) ? gm->tm : 0;
	
return functions[index](gm, program);
}

// internal event functions
// search code: INTFUNC_EVENT
static void eventBits(uint32_t& A, uint32_t& B, const uint32_t bit) {
//	if (! A || ! B)
//	  return;
	
	if (! ((A | B) & bit)) {
	  A |= bit;
	}
	else {
	  if (! (B & bit)) {
	  	A &= bit ^ 0xffff;
	  	B |= bit;
	  }
	}
}

// internal key event functions
// search code: INTFUNC_KEY
static int _keyboard_normal(SDL_Event& event, GameMode* const gm, const uint8_t* keystates) {
	using game::KeySymBits;
	using game::KeySymBitsFirstFrame;
	
	// iterate through a list of all key inputs
	for (uint8_t x = 0; x < GMLevel_Key_NumOfValues; ++x) {
	  switch (_key_sym[x]) {
	  	default: break;
	  	
	  	case GMLevel_Key_s:
	  	  if (keystates[_scancodes[SCAN_S]]) {
	  	  	eventBits(KeySymBitsFirstFrame, KeySymBits, 1);
	  	  }
	  	  else {
	  	  	KeySymBitsFirstFrame &= 0xFFFE;
	  	  	KeySymBits &= 0xFFFE;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Left:
	  	  if (keystates[_scancodes[SCAN_LEFT]]) {
	  	  	eventBits(KeySymBitsFirstFrame, KeySymBits, 2);
	  	  }
	  	  else {
	  	  	KeySymBitsFirstFrame &= 0xFFFD;
	  	  	KeySymBits &= 0xFFFD;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Right:
	  	  if (keystates[_scancodes[SCAN_RIGHT]]) {
	  	  	eventBits(KeySymBitsFirstFrame, KeySymBits, 4);
	  	  }
	  	  else {
	  	  	KeySymBitsFirstFrame &= 0xFFFB;
	  	  	KeySymBits &= 0xFFFB;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Up:
	  	  if (keystates[_scancodes[SCAN_UP]]) {
	  	  	eventBits(KeySymBitsFirstFrame, KeySymBits, 8);
	  	  }
	  	  else {
	  	  	KeySymBitsFirstFrame &= 0xFFF7;
	  	  	KeySymBits &= 0xFFF7;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Down:
	  	  if ( keystates[_scancodes[SCAN_DOWN]] ) {
	  	  	eventBits(KeySymBitsFirstFrame, KeySymBits, 16);
	  	  }
	  	  else {
	  	  	KeySymBitsFirstFrame &= 0xFFEF;
	  	  	KeySymBits &= 0xFFEF;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_l:
	  	  if (keystates[_scancodes[SCAN_L]]) {
	  	  	eventBits(KeySymBitsFirstFrame, KeySymBits, 32);
	  	  }
	  	  else {
	  	  	KeySymBitsFirstFrame &= 0xFFDF;
	  	  	KeySymBits &= 0xFFDF;
	  	  }
	  	  break;
	  	
	  	case GMLevel_Key_Escape:
	  	  if (keystates[_scancodes[SCAN_ESCAPE]]) {
	  	  	if (Flags.mask(FADEOUT|QUITGAME)) {
	  	  	  // freeze the player
	  	  	  ThePlayer->m_obj.xspeed = 0;
	  	  	  ThePlayer->m_obj.frame = 0;
	  	  	  
	  	  	  // fade out the music
	  	  	  Sound_FadeOutMusic(1000);
	  	  	  
	  	  	  // fade out the screen and initiate the game QUIT routine
	  	  	  Flags.set(FADEOUT|QUITGAME);
	  	  	  
	  	  	  // switch to the splash screen
	  	  	  GM_ChangeGamemode(gm, 0, 60);
	  	  	}
	  	  }
	  	  break;
	  }
	}

return 0;
}

// main gm_level keyboard input function
// search code: LEVEL_EVENT
int gm_level_keyboard(SDL_Event& event, GameMode* const gm, uint8_t entry_point) {
	/*
	Return from the function if: the function is not instructed to collect keyboard states;
	if the player is stunned from an enemy's attack; or if the game is either fading in in or out, or the
	input-disabling flag is set.
	*/
	if (entry_point != EVENT_USE_KEYSTATES || ThePlayer->m_StunTimer > 0 || Flags.mask(FADING|DISABLE_INPUT))
	  return 0;
	
return _keyboard_normal(event, gm, SDL_GetKeyboardState(nullptr));
}
