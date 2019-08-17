#include "game.hpp"
#include "gm_splash.hpp"
#include "endian.hpp"
#include "sound.hpp"

using namespace std;

static int _gm_splash_cleanup(GameMode* const gm, const PROGRAM& program) {
	BG_BG1.unload();
	
return 0;
}

static int _gm_splash_blit(GameMode* const gm, const PROGRAM& program) {
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

static int _gm_splash_screen_load_image(GameMode* const gm, const PROGRAM& program) {
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

// gamemode router function
int gm_splash_screen(void* const gamemode_void, const PROGRAM& program) {
	typedef int (*FPI_GameModeFunc)(GameMode* const, const PROGRAM&);
	
	GameMode* gm = (GameMode*) gamemode_void;
	unsigned short int iIndex;
	
	static const FPI_GameModeFunc functions[3] = {
	  _gm_splash_blit,
	  _gm_splash_screen_load_image,
	  _gm_splash_cleanup
	};
	
	iIndex = gm->tm < 3 ? gm->tm : 0;
	
return functions[iIndex](reinterpret_cast<GameMode*>(gamemode_void), program);
}
