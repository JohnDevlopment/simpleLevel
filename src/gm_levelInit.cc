#include "game.hpp"
#include "gm_levelInit.hpp"
#include "levelcode.hpp"
#include "log.hpp"
#include "sound.hpp"
#include "tcl.hpp"
#include "texture_packs.hpp"
#include "tile_collision.hpp"
#include "triggers.hpp"

using namespace std;

int gm_levelInit(void* const vgm, const PROGRAM& pgrm) {
	GameMode* gm = (GameMode*) vgm;
	
	BG_BLACK.blit();
	
	// change the gamemode if the INIT routine has already went
	if (! gm->tm) {
	  GM_ChangeGamemode(gm, 3, 3);
	  return 0;
	}
	
	// return if the timer is >= 2
	else if (gm->tm >= 2)
	  return 0;
	
	// load the level file into memory
	//level::CurrentLevel
	if (level::load("levels/rt1-1.dat", pgrm)) {
	  level::unload();
	  return 1;
	}
	
	// reset the background color
	SDL_SetRenderDrawColor(pgrm.renderer, 0, 0, 0, 0xff);
	
return 0;
}
