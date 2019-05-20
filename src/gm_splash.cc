// global game data
#include "headers/game.hpp"

// this gamemode's header
#include "headers/gm_splash.hpp"

using namespace std;

// private function declarations
static int _gm_splash_cleanup(GameMode* const, const PROGRAM&);
static int _gm_splash_screen_load_image(GameMode* const, const PROGRAM&);
static int _gm_splash_blit(GameMode* const, const PROGRAM&);

// gamemode router function
int gm_splash_screen(void* const gamemode_void, const PROGRAM& program) {
	typedef int (*FPI_GameModeFunc)(GameMode* const, const PROGRAM&);
	
	static const FPI_GameModeFunc functions[3] = {
	  _gm_splash_blit,
	  _gm_splash_screen_load_image,
	  _gm_splash_cleanup
	};
	
	unsigned short int index;
	
	{
	  GameMode* gm = (GameMode*) gamemode_void;
	  index = (gm->tm < 3) ? gm->tm : 0;
	}
	
return functions[index](reinterpret_cast<GameMode*>(gamemode_void), program);
}

// private functions
int _gm_splash_cleanup(GameMode* const gm, const PROGRAM& program) {
	BG_BG1.unload();
	
return 0;
}

int _gm_splash_blit(GameMode* const gm, const PROGRAM& program) {
	using game::Graphics;
	
	// fade out to black after (150 - 97) frames
	if (gm->tm == 97) {
	  game::Flags.set(FADEOUT);
	}
	
	// render two images
	BG_BG1.blit();
	BG_BLACK.blit();
	
return 0;
}

int _gm_splash_screen_load_image(GameMode* const gm, const PROGRAM& program) {
	// set no opacity on black screen
	BG_BLACK.set_alpha(0);
	
	// open image and resize it to fit the screen
	if (! BG_BG1.open("images/ui/splash.bmp"))
	  cerr << BG_BG1.get_error() << '\n';
	else
	  BG_BG1.set_blit_size(WIDTH, HEIGHT);
	
	// set timer for splash image to be 150 frames
	GM_ChangeGamemode(gm, 1, 150);
	
	
return 0;
}
