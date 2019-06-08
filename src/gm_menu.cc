#include "game.hpp"
#include "gm_menu.hpp"
#include "event_defines.hpp"
#include "log.hpp"
#include "tmath.hpp"

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
	  	GM_ChangeGamemode(gm, 2, miliseconds_to_frames(2000));
	  	
	  	// stop music and delay program
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
	
	// play the music after a 800 miliseconds
	SDL_TimerID timer = SDL_AddTimer(800, timerCallback_playMenuMusic, nullptr);
	
	if (! timer)
	  std::cerr << "Failed to initialize timer: " << SDL_GetError() << '\n';
	
	// fade in from black
	Flags.set(FADEIN);
	
	// play windmill.wav
	{
	  MusData* temp = Sound_LoadMUSType("audio/music/windmill.wav", MUS_TYPE_WAV);
	  if (temp) {
	  	GM_SetData(gm, temp);
	  	MusData_SetVolume(temp, MAX_VOLUME - 30);
	  	Sound_PlayMusic(temp, 2);
	  	temp->lpos = 7.2;
	  }
	}
	
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
	
	// free music
	MusData* mus = (MusData*) gm->data;
	
	if (mus) {
	  Sound_FreeMUS(mus);
	  GM_ClearData(gm);
	}
	
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
