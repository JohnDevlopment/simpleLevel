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
#include "triggers.hpp"
#include "script.hpp"

#include "log.hpp"
#include "math.hpp"
#include "gm_level.hpp"
#include "lvalue_rvalue_pointers.hpp"

using namespace std;
using game::Flags;
using level::update;

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

////////////////////////// gm_level MAIN routine /////////////////////////
// search code: LEVEL_MAIN
int _gm_level_main(GameMode* const gm, const PROGRAM& program) {
	PDTexture* blackScreen = (PDTexture*) gm->data;
	
	// update the level
	level::update(gm, program.renderer);
	
	// render the black screen if the right flags are set
	if (Flags.mask(FADING))
	  blackScreen->Blit();
	
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
	// unload the level
	level::unload();
	
	// reset the background color
	JColor black = {0, 0, 0, 255};
	video::SetBgColor(black);
	
	// clean up the level data
	if (Flags.mask(LEVEL_CLEANUP)) {
	  Sound_FreeMUS(LevelMusic);
	  LevelMusic = nullptr;
	  black.red |= LEVEL_CLEANUP;
	}
	black.red |= QUITGAME;
	Flags.unset(black.red);
	
	GM_ClearData(gm);
	
return 0;
}

/////////////////////// gm_level INIT routine //////////////////////////
// search code: LEVEL_INIT
int _gm_level_init(GameMode* const gm, const PROGRAM& program) {
	PDTexture* blackScreen = get_pointer( RManager.LoadTexture(FADER, "images/ui/black.bmp", nullptr) );
	
	// after a certain amount of time, play the level's music
	SDL_TimerID timer = SDL_AddTimer(2000, timerCallback_playLevelMusic, nullptr);
	
	if (! timer) {
	  Log_Cerr("Failed to initialize timer in gm_level: %s\n", SDL_GetError());
	  Sound_HaltMusic();
	  Sound_FreeMUS(LevelMusic);
	  LevelMusic = nullptr;
	}
	
	game::Flags.set(FADEIN);
	blackScreen->Blit();
	
	// correct the camera if it`s not over the player
	camera::correctCamera();

	// evaluate level scripts
	PDScript::EvalScript("source scripts/levels.tcl");
	
	// save pointer
	GM_SetData(gm, blackScreen);

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
