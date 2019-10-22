// engine headers
#include "game.hpp"
#include "gm_splash.hpp"
#include "sound.hpp"
#include "log.hpp"

// other headers
#include "endian.hpp"
#include "lvalue_rvalue_pointers.hpp"

using namespace std;

static int _gm_splash_cleanup(GameMode* const gm, const PROGRAM& program) {
	// free array
	PDTexture** myTextures = (PDTexture**) gm->data;
	delete[] myTextures;
	
	GM_ClearData(gm);
	
return 0;
}

static int _gm_splash_blit(GameMode* const gm, const PROGRAM& program) {
	PDTexture** myTextures = (PDTexture**) gm->data;
	
	// fade out to black after (150 - 97) frames
	if (gm->tm == 97)
	  game::Flags.set(FADEOUT);
	
	// render black screen OVER the splash image
	myTextures[1]->Blit();
	myTextures[0]->Blit();
	
return 0;
}

static int _gm_splash_screen_load_image(GameMode* const gm, const PROGRAM& program) {
	PDTexture** myTextures = new PDTexture*[2];
	
	myTextures[0] = get_pointer( RManager.LoadTexture(FADER, "images/ui/black.bmp", nullptr) );
	myTextures[0]->SetAlpha(0);
	myTextures[1] = get_pointer( RManager.LoadTexture(TEXTURE, "images/ui/splash.bmp", nullptr) );
	
	// change gamemode
	GM_SetData(gm, (void*) myTextures);
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
