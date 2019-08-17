#include "game.hpp"
#include "gm_menu.hpp"
#include "event_defines.hpp"
#include "log.hpp"
#include "tmath.hpp"

using namespace std;

// mouse input function
int gm_menu_event_mousebutton(SDL_Event& event, GameMode* const gm, uint8_t mask) {
	using game::Flags;
	
	// no mouse input when fading screen
	if (Flags.mask(FADING))
	  return 0;
	
	// if the left mouse button was clicked
	if (sdl_buttonstate(event) == EVENT_PRESSED && sdl_left(event)) {
	  int x, y;
	  
	  SDL_GetMouseState(&x, &y);
	  
	  // if the mouse was clicked between (240,180) and (400,300)
	  if ((x >= 240 && x < 400) && (y >= 180 && y < 300)) {
	  	// fade out to black
	  	Flags.set(FADEOUT);
	  	
	  	// change to gamemode 2 in 50 frames
	  	GM_ChangeGamemode(gm, 2, miliseconds_to_frames(2000));
	  	Sound_FadeOutMusic(1000);
	  	Sound_PlaySFX(SFXStartGame, 0);
	  	SDL_Delay(1000);
	  }
	}
	
return 0;
}

// timer callback
static uint32_t timerCallback_playMenuMusic(uint32_t interval, void* data) {
	// player the music for one iteration
	MusData* mus = (MusData*) data;
	if (mus) {
	  Sound_PlayMusic(mus, 1);
	}
	
return 0;
}

// start of gamemode
static int _gm_menu_init(GameMode* const gm, const PROGRAM& program) {
	using namespace game;
	
	MusData* mus;
	SDL_TimerID timer = 0;
	
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
	
	// load music
	mus = Sound_LoadMUSType("audio/music/menu.ogg", MUS_TYPE_OGG);
	if (mus) {
	  GM_SetData(gm, mus);
	  MusData_SetVolume(mus, 45);
	}
	
	// play the music after a 800 miliseconds
	timer = SDL_AddTimer(800, timerCallback_playMenuMusic, mus);
	
	if (! timer)
	  std::cerr << "Failed to initialize timer: " << SDL_GetError() << '\n';
	
	// fade in from black
	Flags.set(FADEIN);
	
return 2;
}

// run every frame after init
static int _gm_menu_blit(GameMode* const gm, const PROGRAM& program) {
	using game::Flags;
	
	// background, button
	BG_BG1.blit();
	BG_BG2.blit(240,180);
	
	// black rectangle
	BG_BLACK.blit();
	
return 0;
}

// cleanup function
static int _gm_menu_cleanup(GameMode* const gm, const PROGRAM& program) {
	// unload the background and button images
	BG_BG1.unload();
	BG_BG2.unload();
	
	// free music
	Sound_FreeMUS(reinterpret_cast<MusData*>(gm->data));
	GM_ClearData(gm);
	
return 0;
}

// router function
int gm_menu(void* const gamemode_void, const PROGRAM& program) {
	typedef int (*FPI_GameModeSubFunction)(GameMode* const, const PROGRAM&);
	
	GameMode* gm = (GameMode*) gamemode_void;
	short int iIndex = (gm->tm < 3) ? gm->tm : 0;
	
	static const FPI_GameModeSubFunction functions[3] = {
	  _gm_menu_blit,
	  _gm_menu_init,
	  _gm_menu_cleanup
	};
	
return functions[iIndex](gm, program);
}
