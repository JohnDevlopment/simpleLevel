#include "game.hpp"
#include "gm_menu.hpp"
#include "event_defines.hpp"
#include "log.hpp"
#include "tmath.hpp"

using namespace std;

static MusData* MenuMusic = nullptr;

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
	  	
	  	// change to gamemode 2 in 50 frames; stop music; play sfx
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
	Sound_PlayMusic(MenuMusic, 1);
	
return 0;
}

// start of gamemode
static int _gm_menu_init(GameMode* const gm, const PROGRAM& program) {
	using namespace game;
	
	Image* bgs = new Image[2];
	SDL_TimerID timer = 0;
	
	// set renderers
	bgs[0].renderer = program.renderer;
	bgs[1].renderer = program.renderer;
	
	// open background image; print error message in background if it fails
	bgs[0].open("images/backgrounds/grassy_field.png");
	
	if (bgs[0].error())
	  Log_Cerr("Failed to load background image grassy_field.png: %s\n", bgs[0].get_error());
	
	// open button UI image
	bgs[1].open("images/ui/button_click_here.png");
	
	if (bgs[1].error()) {
	  cerr << bgs[1].get_error() << " -- aborting program\n";
	  return 1;
	}
	
	// load music
	MenuMusic = Sound_LoadMUSType("audio/music/menu.ogg", MUS_TYPE_OGG);
	if (MenuMusic)
	  MusData_SetVolume(MenuMusic, 45);
	
	// play the music after a 800 miliseconds
	timer = SDL_AddTimer(800, timerCallback_playMenuMusic, MenuMusic);
	if (! timer)
	  std::cerr << "Failed to initialize timer: " << SDL_GetError() << '\n';
	
	// fade in from black
	Flags.set(FADEIN);
	
	// save bg images to GameMode
	GM_SetData(gm, bgs);
	
return 2;
}

// run every frame after init
static int _gm_menu_blit(GameMode* const gm, const PROGRAM& program) {
	Image* bgs = (Image*) gm->data;
	
	// background, button
	bgs[0].blit();
	bgs[1].blit(240, 180);
	
	// black rectangle
	BG_BLACK.blit();
	
return 0;
}

// cleanup function
static int _gm_menu_cleanup(GameMode* const gm, const PROGRAM& program) {
	// free ui images
	delete[] (Image*) gm->data;
	GM_ClearData(gm);
	
	// free music
	Sound_FreeMUS(MenuMusic);
	
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
