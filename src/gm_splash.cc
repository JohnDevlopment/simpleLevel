#include "game.hpp"
#include "gm_splash.hpp"
#include "endian.hpp"
#include "sound.hpp"
#include "log.hpp"

using namespace std;

static int _gm_splash_cleanup(GameMode* const gm, const PROGRAM& program) {
	delete reinterpret_cast<Image*>(gm->data);
	GM_ClearData(gm);
	
return 0;
}

static int _gm_splash_blit(GameMode* const gm, const PROGRAM& program) {
	using game::Graphics;
	
	// fade out to black after (150 - 97) frames
	if (gm->tm == 97) {
	  game::Flags.set(FADEOUT);
	}
	
	// render two images
	reinterpret_cast<Image*>(gm->data)->blit();
	BG_BLACK.blit();
	
return 0;
}

static int _gm_splash_screen_load_image(GameMode* const gm, const PROGRAM& program) {
	Image* bg = new Image("images/ui/splash.bmp", program.renderer);
	
	if (bg->error())
	  Log_Cerr("Failed to load \"images/ui/splash.bmp\" -- %s\n", bg->get_error());
	else
	  bg->set_blit_size(WIDTH, HEIGHT);
	
	// set no opacity on black screen
	BG_BLACK.set_alpha(0);
	
	// set timer for splash image to be 150 frames
	GM_SetData(gm, bg);
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
