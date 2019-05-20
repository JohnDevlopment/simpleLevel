// global game data
#include "headers/game.hpp"

// gamemode header
#include "headers/gm_menu.hpp"

// macros that define various events
#include "headers/event_defines.hpp"

// sound system
#include "headers/sound.hpp"

// logging system
#include "headers/log.hpp"

using namespace std;

using game::Bitmap;

// private function decalarations
static uint32_t timerCallback_playMenuMusic(uint32_t, void*);
static int _gm_menu_init(GameMode* const gm, const PROGRAM& program);
static int _gm_menu_blit(GameMode* const gm, const PROGRAM& program);
static int _gm_menu_cleanup(GameMode* const gm, const PROGRAM& program);

// mouse input function
int gm_menu_event_mousebutton(SDL_Event& event, GameMode* const gm, uint8_t mask) {
	using game::Flags;
	
	// no mouse input when fading screen
	if (Flags.mask(FADING)) return 0;
	
	// if the left mouse button was clicked
	if (sdl_buttonstate(event) == EVENT_PRESSED && sdl_left(event)) {
	  int x, y;
	  
	  SDL_GetMouseState(&x, &y);
	  
	  // if the mouse was clicked between (240,180) and (400,300)
	  if ((x >= 240 && x < 400) && (y >= 180 && y < 300)) {
	  	// fade out to black
	  	Flags.set(FADEOUT);
	  	
	  	// change to gamemode 2 in 50 frames
	  	GM_ChangeGamemode(gm, 2, 50);
	  	
	  	// play sfx
//	  	Sound_PlaySFX(SFXStartGame, 1);
	  	
	  	// stop music and delay program
//	  	Sound_Stop(2000);
	  	SDL_Delay(1000);
	  }
	}
	
return 0;
}

// start of gamemode
int _gm_menu_init(GameMode* const gm, const PROGRAM& program) {
	using namespace game;
	
	// open background image; print error message in background if it fails
	BG_BG1.open("images/backgrounds/grassy_field.png");
	
	if (BG_BG1.error())
	  Log_Cerr("Failed to load background image grassy_field.png: %s\n", BG_BG1.get_error());
	
	// open button UI image
	BG_BG2.open("images/ui/button_click_here.png");
	
	if (BG_BG2.error()) {
	  cerr << BG_BG2.get_error() << " -- aborting program\n";
	  return 1;
	}
	
	// open dialogue box image
	BG_DIALOGUEBOX.open("images/ui/dbox.png");
	
	if (BG_DIALOGUEBOX.error()) {
	  cerr << BG_DIALOGUEBOX.get_error() << " -- aborting program\n";
	  return 1;
	}
	
	// load menu music
//	Sound_LoadMusic("menu.ogg", MTLevel);
	
	// play the music after a 800 miliseconds
	SDL_TimerID timer = SDL_AddTimer(800, timerCallback_playMenuMusic, nullptr);
	
	if (! timer)
	  std::cerr << "Failed to initialize timer: " << SDL_GetError() << '\n';
	
	// fade in from black
	Flags.set(FADEIN);
	
return 2;
}

// run every frame after init
int _gm_menu_blit(GameMode* const gm, const PROGRAM& program) {
	using game::Flags;
	
	// background, button
	BG_BG1.blit();
	BG_BG2.blit(240,180);
	
	// black rectangle
	if (Flags.mask(FADING))
	  BG_BLACK.blit();
	
return 0;
}

// cleanup function
int _gm_menu_cleanup(GameMode* const gm, const PROGRAM& program) {
	// unload the background and button images
	BG_BG1.unload();
	BG_BG2.unload();
	
	// free the music that was just played
//	Sound_Free(MTLevel);
	
return 0;
}

// router function
int gm_menu(void* const gamemode_void, const PROGRAM& program) {
	typedef int (*FPI_GameModeSubFunction)(GameMode* const, const PROGRAM&);
	
	static const FPI_GameModeSubFunction functions[3] = {
	  _gm_menu_blit,
	  _gm_menu_init,
	  _gm_menu_cleanup
	};
	
	GameMode* gm = (GameMode*) gamemode_void;
	short int index = (gm->tm < 3) ? gm->tm : 0;
	
return functions[index](gm, program);
}

// timer callback
uint32_t timerCallback_playMenuMusic(uint32_t interval, void* data) {
	Log_Cout("Music played after %u miliseconds\n", interval);
	
	// player the music for one iteration
//	Sound_PlayMusicOnce(MTLevel);
	
return 0;
}
